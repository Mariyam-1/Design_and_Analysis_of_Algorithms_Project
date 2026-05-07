package org.example;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

public class RevesPuzzleDC {
    // Using Stack<Integer> perfectly models the LIFO peg behavior
    private static List<Stack<Integer>> table = new ArrayList<>();
    private static final int NUM_PEGS = 4;
    private static final int NUM_DISKS = 8;
    private static int moveCount = 0;
    // private static int[] pegTop = new int[NUM_PEGS];


    public static void main(String[] args) {
        // To run the Divide and Conquer version:
        solve(NUM_DISKS, 0, 3);
    }

    /**
     * The D&C System Controller: Sets up the board and identifies spares automatically.
     */
    public static void solve(int n, int src, int dest) {
        // 1. Setup the physical board
        // Ensure initializeTable takes the source peg as an argument
        initializeTable();

        // 2. Automated Resource Allocation (Finding Spares)
        int s1 = -1, s2 = -1;
        for (int i = 0; i < 4; i++) {
            if (i != src && i != dest) {
                if (s1 == -1) s1 = i;
                else s2 = i;
            }
        }

        // 3. Print Header and Initial State
        printHeader(n, src, dest, s1, s2);

        // 4. Start the Recursive D&C Process
        try {
            RevesTask initialTask = new RevesTask(n, src, dest, s1, s2);
            moveReves(initialTask); // Your D&C implementation
            System.out.println("\n[SUCCESS] Total moves performed: " + moveCount);
        } catch (Exception e) {
            System.err.println("[RUNTIME ERROR] " + e.getMessage());
        }
    }

    private static void printHeader(int n, int src, int dest, int s1, int s2) {
        System.out.println("============== REVE'S SOLVER (D&C) =============");
        System.out.printf("MODE:          Divide & Conquer%n");
        System.out.printf("CONFIGURATION: %d Disks | Source: %d | Target: %d%n", n, src, dest);
        System.out.printf("RESOURCES:     Spare 1: %d | Spare 2: %d%n", s1, s2);
        System.out.println("------------------------------------------------");
        System.out.println("INITIAL STATE:");
        printState();
        System.out.println("================================================\n");
    }
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

    /**
     * Represents a single 4-peg recursive move operation.
     * Encapsulates the sub-problem state.
     */
    public record RevesTask(
            int diskCount,    // How many disks (k) to move
            int src,       // Origin peg index
            int dest,  // Target peg index
            int spare1,       // First auxiliary peg
            int spare2        // Second auxiliary peg
    ) {}

    // R(n) = 2R(t) + H(n-t)
    // how to determine t?
    // for n = 8, i tried myslef and found that by halfing them is optimum
    public static void moveReves(RevesTask t) {
        if (t.diskCount <= 2) {     // R(1) = H(1), R(2) = H(2)
            try {
                moveHanoi(t.diskCount, t.src,  t.dest, t.spare1);     // spare 1 or spare 2
            } catch (RuntimeException e) {
                throw new RuntimeException(e);
            }
            return;
        }
        RevesTask t1 =  new RevesTask(t.diskCount/2, t.src, t.spare1, t.dest, t.spare2);
        moveReves(t1);

        moveHanoi(t.diskCount-t.diskCount/2, t.src,  t.dest, t.spare2);

        RevesTask t2 =  new RevesTask(t.diskCount/2, t.spare1, t.dest, t.spare2, t.src);
        moveReves(t2);

    }

    // H(n) = 2H(n-1) + 1
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
//    public static void moveHanoi(int disks, int pegSrc, int pegDest) {
//        Stack<Integer> source = table.get(pegSrc);
//        if (disks > source.size()) {
//            throw new RuntimeException("Number of disks to move are larger what's in Src Peg " + pegSrc);
//        }
//        List<Integer> availablePegs = new ArrayList<>();
//        // base case
//        if (disks == 1) {
//            try {
//                checkDisk(source.peek(), availablePegs);
//            }
//            catch (Exception e) {
//                throw new RuntimeException(e.getMessage());
//            }
//            if (availablePegs.contains(pegDest)) {
//                try {
//                    moveDisk(source.peek(), pegSrc, pegDest);
//                } catch (RuntimeException e) {
//                    throw new RuntimeException(e);
//                }
//            }
//            else {
//                throw new RuntimeException("Cant move no of disks: " + disks + " to pegDst " + pegDest);
//            }
//            return;
//        }
//        // move n-1 disks
//        try {
//            checkDisk(source.get(source.size()-disks+1), availablePegs);
//        }
//        catch (Exception e) {
//            throw new RuntimeException(e.getMessage());
//        }
//
//        // int pegInter = availablePegs.get(0);
//        int pegInter = -1;
//        for (int p : availablePegs) {
//            if (p != pegDest && p != pegSrc) {
//                pegInter = p;
//                break; // Found our spare!
//            }
//        }
//
//        if (pegInter == -1) {
//            throw new RuntimeException("Could not find an intermediate peg.");
//        }
//        // System.out.println("pegInter: " + pegInter);
//        moveHanoi(disks-1, pegSrc, pegInter);
//
//        // split at n-1
//        int splitDisk = source.peek();
//        // System.out.println("split disk" + splitDisk);
//        try {
//            checkDisk(splitDisk, availablePegs);
//        }
//        catch (Exception e) {
//            throw new RuntimeException(e.getMessage());
//        }
//        moveDisk(splitDisk, pegSrc, pegDest);
//
//        // move n-1 disks
//        moveHanoi(disks-1, pegInter, pegDest);
//
//    }
//    private static void checkDisk(int diskNum, List<Integer> availablePegs) {
//        availablePegs.clear();
//        for(int i = 0; i < NUM_PEGS; i++) {
//            if (table.get(i).isEmpty() || table.get(i).peek() > diskNum) {
//                availablePegs.add(i);
//            }
//        }
//        // System.out.println("Available Pegs: " + availablePegs);
//        if (availablePegs.isEmpty()) {
//            throw new RuntimeException("No valid location to move Disk " + diskNum);
//        }
//    }
}