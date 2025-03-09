#include "TripPlanner.h"
#include "DatabaseManager.h"
#include <limits>

TripPlanner::TripPlanner() : totalCost(0), n(0) { }

double TripPlanner::tspHelper(int mask, int curr, const std::vector<std::vector<double>> &cost) {
    // Base case: when all colleges have been visited, do not add the return cost.
    if (mask == (1 << n) - 1) {
        return 0;
    }

    if (memo[curr][mask] != -1)
        return memo[curr][mask];

    double ans = std::numeric_limits<double>::max();
    for (int i = 0; i < n; i++) {
        if ((mask & (1 << i)) == 0) {
            double newCost = cost[curr][i] + tspHelper(mask | (1 << i), i, cost);
            if (newCost < ans) {
                ans = newCost;
                nextNode[curr][mask] = i;
            }
        }
    }

    return memo[curr][mask] = ans;
}

std::vector<int> TripPlanner::reconstructPath() {
    int mask = 1, curr = 0;
    std::vector<int> path;
    path.push_back(curr);

    while (mask != (1 << n) - 1) {
        int next = nextNode[curr][mask];
        path.push_back(next);
        curr = next;
        mask |= (1 << next);
    }

    return path;
}

void TripPlanner::calculateTrip(const std::vector<QString>& colleges, DatabaseManager* dbManager) {
    // Store the provided college list.
    collegeList = colleges;
    n = static_cast<int>(colleges.size());
    
    // Build the cost matrix. If a distance between two colleges isn’t found,
    // uses (INF) to represent no direct connection.
    double INF = std::numeric_limits<double>::max() / 2;
    std::vector<std::vector<double>> costMatrix(n, std::vector<double>(n, INF));

    for (int i = 0; i < n; i++) {
        costMatrix[i][i] = 0;
        // Get the distances from collegeList[i] to other colleges.
        auto distances = dbManager->getDistances(colleges[i]);
        // Update the cost matrix only for colleges that are in the provided list.
        for (auto &p : distances) {
            QString dest = p.first;
            double dist = p.second;
            // Find the index of the destination college in the list.
            for (int j = 0; j < n; j++) {
                if (colleges[j] == dest) {
                    costMatrix[i][j] = dist;
                    break;
                }
            }
        }
    }

    // Initialize DP memo and nextNode tables.
    memo.assign(n, std::vector<double>(1 << n, -1));
    nextNode.assign(n, std::vector<int>(1 << n, -1));

    // Run the TSP DP starting at the first college (index 0) with mask 1.
    totalCost = tspHelper(1, 0, costMatrix);
    path = reconstructPath();
}

double TripPlanner::getTotalDistance() {
    return totalCost;
}

std::vector<QString> TripPlanner::getPath() {
    std::vector<QString> collegePath;
    // Map each index in the computed path to its college name.
    for (int idx : path) {
        collegePath.push_back(collegeList[idx]);
    }
    return collegePath;
}