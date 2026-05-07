#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int u, v, w;
};

int cutValue(vector<Edge>& edges, vector<int>& group) {
    int cut = 0;
    for (Edge e : edges) {
        if (group[e.u] != group[e.v]) {
            cut += e.w;
        }
    }
    return cut;
}

void printGroups(vector<int>& group) {
    cout << "Set A: ";
    for (int i = 0; i < group.size(); i++)
        if (group[i] == 0)
            cout << i << " ";
    cout << "\nSet B: ";
    for (int i = 0; i < group.size(); i++)
        if (group[i] == 1)
            cout << i << " ";
    cout << endl;
}

void generateGroups(int index, int n, vector<int>& group, vector<Edge>& edges, int& bestCut, vector<int>& bestGroup) {
    // Base case: all vertices assigned
    if (index == n) {
        bool hasA = false;
        bool hasB = false;

        // check that we've 2 groups
        for (int x : group) {
            if (x == 0) hasA = true;
            if (x == 1) hasB = true;
        }

        // skip invalid partition
        if (!hasA || !hasB) return;

        int currentCut = cutValue(edges, group);

        if (currentCut < bestCut) {
            // determine the min_cut
            bestCut = currentCut;
            bestGroup = group;
        }
        return;
    }

    //try putting current vertex in set A or B
    for (int g = 0; g <= 1; g++) {
        group[index] = g;
        generateGroups(index + 1, n, group, edges, bestCut, bestGroup);
    }
}

void bruteForce(int n, vector<Edge>& edges) {
    int bestCut = INT_MAX;
    vector<int> bestGroup(n);
    vector<int> group(n, 0);

    generateGroups(0, n, group, edges, bestCut, bestGroup);

    cout << "\nBrute Force Minimum Cut = " << bestCut << endl;
    printGroups(bestGroup);
}

bool isValidPartition(vector<int>& group) {
    int count0 = 0, count1 = 0;

    for (int g : group) {
        if (g == 0) count0++;
        else count1++;
    }

    return (count0 > 0 && count1 > 0);
}

void iterativeImprovement(int n, vector<Edge>& edges) {
    vector<int> group(n, 0);

    // initial split
    for (int i = n / 2; i < n; i++) {
        group[i] = 1;
    }

    int currentCut = cutValue(edges, group);
    bool improved = true;

    while (improved) {
        improved = false;

        for (int i = 0; i < n; i++) {
            //move vertex to the other set
            group[i] = 1 - group[i];

            //prevent invalid partition
            if (!isValidPartition(group)) {
                group[i] = 1 - group[i];
                continue;
            }

            int newCut = cutValue(edges, group);

            if (newCut < currentCut) {
                currentCut = newCut;
                improved = true;
            } else {
                group[i] = 1 - group[i]; // undo
            }
        }
    }

    cout << "\nIterative Improvement Cut = " << currentCut << endl;
    printGroups(group);
}

int main() {
    int n, m;

    cout << "Enter number of vertices: ";
    cin >> n;

    cout << "Enter number of edges: ";
    cin >> m;

    vector<Edge> edges(m);

    cout << "Enter each edge as: u v weight\n";
    for (int i = 0; i < m; i++) {
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
    }

    bruteForce(n, edges);
    iterativeImprovement(n, edges);

    return 0;
}
