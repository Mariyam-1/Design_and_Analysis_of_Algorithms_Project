#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

// ─────────────────────────────────────────────
//  Target: moves +-1 each round (random, clamped)
// ─────────────────────────────────────────────
class Target {
    int pos, n;
public:
    Target(int startPos, int n) : pos(startPos), n(n) {}
    int getPos() const { return pos; }
    void move() {// rand a move (cant stay still)
        int dir = (rand() % 2 == 0) ? 1 : -1;
        int next = pos + dir;
        if (next < 1) next = 2;
        if (next > n) next = n - 1;
        pos = next;
    }
};

bool shoot(int shotPos,  Target& target) {
    bool ans=(shotPos == target.getPos());

    cout<< "Shooting at position " << shotPos << " - " << (ans ? "HIT!" : "MISS") << endl;
    if (!ans)
    target.move();
    return ans;
}

// ─────────────────────────────────────────────
//  FORWARD: Binary search left-to-right
//  Returns vector of shot positions
//  Sets hit_found to true when target is hit
// ─────────────────────────────────────────────

void FORWARD(int l, int r,  Target& target, bool& hit_found) {

    if (l > r || hit_found) return ;

    if (l == r) {

        if (shoot(l, target)) {

            hit_found = true;
        }
        return ;
    }

    int m = (l + r) / 2;



    // left half:
    FORWARD(l, m - 1, target, hit_found);

    if (hit_found) return ;

// Shoot at mid

    if (shoot(m, target)) {
        hit_found = true;
        return ;
    }


    // right half:
     FORWARD(m + 1, r, target, hit_found);

    return ;
}

void BACKWARD(int l, int r,  Target& target, bool& hit_found) {
;
    if (l > r || hit_found) return ;

    if (l == r) {

        if (shoot(l, target)) {
            cout << "HIT at position " << l << " (BACKWARD)" << endl;
            hit_found = true;
        }
        return ;
    }

    int m = (l + r) / 2;
    // right half
    BACKWARD(m + 1, r, target, hit_found);
   if (hit_found) return ;

    // Shoot at mid point
    if (shoot(m, target)) {
        cout << "HIT at position " << m << " (BACKWARD)" << endl;
        hit_found = true;
        return ;
    }
    // left half
   BACKWARD(l, m - 1, target, hit_found);

    return ;
}

void HIT_TARGET(int n,  Target& target) {
    cout << "\n=== Hit target Strategy (n=" << n << ") ===" << endl;
    cout << "Target starting position: " << target.getPos() << endl;

    bool hit_found = false;

    if (n == 2) {
        cout << "n==2: Shooting position 1 twice" << endl;
        if (shoot(1, target)) {
            cout << "Target eliminated! Stopping." << endl;
            return;
        }
        if (shoot(1, target)) {
            cout << "Target eliminated! Stopping." << endl;
            return;
        }
        cout << "Target survived all shots!" << endl;
        return;
    }

    // First, shoot FORWARD(2, n-1)
    cout << "First: FORWARD(2, " << n - 1 << ")" << endl;
    cout << "  Shots: \n";
     FORWARD(2, n - 1, target, hit_found);




    if (hit_found) {
        cout << "Target eliminated! Stopping." << endl;
        return;
    }

    //  decide based on odd/even
    if (n % 2 ) {
        // n is odd: use FORWARD again
        cout << "n is odd: Second: FORWARD(2, " << n - 1 << ")" << endl;
        cout << "  Shots: \n";
        FORWARD(2, n - 1, target, hit_found);

        if (hit_found) {
            cout << "Target eliminated! Stopping." << endl;
            return;
        }
    } else {
        // n is even: use BACKWARD
        cout << "n is even: Second: BACKWARD(2, " << n - 1 << ")" << endl;
        cout << "  Shots: \n";
        BACKWARD(2, n - 1, target, hit_found);

        if (hit_found) {
            cout << "Target eliminated! Stopping." << endl;
            return;
        }

    }
    cout << "Target survived all shots!" << endl;

}

// ─────────────────────────────────────────────
//  Main: Test the algorithms
// ─────────────────────────────────────────────
int main() {
    srand(time(0));


    cout << "||===   TARGET SHOOTING GAME - Algorithms  ==||" << endl;

    while(1){
    int n;
    cout << "Enter number of positions (n >= 2) or 0 to exit: ";
    cin>>n;
    if(n == 0) break;
    int startPos = 1 + rand() % n;
    Target target(startPos, n);
    cout << "Target starts at position: " << target.getPos() << endl;
    HIT_TARGET(n, target);
    }
    return 0;
}