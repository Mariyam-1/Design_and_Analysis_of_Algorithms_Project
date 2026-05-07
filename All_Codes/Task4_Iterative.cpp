#include <bits/stdc++.h>
using namespace std;

int rows = 4, cols = 3;

vector<pair<int,int>> knightMoves = {
    {2,1},{2,-1},{-2,1},{-2,-1},
    {1,2},{1,-2},{1,-2},{-1,2},{-1,-2}
};

void printBoard(string s) {
    for (int i = 0; i < 12; i++) {
        cout << s[i] << " ";
        if ((i + 1) % 3 == 0) cout << "\n";
    }
    cout << "------\n";
}

// lower score = better
int heuristic(string s) {
    int score = 0;

    // W should go upward
    for (int i = 0; i < 12; i++) {
        int r = i / cols;

        if (s[i] == 'W') {
            score += r;        // W is better when row is smaller
        }
        else if (s[i] == 'B') {
            score += 3 - r;    // B is better when row is bigger
        }
    }

    return score;
}

int main() {
    string current = "BBB......WWW";
    string goal    = "WWW......BBB";

    vector<pair<int,int>> path;
    unordered_set<string> visited;

    visited.insert(current);

    cout << "Start:\n";
    printBoard(current);

    while (current != goal) {
        string bestState = current;
        int bestFrom = -1, bestTo = -1;
        int bestScore = INT_MAX;

        // Generate all possible neighbor states
        for (int i = 0; i < 12; i++) {
            if (current[i] == '.') continue;

            int r = i / cols;
            int c = i % cols;

            for (auto mv : knightMoves) {
                int nr = r + mv.first;
                int nc = c + mv.second;

                if (nr < 0 || nr >= rows || nc < 0 || nc >= cols)
                    continue;

                int j = nr * cols + nc;

                if (current[j] != '.') continue;

                string next = current;
                swap(next[i], next[j]);

                if (visited.count(next)) continue;

                int sc = heuristic(next);

                if (sc < bestScore) {
                    bestScore = sc;
                    bestState = next;
                    bestFrom = i;
                    bestTo = j;
                }
            }
        }

        // no better/new neighbor found
        if (bestFrom == -1) {
            cout << "Stuck in local optimum. No solution by pure iterative improvement.\n";
            return 0;
        }

        current = bestState;
        visited.insert(current);
        path.push_back({bestFrom, bestTo});

        cout << "Move " << path.size() << ": "
             << bestFrom << " -> " << bestTo << "\n";
        printBoard(current);
    }

    cout << "Goal reached\n";
    cout << "Total moves = " << path.size() << "\n";
    printBoard(goal);

    return 0;
}
