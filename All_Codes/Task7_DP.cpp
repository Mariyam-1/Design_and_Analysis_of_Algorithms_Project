#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// Structure to represent each job
struct Job {
    int t; // processing time (how long the job takes)
    int d; // deadline (must finish before this time)
    int p; // penalty (cost if we miss the deadline)
};

// Function to sort jobs by deadline (ascending)
bool compare(Job a, Job b) {
    return a.d < b.d; // job with smaller deadline comes first
}

int main() {
    int n;
    cin >> n; // number of jobs

    vector<Job> jobs(n); // store all jobs

    int total_penalty = 0; // sum of all penalties

    // Step 1: Read input
    for (int i = 0; i < n; i++) {
        cin >> jobs[i].t >> jobs[i].d >> jobs[i].p;

        // Add penalty of this job to total
        // This represents worst case (if we fail all jobs)
        total_penalty += jobs[i].p;
    }

    // Step 2: Sort jobs by deadline
    // This helps us process jobs in a logical order
    sort(jobs.begin(), jobs.end(), compare);

    // Step 3: Find maximum deadline
    // We will use it as the size of DP array
    int max_deadline = 0;
    for (int i = 0; i < n; i++) {
        max_deadline = max(max_deadline, jobs[i].d);
    }

    // Step 4: Create DP array
    // dp[t] = maximum penalty we can SAVE using time = t
    vector<int> dp(max_deadline + 1, 0);

    // Step 5: Process each job one by one
    for (int i = 0; i < n; i++) {

        int t = jobs[i].t; // time required for this job
        int d = jobs[i].d; // deadline
        int p = jobs[i].p; // penalty

        // loop backwards
        // to make sure each job is used only once
        for (int time = d; time >= t; time--) {

            // Option 1: do NOT take the job → dp[time]
            // Option 2: take the job → dp[time - t] + p

            dp[time] = max(dp[time], dp[time - t] + p);

            /*
            Explanation of this line:

            dp[time]        = current best result without using this job
            dp[time - t]    = best result before adding this job
            + p             = we gain penalty (we avoid paying it)

            So we choose:
            - either keep old result
            - or take job and improve result

            We always take the better option.
            */
        }
    }

    // Step 6: Find maximum saved penalty
    int max_saved = 0;

    for (int i = 0; i <= max_deadline; i++) {
        max_saved = max(max_saved, dp[i]);
    }

    // Step 7: Compute final answer
    // total_penalty = worst case (lose everything)
    // max_saved = what we managed to save
    // remaining = actual penalty we must pay
    int answer = total_penalty - max_saved;

    cout << "Minimum Penalty = " << answer;

    return 0;
}
