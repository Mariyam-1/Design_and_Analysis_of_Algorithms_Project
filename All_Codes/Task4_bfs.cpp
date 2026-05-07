#include <bits/stdc++.h>
using namespace std;

struct Node {
    string state;
    vector<pair<int,int>> path;
};

void printBoard(string s) {
    for (int i = 0; i < 12; i++) {
        cout << s[i] << " ";
        if ((i + 1) % 3 == 0) cout << "\n";
    }
    cout << "------\n";
}

int main() {
    int rows = 4, cols = 3;

    string start = "BBB......WWW";
    string goal  = "WWW......BBB";

    vector<pair<int,int>> moves = {
        {2,1},{2,-1},{-2,1},{-2,-1},
        {1,2},{1,-2},{-1,2},{-1,-2}
    };

    queue<Node> q;
    unordered_set<string> vis;

    q.push({start, {}});
    vis.insert(start);

    while (!q.empty()) {
        Node cur = q.front();
        q.pop();

        if (cur.state == goal) {
            cout << "Minimum moves = " << cur.path.size() << "\n\n";

            string temp = start;

            cout << "Start:\n";
            printBoard(temp);

            for (int i = 0; i < cur.path.size(); i++) {
                int from = cur.path[i].first;
                int to   = cur.path[i].second;

                swap(temp[from], temp[to]);

                cout << "Move " << i + 1 << ": "
                     << from << " -> " << to << "\n";

                printBoard(temp);
            }

            cout << "Goal reached\n";
            printBoard(goal);

            return 0;
        }

        for (int i = 0; i < 12; i++) {
            if (cur.state[i] == '.') continue;

            int r = i / cols;
            int c = i % cols;

            for (auto mv : moves) {
                int nr = r + mv.first;
                int nc = c + mv.second;

                if (nr < 0 || nr >= rows || nc < 0 || nc >= cols)
                    continue;

                int j = nr * cols + nc;

                if (cur.state[j] != '.') continue;

                string next = cur.state;
                swap(next[i], next[j]);

                if (!vis.count(next)) {
                    vis.insert(next);

                    auto newPath = cur.path;
                    newPath.push_back({i, j});

                    q.push({next, newPath});
                }
            }
        }
    }

    cout << "No solution\n";
    return 0;
}
