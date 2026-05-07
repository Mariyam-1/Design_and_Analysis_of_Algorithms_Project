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

void solveBruteForce(vector<Job>& jobs) {
    int n = jobs.size();
    int min_penalty = INT_MAX;
    vector<int> optimal_schedule;

    // Create an array of job indices to permute
    vector<int> indices(n);
    for (int i = 0; i < n; i++) {
        indices[i] = i;
    }

    // Evaluate all permutations
    do {
        int current_time = 0;
        int current_penalty = 0;

        for (int i = 0; i < n; i++) {
            int idx = indices[i];
            current_time += jobs[idx].t;

            // If the job finishes after its deadline, add penalty
            if (current_time > jobs[idx].d) {
                current_penalty += jobs[idx].p;
            }
        }

        // Update minimum penalty and optimal schedule if a better one is found
        if (current_penalty < min_penalty) {
            min_penalty = current_penalty;
            optimal_schedule = indices;
        }

    } while (next_permutation(indices.begin(), indices.end()));// Takes a sorted array in ascending order
    //provides all possible permutations and returns true as long as it finds a new larger permutation
    //once it reaches the descending order, calling it again will reset the array to ascending order and return false.

    // Output results
    cout << "Minimum Total Penalty: " << min_penalty << endl;
    cout << "Optimal Job Schedule (by ID): ";
    for (int i = 0; i < n; i++) {
        cout << jobs[optimal_schedule[i]].id << " ";
    }
    cout << endl;
}

