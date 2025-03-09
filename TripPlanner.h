#ifndef TRIPPLANNER_H
#define TRIPPLANNER_H

#include <vector>
#include <QString>
#include <limits>

// Forward declaration of DatabaseManager
class DatabaseManager;

class TripPlanner {
private:
    // Stores the optimal trip as a list of indices (into collegeList)
    std::vector<int> path;
    // Total distance of the computed trip
    double totalCost;
    // Number of colleges to visit
    int n;
    // The list of colleges provided by the user.
    std::vector<QString> collegeList;
    // DP memo table: memo[current][mask] holds the minimal distance
    std::vector<std::vector<double>> memo;
    // Table to reconstruct the optimal path: nextNode[current][mask]
    std::vector<std::vector<int>> nextNode;
    // Recursive TSP helper using DP with bitmasking.
    double tspHelper(int mask, int curr, const std::vector<std::vector<double>> &cost);
    // Reconstructs the optimal path (as a vector of indices) based on nextNode.
    std::vector<int> reconstructPath();

public:
    TripPlanner();
    // Given a list of colleges and a pointer to the database manager,
    // calculates the optimal trip and total distance to be called with getTotalDistance and getPath.
    void calculateTrip(const std::vector<QString>& colleges, DatabaseManager* dbManager);
    // Returns the total distance (cost) of the most recent trip.
    double getTotalDistance();
    // Returns the optimal trip as a list of college names.
    std::vector<QString> getPath();
};

#endif // TRIPPLANNER_H