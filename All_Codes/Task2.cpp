#include <iostream>
#include <vector>

using namespace std;

int n;
int x,y;
vector<vector<int>> board;

void initializeBoard() {
    board.resize(n, vector<int>(n, 0));
}

void updateCell(int row, int col, int value) {
    board[row][col] = value;
}

void chooseStep(int cur_x, int cur_y);
int countOutMoves(int pot_x, int pot_y);

int main (){
    cout << "enter board dimensions n*n";
    cin >> n;
    int step = 0;

    if (n < 6 || n%2==1){
        cout << "Not applicable to perform closed Knight's tour";
        return 0;
    }

    initializeBoard();

    cout << "Enter starting row value";
    cin >> x;

    while(x >= n || x < 0){
        cout << "row value has to be greater than or equal 0 and less than " << n;
        cin >> x;
    }

    cout << "Enter starting column value";
    cin >> y;

    while(y >= n || y < 0){
        cout << "column value has to be greater than or equal 0 and less than " << n;
        cin >> y;
    }

    step++;
    updateCell(x, y, step);

    while (step < n*n){

        chooseStep(x,y);

        if(x == -1 || y == -1){
            cout << "Knight's tour stopped at " << step << " step";
            return 0;
        }

        step++;
        updateCell(x, y, step);
    }

    cout << "Knight's tour completed, number of steps is " << step;
    return 0;
}

double distanceFromCenter(int px, int py) {
    double cx = (n - 1) / 2.0;
    double cy = (n - 1) / 2.0;
    return (px - cx) * (px - cx) + (py - cy) * (py - cy);
}

void chooseStep(int cur_x, int cur_y){

    int out_count [8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    int min_index = -1;
    int min_value = 100;
    double min_dist = 1e9;

    if (cur_x < 0 || cur_y < 0 || cur_x >= n || cur_y >= n){
        x = -1;
        y = -1;
        return;
    }

    if(cur_x + 2 < n){
        if(cur_y + 1 < n && board[cur_x + 2][cur_y + 1] == 0){
            out_count[0] = countOutMoves(cur_x + 2, cur_y + 1);
        }
        if(cur_y - 1 >= 0 && board[cur_x + 2][cur_y - 1] == 0){
            out_count[1] = countOutMoves(cur_x + 2, cur_y - 1);
        }
    }

    if(cur_x - 2 >= 0){
        if(cur_y + 1 < n && board[cur_x - 2][cur_y + 1] == 0){
            out_count[2] = countOutMoves(cur_x - 2, cur_y + 1);
        }
        if(cur_y - 1 >= 0 && board[cur_x - 2][cur_y - 1] == 0){
            out_count[3] = countOutMoves(cur_x - 2, cur_y - 1);
        }
    }

    if(cur_x + 1 < n){
        if(cur_y + 2 < n && board[cur_x + 1][cur_y + 2] == 0){
            out_count[4] = countOutMoves(cur_x + 1, cur_y + 2);
        }
        if(cur_y - 2 >= 0 && board[cur_x + 1][cur_y - 2] == 0){
            out_count[5] = countOutMoves(cur_x + 1, cur_y - 2);
        }
    }

    if(cur_x - 1 >= 0){
        if(cur_y + 2 < n && board[cur_x - 1][cur_y + 2] == 0){
            out_count[6] = countOutMoves(cur_x - 1, cur_y + 2);
        }
        if(cur_y - 2 >= 0 && board[cur_x - 1][cur_y - 2] == 0){
            out_count[7] = countOutMoves(cur_x - 1, cur_y - 2);
        }
    }

    int dx[] = {2, 2, -2, -2, 1, 1, -1, -1};
    int dy[] = {1, -1, 1, -1, 2, -2, 2, -2};

    for(int i = 0; i < 8; i++){
        if(out_count[i] == -1) continue;

        int nx = cur_x + dx[i];
        int ny = cur_y + dy[i];
        double dist = distanceFromCenter(nx, ny);

        if(out_count[i] < min_value ||
          (out_count[i] == min_value && dist > min_dist)){
            min_value = out_count[i];
            min_index = i;
            min_dist = dist;
        }
    }

    switch(min_index) {

        case 0:
            x = cur_x + 2;
            y = cur_y + 1;
            break;

        case 1:
            x = cur_x + 2;
            y = cur_y - 1;
            break;

        case 2:
            x = cur_x - 2;
            y = cur_y + 1;
            break;

        case 3:
            x = cur_x - 2;
            y = cur_y - 1;
            break;

        case 4:
            x = cur_x + 1;
            y = cur_y + 2;
            break;

        case 5:
            x = cur_x + 1;
            y = cur_y - 2;
            break;

        case 6:
            x = cur_x - 1;
            y = cur_y + 2;
            break;

        case 7:
            x = cur_x - 1;
            y = cur_y - 2;
            break;

        default:
            x = -1;
            y = -1;
    }
}

int countOutMoves(int pot_x, int pot_y){

    int out_moves = 0;

    if(pot_x + 2 < n){

        if(pot_y + 1 < n && board[pot_x + 2][pot_y + 1] == 0){
            out_moves++;
        }

        if(pot_y - 1 >= 0 && board[pot_x + 2][pot_y - 1] == 0){
            out_moves++;
        }
    }

    if(pot_x - 2 >= 0){

        if(pot_y + 1 < n && board[pot_x - 2][pot_y + 1] == 0){
            out_moves++;
        }

        if(pot_y - 1 >= 0 && board[pot_x - 2][pot_y - 1] == 0){
            out_moves++;
        }
    }

    if(pot_x + 1 < n){

        if(pot_y + 2 < n && board[pot_x + 1][pot_y + 2] == 0){
            out_moves++;
        }

        if(pot_y - 2 >= 0 && board[pot_x + 1][pot_y - 2] == 0){
            out_moves++;
        }
    }

    if(pot_x - 1 >= 0){

        if(pot_y + 2 < n && board[pot_x - 1][pot_y + 2] == 0){
            out_moves++;
        }

        if(pot_y - 2 >= 0 && board[pot_x - 1][pot_y - 2] == 0){
            out_moves++;
        }
    }
    return out_moves;
}