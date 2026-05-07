#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

// Structure to represent a Job
struct Job {
    int id;
    int t; // Processing time
    int d; // Deadline
    int p; // Penalty
};

// Comparator to sort jobs by deadline ascending
bool compareDeadline(const Job& a, const Job& b) {
    return a.d < b.d;
}

// Recursive Divide and Conquer function
int solveDivideAndConquer(const vector<Job>& jobs, int index, int current_time) {
    // Base case: No more jobs to evaluate
    if (index == jobs.size()) {
        return 0;
    }

    // Branch 1: Exclude the current job from the "on-time" schedule
    // We don't advance the time, but we increase the penalty
    int penaltySkip = jobs[index].p + solveDivideAndConquer(jobs, index + 1, current_time);

    // Branch 2: Include the current job in the "on-time" schedule
    // Only feasible if it completes within its deadline
    int penaltyInclude = INT_MAX;
    if (current_time + jobs[index].t <= jobs[index].d) {
        penaltyInclude = solveDivideAndConquer(jobs, index + 1, current_time + jobs[index].t);
    }

    // Return the minimum penalty from both branches
    return min(penaltySkip, penaltyInclude);
}

// Wrapper function to handle sorting and initial call
int getMinPenalty(vector<Job>& jobs) {
    // Sort jobs by deadline first for optimal subset testing
    sort(jobs.begin(), jobs.end(), compareDeadline);
    return solveDivideAndConquer(jobs, 0, 0);
}
