/*
 * Task 9: K-Means Clustering
 * ---------------------------
 * Cluster n points into k clusters using three algorithmic paradigms:
 *   1. Brute Force          – Enumerate all k^n possible assignments  (O(k^n * n))
 *   2. Iterative Improvement – Lloyd's algorithm / k-means             (O(I * n * k))
 *   3. Divide and Conquer   – Split, recurse, merge, refine            (O(n log n * k))
 *
 * Objective: minimize the total Within-Cluster Sum of Squared distances (WCSS).
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>
#include <numeric>

using namespace std;

// ========================== DATA STRUCTURES ==========================

struct Point {
    double x, y;
};

struct ClusterResult {
    vector<int>   assignment; // assignment[i] = cluster id of point i
    vector<Point> centroids;  // centroids[c] = center of cluster c
    double        cost;       // total WCSS
    int           iterations; // meaningful for iterative improvement
};

// ========================== UTILITY FUNCTIONS ==========================

// Squared Euclidean distance
double distSq(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx * dx + dy * dy;
}

// Compute centroids from a given assignment
vector<Point> computeCentroids(const vector<Point>& pts,
                               const vector<int>&   assign,
                               int k) {
    vector<Point> centroids(k, {0.0, 0.0});
    vector<int>   count(k, 0);
    for (int i = 0; i < (int)pts.size(); i++) {
        int c = assign[i];
        centroids[c].x += pts[i].x;
        centroids[c].y += pts[i].y;
        count[c]++;
    }
    for (int c = 0; c < k; c++) {
        if (count[c] > 0) {
            centroids[c].x /= count[c];
            centroids[c].y /= count[c];
        }
    }
    return centroids;
}

// Total Within-Cluster Sum of Squared distances
double computeWCSS(const vector<Point>& pts,
                   const vector<int>&   assign,
                   const vector<Point>& centroids) {
    double cost = 0.0;
    for (int i = 0; i < (int)pts.size(); i++)
        cost += distSq(pts[i], centroids[assign[i]]);
    return cost;
}

// Assign every point to the nearest centroid; returns true if any changed
bool assignToNearest(const vector<Point>& pts,
                     const vector<Point>& centroids,
                     int k,
                     vector<int>& assign) {
    bool changed = false; 
    for (int i = 0; i < (int)pts.size(); i++) {
        int    bestC = 0;
        double bestD = numeric_limits<double>::max();
        for (int c = 0; c < k; c++) {
            double d = distSq(pts[i], centroids[c]);
            if (d < bestD) { bestD = d; bestC = c; }
        }
        if (assign[i] != bestC) { assign[i] = bestC; changed = true; }
    }
    return changed;
}

// Pretty-print a clustering result
void printResult(const string& title,
                 const vector<Point>& pts,
                 const ClusterResult& res,
                 double elapsedMs) {
    cout << "\n===== " << title << " =====\n";
    cout << fixed << setprecision(4);
    cout << "WCSS (cost)  : " << res.cost << "\n";
    if (res.iterations >= 0)
        cout << "Iterations   : " << res.iterations << "\n";
    cout << "Time         : " << elapsedMs << " ms\n";

    cout << "Centroids    : ";
    for (int c = 0; c < (int)res.centroids.size(); c++)
        cout << "(" << res.centroids[c].x << ", " << res.centroids[c].y << ")  ";
    cout << "\n";

    // Show cluster membership
    int k = (int)res.centroids.size();
    for (int c = 0; c < k; c++) {
        cout << "  Cluster " << c << ": ";
        bool first = true;
        for (int i = 0; i < (int)pts.size(); i++) {
            if (res.assignment[i] == c) {
                if (!first) cout << ", ";
                cout << "(" << pts[i].x << ", " << pts[i].y << ")";
                first = false;
            }
        }
        cout << "\n";
    }
}

// =====================================================================
//  1.  BRUTE FORCE
// =====================================================================
/*
 * Enumerate ALL possible assignments of n points to k clusters.
 * Total partitions = k^n  (only valid ones where every cluster is non-empty).
 * For each valid partition, compute WCSS and keep the best.
 *
 * Time  : O(k^n * n)
 * Space : O(n)
 * Only feasible for very small n (recommended n <= 15).
 */

void bruteForceHelper(const vector<Point>& pts, int k,
                      int idx, vector<int>& cur,
                      ClusterResult& best) {
    int n = (int)pts.size();
    if (idx == n) {
        // Verify every cluster has at least one point
        vector<bool> used(k, false);
        for (int c : cur) used[c] = true;
        for (int c = 0; c < k; c++)
            if (!used[c]) return;

        vector<Point> centroids = computeCentroids(pts, cur, k);
        double cost = computeWCSS(pts, cur, centroids);
        if (cost < best.cost) {
            best.cost       = cost;
            best.assignment = cur;
            best.centroids  = centroids;
        }
        return;
    }
    for (int c = 0; c < k; c++) {
        cur[idx] = c;
        bruteForceHelper(pts, k, idx + 1, cur, best);
    }
}

ClusterResult bruteForce(const vector<Point>& pts, int k) {
    int n = (int)pts.size();
    ClusterResult best;
    best.cost       = numeric_limits<double>::max();
    best.iterations = -1; // not applicable
    vector<int> cur(n, 0);
    bruteForceHelper(pts, k, 0, cur, best);
    return best;
}

// =====================================================================
//  2.  ITERATIVE IMPROVEMENT  (Lloyd's / K-Means)
// =====================================================================
/*
 * Classic k-means with k-means++ initialization:
 *   1. Pick initial centroids with k-means++ (weighted random seeding).
 *   2. Repeat until convergence or maxIter:
 *        a. Assign every point to the nearest centroid.
 *        b. Recompute centroids as the mean of their members.
 *
 * Time  : O(I * n * k)   where I = number of iterations until convergence
 * Space : O(n + k)
 */

ClusterResult iterativeImprovement(const vector<Point>& pts, int k,
                                   int maxIter = 1000, int seed = 42) {
    int n = (int)pts.size();
    mt19937 rng(seed);

    // ---------- k-means++ seeding ----------
    vector<Point> centroids;
    uniform_int_distribution<int> uniIdx(0, n - 1);
    centroids.push_back(pts[uniIdx(rng)]);

    vector<double> minD(n);
    for (int i = 1; i < k; i++) {
        double total = 0.0;
        for (int j = 0; j < n; j++) {
            double best = numeric_limits<double>::max();
            for (auto& c : centroids)
                best = min(best, distSq(pts[j], c));
            minD[j] = best;
            total += best;
        }
        uniform_real_distribution<double> uniReal(0.0, total);
        double r   = uniReal(rng);
        double cum = 0.0;
        for (int j = 0; j < n; j++) {
            cum += minD[j];
            if (cum >= r) { centroids.push_back(pts[j]); break; }
        }
    }

    // ---------- Main loop ----------
    vector<int> assign(n, 0);
    int iter = 0;
    for (; iter < maxIter; iter++) {
        bool changed = assignToNearest(pts, centroids, k, assign);
        if (!changed && iter > 0) break;
        centroids = computeCentroids(pts, assign, k);
    }

    double cost = computeWCSS(pts, assign, centroids);
    return {assign, centroids, cost, iter};
}

// =====================================================================
//  3.  DIVIDE AND CONQUER
// =====================================================================
/*
 * Strategy:
 *   1. Sort points by x-coordinate.
 *   2. Split into two halves.
 *   3. Assign ceil(k/2) clusters to the left half,
 *      and floor(k/2) to the right half.
 *   4. Recursively cluster each half.
 *   5. Merge the two sets of centroids and refine
 *      with a few Lloyd iterations over ALL points.
 *
 * Base case: when n <= 2*k or n is very small,
 *            fall back to iterative improvement.
 *
 * Time  : O(n log n * k)   (dominated by sorting + merge-refine passes)
 * Space : O(n log n)
 */

// Internal recursive helper (works on indexed points to map back)
ClusterResult dacHelper(vector<pair<Point, int>>& idxPts, int k, int seed) {
    int n = (int)idxPts.size();
    int actualK = min(k, n);

    // ---- Base case ----
    // Also base-case when k <= 1 (can't split clusters further)
    if (k <= 1 || n <= 2 * k || n <= 10) {
        vector<Point> sub(n);
        for (int i = 0; i < n; i++) sub[i] = idxPts[i].first;
        auto res = iterativeImprovement(sub, actualK, 300, seed);
        return res;
    }

    // ---- Sort by x-coordinate ----
    sort(idxPts.begin(), idxPts.end(),
         [](const pair<Point, int>& a, const pair<Point, int>& b) {
             return a.first.x < b.first.x;
         });

    int mid = n / 2;
    int k1 = (k + 1) / 2;   // ceil(k/2), guaranteed >= 1 since k >= 2
    int k2 = k - k1;         // floor(k/2), guaranteed >= 1 since k >= 2

    vector<pair<Point, int>> left(idxPts.begin(), idxPts.begin() + mid);
    vector<pair<Point, int>> right(idxPts.begin() + mid, idxPts.end());

    auto leftRes  = dacHelper(left,  k1, seed);
    auto rightRes = dacHelper(right, k2, seed + 1);

    // ---- Merge centroids ----
    vector<Point> mergedCentroids;
    for (auto& c : leftRes.centroids)  mergedCentroids.push_back(c);
    for (auto& c : rightRes.centroids) mergedCentroids.push_back(c);

    // Gather all points (in current order)
    vector<Point> allPts(n);
    for (int i = 0; i < n; i++) allPts[i] = idxPts[i].first;

    // ---- Refine with Lloyd iterations using merged centroids ----
    vector<int> assign(n, 0);
    assignToNearest(allPts, mergedCentroids, k, assign);

    const int REFINE_ITER = 50;
    for (int it = 0; it < REFINE_ITER; it++) {
        mergedCentroids = computeCentroids(allPts, assign, k);
        if (!assignToNearest(allPts, mergedCentroids, k, assign))
            break;
    }
    mergedCentroids = computeCentroids(allPts, assign, k);

    double cost = computeWCSS(allPts, assign, mergedCentroids);
    return {assign, mergedCentroids, cost, -1};
}

ClusterResult divideAndConquer(const vector<Point>& pts, int k) {
    int n = (int)pts.size();
    vector<pair<Point, int>> idxPts(n);
    for (int i = 0; i < n; i++) idxPts[i] = {pts[i], i};

    auto res = dacHelper(idxPts, k, 42);

    // Map assignments back to original point order
    vector<int> finalAssign(n);
    for (int i = 0; i < n; i++)
        finalAssign[idxPts[i].second] = res.assignment[i];

    // Recompute centroids in original order
    vector<Point> centroids = computeCentroids(pts, finalAssign, k);
    double cost = computeWCSS(pts, finalAssign, centroids);
    return {finalAssign, centroids, cost, -1};
}

// =====================================================================
//  MAIN
// =====================================================================

int main() {
    cout << "========================================\n";
    cout << "   K-Means Clustering  –  Task 9\n";
    cout << "========================================\n\n";

    int mode;
    cout << "Choose input mode:\n";
    cout << "  1. Manual input\n";
    cout << "  2. Auto-generate random test data\n";
    cout << "Choice: ";
    cin >> mode;

    int n, k;
    vector<Point> points;

    if (mode == 1) {
        // ---- Manual input ----
        cout << "Enter number of points (n): ";
        cin >> n;
        cout << "Enter number of clusters (k): ";
        cin >> k;

        points.resize(n);
        cout << "Enter " << n << " points (x y), one per line:\n";
        for (int i = 0; i < n; i++)
            cin >> points[i].x >> points[i].y;
    } else {
        // ---- Auto-generate ----
        cout << "Enter number of points (n): ";
        cin >> n;
        cout << "Enter number of clusters (k): ";
        cin >> k;

        // Generate k well-separated Gaussian blobs
        mt19937 rng(123);
        normal_distribution<double>  noise(0.0, 1.5);
        uniform_real_distribution<double> pos(-20.0, 20.0);

        // Random cluster centers
        vector<Point> trueCenters(k);
        for (int c = 0; c < k; c++)
            trueCenters[c] = {pos(rng), pos(rng)};

        uniform_int_distribution<int> pickCluster(0, k - 1);
        points.resize(n);
        for (int i = 0; i < n; i++) {
            int c = pickCluster(rng);
            points[i].x = trueCenters[c].x + noise(rng);
            points[i].y = trueCenters[c].y + noise(rng);
        }

        cout << "\nGenerated " << n << " points around " << k << " true centers:\n";
        for (int c = 0; c < k; c++)
            cout << "  Center " << c << ": ("
                 << fixed << setprecision(2)
                 << trueCenters[c].x << ", " << trueCenters[c].y << ")\n";
        cout << "\nPoints:\n";
        for (int i = 0; i < n; i++)
            cout << "  (" << points[i].x << ", " << points[i].y << ")\n";
    }

    if (k > n) {
        cout << "\nError: k cannot be greater than n.\n";
        return 1;
    }

    // ---- Run all three methods ----

    // 1. Brute Force (only if feasible)
    const int BF_LIMIT = 20;
    if (n <= BF_LIMIT) {
        auto t0  = chrono::high_resolution_clock::now();
        auto res = bruteForce(points, k);
        auto t1  = chrono::high_resolution_clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();
        printResult("BRUTE FORCE (optimal)", points, res, ms);
    } else {
        cout << "\n===== BRUTE FORCE =====\n";
        cout << "Skipped: n = " << n << " > " << BF_LIMIT
             << " (would be too slow, O(k^n)).\n";
    }

    // 2. Iterative Improvement (Lloyd's / k-means)
    {
        auto t0  = chrono::high_resolution_clock::now();
        auto res = iterativeImprovement(points, k);
        auto t1  = chrono::high_resolution_clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();
        printResult("ITERATIVE IMPROVEMENT (Lloyd's k-means)", points, res, ms);
    }

    // 3. Divide and Conquer
    {
        auto t0  = chrono::high_resolution_clock::now();
        auto res = divideAndConquer(points, k);
        auto t1  = chrono::high_resolution_clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();
        printResult("DIVIDE AND CONQUER", points, res, ms);
    }

    cout << "\n========================================\n";
    cout << "   Comparison complete.\n";
    cout << "========================================\n";

    return 0;
}
