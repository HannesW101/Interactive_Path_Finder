#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "gridmodel.h"
#include <vector>
#include <queue>
#include <cmath>

class Pathfinder {
public:
    // this is what will be returned for the optimal path.
    // it consists of a path, and the total cost of the path to be displayed to the user.
    struct PathResult {
        std::vector<std::pair<uint8_t, uint8_t>> path;
        double totalCost;
    };

    // constructor with reference to GridModel that is saved to m_model to access throughout class, used to access cell states / positions of celltypes.
    // const so it wont alter current GridModel
    Pathfinder(const GridModel& model);

    // execute pathfinding algo and return list of co-ordinates representing the path [{a,b}, {b,c}, {c,d}].
    // if path doesnt exist then return empty vector
    PathResult findPath();
private:
    struct Node {
        // grid co-ordinates
        uint8_t row;
        uint8_t col;

        // accumulated cost from the start node to this node.
        // represents the actual cost of the path so far.
        double g;

        // estimated total cost f = g + h.
        // h = heuristic estimate from this node to the goal.
        // prioritizes nodes likely to lead to the optimal path.
        double f;

        // operator overload of > to enable comparison of nodes in the priority queue.
        // returns true if this node’s f is greater than another node’s f.
        bool operator>(const Node& other) const {
            return f > other.f;
        }

        /*
        We only need to overload > operator not < operator because default std::priority_queue uses std::less (max-heap)
        by using std::greater, we invert the order to get a min-heap without rewriting the container
        this matches the natural A* requirement to prioritize nodes with lower f
         */
    };

    // save reference of GridModel from constructor to be used in methods of PathFinder class
    const GridModel& m_model;

    // tracks the lowest known cost to reach each cell from the start.
    // 2D vector matching grid dimensions.
    // all cells start at infinity (unreachable).
    // start cell initialized to 0.0 (zero cost to reach itself).
    std::vector<std::vector<double>> m_costGrid;

    // marks cells that have already been processed.
    // cells start as false (unvisited).
    // set to true once removed from the priority queue.
    std::vector<std::vector<bool>> m_visited;

    // reconstructs the path by tracking where each node was reached from.
    // e.g if m_previous[2][3] = {1, 3}, the path to (2,3) came from (1,3).
    // start cell has {101, 101} (invalid coordinates)
    std::vector<std::vector<std::pair<uint8_t, uint8_t>>> m_previous;

    // prioritizes nodes to explore next during A*.
    // uses a min-heap (due to std::greater<Node>).
    // nodes with the lowest f (estimated total cost) are processed first.
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> m_queue;

    // resets all algorithm state before a new pathfinding run.
    void initialize();

    // returns the movement cost for a given terrain type.
    double getCost(GridModel::CellType type) const;

    // estimates the remaining cost from a cell to the goal. (Manhattan distance)
    double heuristic(uint8_t row, uint8_t col) const;

    // backtraces from the goal to the start using parent pointers.
    // (1) Start at the goal coordinates.
    // (2) Follow m_previous entries backward until reaching the start.
    // (3) Reverse the collected coordinates to get start -> goal order.
    std::vector<std::pair<uint8_t, uint8_t>> reconstructPath(const std::pair<uint8_t, uint8_t>& goal) const;
};

#endif // PATHFINDER_H
