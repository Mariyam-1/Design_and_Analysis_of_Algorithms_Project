package org.example;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Stack;

public class RevesPuzzleDP {

    // --- CONSTANTS
    private static final int NUM_PEGS = 5;
    private static final int NUM_DISKS = 10;
    // --- DATA STRUCTURES (THE LUT) ---
    private static long[] minMovesTable =  new long[NUM_DISKS+1];
    private static int[] optimalKTable =  new int[NUM_DISKS+1];

    // --- PHYSICAL STATE (PEGS) ---
    private static List<Stack<Integer>> table = new ArrayList<>();
    private static int moveCount = 0;


    // --- MAIN ENTRY POINT ---

    public static void main(String[] args) {
        solve(NUM_DISKS, 0, 3);
    }


    /**
     * The System Controller: Sets up the brain, the board, and the spares.
     */
    public static void solve(int n, int src, int dest) {
        // 1. Fill the Look-Up Table (The Brain)
        preCompute();

        // 2. Setup the physical stacks on the specified source peg
        initializeTable();

        // 3. Automated Resource Allocation (Finding Spares)
        // We search the set {0, 1, 2, 3} for indices that aren't src or dest.
        int s1 = -1, s2 = -1;
        for (int i = 0; i < 4; i++) {
            if (i != src && i != dest) {
                if (s1 == -1) s1 = i;
                else s2 = i;
            }
        }

        // 4. Print System Diagnostics (Optimized Header)
        printDiagnostics(n, src, dest, s1, s2);

        // 5. Execute the optimized sequence
        try {
            RevesTask initialTask = new RevesTask(n, src, dest, s1, s2);
            moveReves(initialTask);
            System.out.println("\n[SUCCESS] Total moves performed: " + moveCount);
        } catch (Exception e) {
            System.err.println("[RUNTIME ERROR] " + e.getMessage());
        }
    }

    private static void printDiagnostics(int n, int src, int dest, int s1, int s2) {
        System.out.println("================ REVE'S SOLVER (DP) ================");
        System.out.printf("CONFIGURATION: %d Disks | Source: %d | Target: %d%n", n, src, dest);
        System.out.printf("RESOURCES:     Spare 1: %d | Spare 2: %d%n", s1, s2);
        System.out.println("----------------------------------------------------");

        // Print the LUT (Look-Up Table)
        System.out.println("DP TABLE (Principle of Optimality):");
        System.out.println("Disks | Min Moves | Optimal K (Split)");
        for (int i = 1; i <= n; i++) {
            System.out.printf("%-5d | %-9d | %-9d%n", i, minMovesTable[i], optimalKTable[i]);
        }

        System.out.println("----------------------------------------------------");
        System.out.println("INITIAL MEMORY STATE:");
        printState();
        System.out.println("====================================================\n");
    }
    // --- INITIALIZATION ---

    /**
     * Pre-calculates the optimal moves and split points (k) for every n
     * up to the maximum number of disks.
     */
    public static void preCompute() {
        // Set base cases
        // n =1
        minMovesTable[1] = 1;
        optimalKTable[1] = 0;
        // n=2
        minMovesTable[2] = 3;
        optimalKTable[2] = 1;

        Arrays.fill(minMovesTable, 3, minMovesTable.length, Long.MAX_VALUE);
        // Implement the Principle of Optimality loops here
        for (int i = 3; i <= NUM_DISKS; i++) {
            for (int j = 1; j < i; j++) {
                long min = computeMoves(i, j);
                if (min < minMovesTable[i]) {
                    minMovesTable[i] = min;
                    optimalKTable[i] = j;
                }
            }
        }
    }

    private static long computeMoves(int n, int k) {
        return 2*minMovesTable[k] + (long)Math.pow(2, n-k) - 1;
    }
    /**
     * Sets up the stacks and disks on the starting peg.
     */
    public static void initializeTable() {
        table.clear();
        for (int i = 0; i < NUM_PEGS; i++) {
            table.add(new Stack<>());
        }

        // Push disks onto Peg 0: [8, 7, 6, 5, 4, 3, 2, 1]
        // Bottom is index 0, Top is the last element
        for (int i = NUM_DISKS; i >= 1; i--) {
            table.get(0).push(i);
        }
        moveCount = 0;
//        for (int i = 0; i < NUM_PEGS; i++) {
//            pegTop[i] = table.get(i).peek();
//        }
    }

    // --- RECURSIVE SOLVER ---

    /**
     * The DP-based solver. Instead of calculating k, it reads from optimalKTable.
     */
    public record RevesTask(
            int diskCount,    // How many disks (k) to move
            int src,       // Origin peg index
            int dest,  // Target peg index
            int spare1,       // First auxiliary peg
            int spare2        // Second auxiliary peg
    ) {}

    // R(n) = 2R(k) + H(n-k)
    // now we searched for all optimal values of k and found them, acorrding to the initial decisions
    public static void moveReves(RevesTask t) {
        // base case
        if (t.diskCount <= 2) {     // R(1) = H(1), R(2) = H(2)
            try {
                moveHanoi(t.diskCount, t.src,  t.dest, t.spare1);     // spare 1 or spare 2
            } catch (RuntimeException e) {
                throw new RuntimeException(e);
            }
            return;
        }
        int k = optimalKTable[t.diskCount];
        RevesTask t1 =  new RevesTask(k, t.src, t.spare1, t.dest, t.spare2);
        moveReves(t1);

        moveHanoi(t.diskCount-k, t.src,  t.dest, t.spare2);

        RevesTask t2 =  new RevesTask(k, t.spare1, t.dest, t.spare2, t.src);
        moveReves(t2);

    }


    public static void moveHanoi(int disks, int src, int dest, int spare) {
        // Base Case: Move the single disk directly
        if (disks == 1) {
            int diskNum = table.get(src).peek();
            moveDisk(diskNum, src, dest);
            return;
        }

        // Step 1: Move n-1 disks from Source to Spare (using Dest as temp)
        moveHanoi(disks - 1, src, spare, dest);

        // Step 2: Move the largest disk from Source to Destination
        int splitDisk = table.get(src).peek();
        moveDisk(splitDisk, src, dest);

        // Step 3: Move n-1 disks from Spare to Destination (using Src as temp)
        moveHanoi(disks - 1, spare, dest, src);
    }


    // --- EXECUTION ---

    /**
     * Performs the actual movement and validation of disks.
     */
    public static void moveDisk(int diskNum, int pegSrc, int pegDest) {
        Stack<Integer> source = table.get(pegSrc);
        Stack<Integer> dest = table.get(pegDest);

        // 1. Validation
        if (source.isEmpty()) {
            throw new RuntimeException("Source peg " + pegSrc + " is empty!");
        }

        // 2. Validation: Top check
        if (source.peek() != diskNum) {
            throw new RuntimeException("Disk " + diskNum + " is not at the top of Peg " + pegSrc);
        }

        // 3. Validation: Rule of Hanoi
        if (!dest.isEmpty() && dest.peek() < diskNum) {
            throw new RuntimeException("Illegal Move: Cannot place " + diskNum + " on " + dest.peek());
        }

        // 4. Execution
        dest.push(source.pop());
        moveCount++;
        // pegTop[pegDest] = dest.peek();

        System.out.println("Step " + moveCount + ": Move Disk " + diskNum + " from Peg " + pegSrc + " to " + pegDest);
        System.out.println("State:");
        printState();
    }


    /**
     * Formats the current peg states for console output.
     */
    public static void printState() {
        // Starts the line with (Disks, Pegs,
        System.out.print("(" + NUM_DISKS + ", " + NUM_PEGS + ", [");

        for (int i = 0; i < NUM_PEGS; i++) {
            // Formats the stack to look like [1, 2, 3] or []
            String stackStr = table.get(i).toString();
            System.out.print(stackStr);

            // Adds a comma between pegs, but not after the last one
            if (i < NUM_PEGS - 1) {
                System.out.print(", ");
            }
        }

        // Closes the list and adds the move counter
        System.out.println("], " + moveCount + ")");
    }

}
