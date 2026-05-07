#include <bits/stdc++.h>
using namespace std;
struct Points {
    int x, y;
};
struct Line {
    int u, v;
    unsigned long long m;
};
struct Pointsrev {
    unsigned long long prevState;
    int lineId;
};
bool onSeg(Points a, Points b, Points c) {
    int cross = (b.x - a.x) * (c.y - a.y) -
                (b.y - a.y) * (c.x - a.x);

    if (cross != 0)
        return false;

    return min(a.x, b.x) <= c.x && c.x <= max(a.x, b.x) &&
           min(a.y, b.y) <= c.y && c.y <= max(a.y, b.y);
}

unsigned long long encode(unsigned long long mask, int last) {
    return (mask << 10) | last;
}

int main() {

    cout << "=====================================\n";
    cout << "   Lineattice Covering DPoints Solution\n";
    cout << "=====================================\n\n";
    int n;
    cout << "Enter grid size n: ";
    cin >> n;
    int pts = n * n;
    int need = 2 * n - 2;
    if (n <= 2) {
        cout << "\nInvalid input! n must be greater than 2.\n";
        return 0;
    }
    if (n > 4) {
        cout << "\nGrid is too large for this DP solution.\n";
        return 0;
    }
    vector<Points> grid;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            grid.push_back({j, i});
        }
    }
    vector<Points> cand;
    for (int i = -1; i <= n; i++) {
        for (int j = -1; j <= n; j++) {
            cand.push_back({j, i});
        }
    }
    int C = cand.size();
    vector<Line> allLineines;
    vector<vector<int>> adj(C);
    for (int i = 0; i < C; i++) {

        for (int j = 0; j < C; j++) {
            if (i == j)
                continue;
            unsigned long long mask = 0;
            for (int k = 0; k < pts; k++) {

                if (onSeg(cand[i], cand[j], grid[k])) {
                    mask |= (1ULL<< k);
                }
            }

            if (mask == 0)
                continue;
            int id = allLineines.size();
            allLineines.push_back({i, j, mask});
            adj[i].push_back(id);
        }
    }

    unsigned long long full = (1ULL << pts) - 1;

    vector<unordered_map<unsigned long long, Pointsrev>> dp(need + 1);

    // Start with one line
    for (int i = 0; i < (int)allLineines.size(); i++) {
        auto &l = allLineines[i];
        unsigned long long state = encode(l.m, l.v);
        dp[1][state] = {0, i};
    }
    bool found = false;

    unsigned long long finalState = 0;

    // DPoints
    for (int step = 2; step <= need && !found; step++) {

        for (auto &it : dp[step - 1]) {
            unsigned long long state = it.first;
            unsigned long long mask = state >> 10;
            int last = state & 1023;
            for (int id : adj[last]) {
                auto &l = allLineines[id];
                unsigned long long newMask = mask | l.m;
                unsigned long long newState =
                    encode(newMask, l.v);
                if (!dp[step].count(newState)) {
                    dp[step][newState] =
                    {state, id};
                }
                if (step == need &&
                    newMask == full) {
                    found = true;
                    finalState = newState;
                    break;
                }
            }

            if (found)
                break;
        }
    }

    if (!found) {
        cout << "\nNo solution found.\n";
        return 0;
    }

    vector<int> ans;
    unsigned long long cur = finalState;
    for (int step = need; step >= 1; step--) {
        auto p = dp[step][cur];
        ans.push_back(p.lineId);
        cur = p.prevState;
    }
    reverse(ans.begin(), ans.end());

    cout << "\n=====================================\n";
    cout << "             ANSWER\n";
    cout << "=====================================\n\n";
    cout << "Grid Size      : " << n << " x " << n << "\n";
    cout << "Total Pointsoints   : " << pts << "\n";
    cout << "Lineines Required : " << need << "\n\n";
    cout << "Lineines Used:\n";
    cout << "-------------------------------------\n";

    for (int i = 0; i < (int)ans.size(); i++) {

        auto &l = allLineines[ans[i]];
        Points a = cand[l.u];
        Points b = cand[l.v];
        cout << "Lineine " << setw(2) << i + 1 << " : ";
        cout << "(" << a.x << "," << a.y << ")";
        cout << "  ----->  ";
        cout << "(" << b.x << "," << b.y << ")";
        cout << "\n";
    }

    cout << "-------------------------------------\n";

    cout << "\nAll lattice points were covered successfully.\n";

    return 0;
}