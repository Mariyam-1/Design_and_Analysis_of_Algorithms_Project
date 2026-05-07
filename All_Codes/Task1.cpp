#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <cmath>

using namespace std;

// ============================================================
//  Board representation
//  0  = empty
// -1  = missing square
//  positive integer = tromino ID
// ============================================================

int board[64][64];   // supports up to 2^6 = 64 x 64
int trominoCount = 0;

// Each tromino stores its 3 cells
struct Tromino {
    int id;
    int color;   // 1, 2, or 3
    vector<pair<int,int>> cells;
};

vector<Tromino> trominoes;

// Adjacency list: trominoID -> set of neighboring trominoIDs
map<int, set<int>> adjacency;

// ============================================================
//  DIVIDE AND CONQUER TILING
// ============================================================

void placeTromino(int r1, int c1, int r2, int c2, int r3, int c3) {
    trominoCount++;
    board[r1][c1] = trominoCount;
    board[r2][c2] = trominoCount;
    board[r3][c3] = trominoCount;

    Tromino t;
    t.id    = trominoCount;
    t.color = 0; // uncolored yet
    t.cells = {{r1,c1},{r2,c2},{r3,c3}};
    trominoes.push_back(t);
}

void tile(int rowStart, int colStart, int size, int missingRow, int missingCol) {
    // Base case: 2x2 board
    if (size == 2) {
        // Place one tromino covering the 3 non-missing cells
        for (int r = rowStart; r < rowStart + 2; r++) {
            for (int c = colStart; c < colStart + 2; c++) {
                if (!(r == missingRow && c == missingCol)) {
                    // collect the 3 cells and place
                }
            }
        }
        // Collect all 3 non-missing cells
        vector<pair<int,int>> cells;
        for (int r = rowStart; r < rowStart + 2; r++)
            for (int c = colStart; c < colStart + 2; c++)
                if (!(r == missingRow && c == missingCol))
                    cells.push_back({r, c});
        placeTromino(cells[0].first, cells[0].second,
                     cells[1].first, cells[1].second,
                     cells[2].first, cells[2].second);
        return;
    }

    int half      = size / 2;
    int centerRow = rowStart + half - 1;
    int centerCol = colStart + half - 1;

    // Determine which quadrant has the missing square
    bool missingInTL = (missingRow < rowStart + half) && (missingCol < colStart + half);
    bool missingInTR = (missingRow < rowStart + half) && (missingCol >= colStart + half);
    bool missingInBL = (missingRow >= rowStart + half) && (missingCol < colStart + half);
    // else: missing in BR

    if (missingInTL) {
        // Place center tromino in TR, BL, BR corners
        placeTromino(centerRow,   centerCol+1,
                     centerRow+1, centerCol,
                     centerRow+1, centerCol+1);
        tile(rowStart,      colStart,      half, missingRow,   missingCol  );
        tile(rowStart,      colStart+half, half, centerRow,    centerCol+1 );
        tile(rowStart+half, colStart,      half, centerRow+1,  centerCol   );
        tile(rowStart+half, colStart+half, half, centerRow+1,  centerCol+1 );
    }
    else if (missingInTR) {
        // Place center tromino in TL, BL, BR corners
        placeTromino(centerRow,   centerCol,
                     centerRow+1, centerCol,
                     centerRow+1, centerCol+1);
        tile(rowStart,      colStart,      half, centerRow,    centerCol   );
        tile(rowStart,      colStart+half, half, missingRow,   missingCol  );
        tile(rowStart+half, colStart,      half, centerRow+1,  centerCol   );
        tile(rowStart+half, colStart+half, half, centerRow+1,  centerCol+1 );
    }
    else if (missingInBL) {
        // Place center tromino in TL, TR, BR corners
        placeTromino(centerRow,   centerCol,
                     centerRow,   centerCol+1,
                     centerRow+1, centerCol+1);
        tile(rowStart,      colStart,      half, centerRow,    centerCol   );
        tile(rowStart,      colStart+half, half, centerRow,    centerCol+1 );
        tile(rowStart+half, colStart,      half, missingRow,   missingCol  );
        tile(rowStart+half, colStart+half, half, centerRow+1,  centerCol+1 );
    }
    else {
        // Missing in BR -- place center tromino in TL, TR, BL corners
        placeTromino(centerRow,   centerCol,
                     centerRow,   centerCol+1,
                     centerRow+1, centerCol);
        tile(rowStart,      colStart,      half, centerRow,    centerCol   );
        tile(rowStart,      colStart+half, half, centerRow,    centerCol+1 );
        tile(rowStart+half, colStart,      half, centerRow+1,  centerCol   );
        tile(rowStart+half, colStart+half, half, missingRow,   missingCol  );
    }
}

// ============================================================
//  BUILD ADJACENCY LIST
//  Two trominoes are adjacent if they share an edge
// ============================================================

void buildAdjacency(int size) {
    // Direction vectors for 4 edges of a cell
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (auto& t : trominoes) {
        for (auto& cell : t.cells) {
            int r = cell.first, c = cell.second;
            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d];
                int nc = c + dc[d];
                if (nr >= 0 && nr < size && nc >= 0 && nc < size) {
                    int neighborID = board[nr][nc];
                    if (neighborID > 0 && neighborID != t.id) {
                        adjacency[t.id].insert(neighborID);
                        adjacency[neighborID].insert(t.id);
                    }
                }
            }
        }
    }
}

// ============================================================
//  GREEDY 3-COLORING
// ============================================================

bool greedyColor() {
    for (auto& t : trominoes) {
        set<int> usedColors;
        for (int neighborID : adjacency[t.id]) {
            // Find the neighbor's color
            for (auto& nt : trominoes) {
                if (nt.id == neighborID && nt.color != 0)
                    usedColors.insert(nt.color);
            }
        }
        // Assign first available color
        for (int c = 1; c <= 3; c++) {
            if (usedColors.find(c) == usedColors.end()) {
                t.color = c;
                break;
            }
        }
        if (t.color == 0) return false; // failed
    }
    return true;
}

// ============================================================
//  BRUTE FORCE TILING  (backtracking)
// ============================================================

// 4 tromino orientations (offsets from top-left anchor):
// Each orientation covers 3 cells relative to anchor (r,c)
int orientations[4][3][2] = {
    {{0,0},{0,1},{1,0}},  // TL shape
    {{0,0},{0,1},{1,1}},  // TR shape
    {{0,0},{1,0},{1,1}},  // BL shape
    {{0,1},{1,0},{1,1}}   // BR shape
};

int bfBoard[64][64];
int bfSize;

bool bfTile(int pos) {
    // Find next empty cell from position pos
    while (pos < bfSize * bfSize && bfBoard[pos / bfSize][pos % bfSize] != 0)
        pos++;

    if (pos == bfSize * bfSize) return true; // all filled

    int r = pos / bfSize;
    int c = pos % bfSize;

    // Try all 4 orientations with (r,c) as anchor or as one of the cells
    // We try each orientation where one cell lands on (r,c)
    for (int ori = 0; ori < 4; ori++) {
        for (int startCell = 0; startCell < 3; startCell++) {
            // Compute anchor such that cell[startCell] = (r, c)
            int anchorR = r - orientations[ori][startCell][0];
            int anchorC = c - orientations[ori][startCell][1];

            // Check all 3 cells are valid and empty
            bool valid = true;
            int cells[3][2];
            for (int k = 0; k < 3; k++) {
                cells[k][0] = anchorR + orientations[ori][k][0];
                cells[k][1] = anchorC + orientations[ori][k][1];
                int cr = cells[k][0], cc = cells[k][1];
                if (cr < 0 || cr >= bfSize || cc < 0 || cc >= bfSize || bfBoard[cr][cc] != 0) {
                    valid = false;
                    break;
                }
            }
            if (!valid) continue;

            // Place tromino
            trominoCount++;
            int tid = trominoCount;
            for (int k = 0; k < 3; k++)
                bfBoard[cells[k][0]][cells[k][1]] = tid;

            if (bfTile(pos + 1)) return true;

            // Backtrack
            trominoCount--;
            for (int k = 0; k < 3; k++)
                bfBoard[cells[k][0]][cells[k][1]] = 0;
        }
    }
    return false;
}

// ============================================================
//  BRUTE FORCE 3-COLORING
// ============================================================

bool bfColor(vector<Tromino>& tromList, map<int,set<int>>& adj, int idx) {
    if (idx == (int)tromList.size()) {
        // Validate
        for (auto& t : tromList) {
            for (int nid : adj[t.id]) {
                for (auto& nt : tromList) {
                    if (nt.id == nid && nt.color == t.color) return false;
                }
            }
        }
        return true;
    }
    for (int c = 1; c <= 3; c++) {
        tromList[idx].color = c;
        if (bfColor(tromList, adj, idx + 1)) return true;
    }
    tromList[idx].color = 0;
    return false;
}

// ============================================================
//  DISPLAY
// ============================================================

void printBoard(int b[][64], int size, vector<Tromino>& tList) {
    // Build color map
    map<int,char> colorMap;
    for (auto& t : tList) colorMap[t.id] = t.color==1 ? 'R' : (t.color==2 ? 'G' : 'B');

    string colorChars = ".123";
    cout << "\nBoard (tromino IDs):\n";
    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            if (b[r][c] == -1)      cout << "  X";
            else if (b[r][c] == 0)  cout << "  .";
            else                    cout << "  " << b[r][c];
        }
        cout << "\n";
    }

    cout << "\nBoard (colors -- 1=Red, 2=Green, 3=Blue):\n";
    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            if (b[r][c] == -1)     cout << "  X";
            else if (b[r][c] == 0) cout << "  .";
            else                   cout << "  " << colorMap[b[r][c]];
        }
        cout << "\n";
    }
}

// ============================================================
//  MAIN
// ============================================================

int main() {
    cout << "=========================================\n";
    cout << "  Tromino Tiling -- Divide & Conquer\n";
    cout << "=========================================\n\n";

    int n;
    cout << "Enter n (board will be 2^n x 2^n, n >= 2): ";
    cin >> n;
    int size = 1 << n; // 2^n

    int mr, mc;
    cout << "Enter missing square row (0-indexed, 0 to " << size-1 << "): ";
    cin >> mr;
    cout << "Enter missing square col (0-indexed, 0 to " << size-1 << "): ";
    cin >> mc;

    // Initialize board
    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            board[r][c] = 0;
    board[mr][mc] = -1;

    cout << "\n--- Running Divide & Conquer Tiling ---\n";
    trominoCount = 0;
    trominoes.clear();
    adjacency.clear();

    tile(0, 0, size, mr, mc);
    buildAdjacency(size);

    bool colored = greedyColor();
    cout << "Tiling complete. Total trominoes placed: " << trominoCount << "\n";
    cout << "Greedy 3-coloring: " << (colored ? "SUCCESS" : "FAILED") << "\n";
    printBoard(board, size, trominoes);

    // ---- Brute Force (only for n=2, i.e. 4x4) ----
    if (n == 2) {
        cout << "\n--- Running Brute Force Tiling (4x4 only) ---\n";
        for (int r = 0; r < size; r++)
            for (int c = 0; c < size; c++)
                bfBoard[r][c] = 0;
        bfBoard[mr][mc] = -1;
        bfSize = size;
        trominoCount = 0;
        vector<Tromino> bfTrominoes;

        bool bfSuccess = bfTile(0);
        cout << "Brute force tiling: " << (bfSuccess ? "SUCCESS" : "FAILED") << "\n";

        if (bfSuccess) {
            // Rebuild tromino list from bfBoard
            map<int,Tromino> tMap;
            for (int r = 0; r < size; r++) {
                for (int c = 0; c < size; c++) {
                    int tid = bfBoard[r][c];
                    if (tid > 0) tMap[tid].cells.push_back({r,c});
                }
            }
            for (auto& kv : tMap) {
                Tromino t; t.id = kv.first; t.color = 0; t.cells = kv.second.cells;
                bfTrominoes.push_back(t);
            }

            // Build adjacency for bf board
            map<int,set<int>> bfAdj;
            int dr[] = {-1,1,0,0}, dc[] = {0,0,-1,1};
            for (auto& t : bfTrominoes) {
                for (auto& cell : t.cells) {
                    for (int d = 0; d < 4; d++) {
                        int nr = cell.first+dr[d], nc = cell.second+dc[d];
                        if (nr>=0 && nr<size && nc>=0 && nc<size) {
                            int nid = bfBoard[nr][nc];
                            if (nid > 0 && nid != t.id) {
                                bfAdj[t.id].insert(nid);
                                bfAdj[nid].insert(t.id);
                            }
                        }
                    }
                }
            }

            bool bfColored = bfColor(bfTrominoes, bfAdj, 0);
            cout << "Brute force 3-coloring: " << (bfColored ? "SUCCESS" : "FAILED") << "\n";
            printBoard(bfBoard, size, bfTrominoes);
        }
    } else {
        cout << "\n(Brute force skipped -- only runs for n=2 due to exponential complexity)\n";
    }
    return 0;
}