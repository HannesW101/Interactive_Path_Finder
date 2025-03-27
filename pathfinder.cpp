#include "pathfinder.h"
#include <algorithm>

Pathfinder::Pathfinder(const GridModel& model) : m_model(model) {}

Pathfinder::PathResult Pathfinder::findPath() {
    // this is the result to be returned from this function {path, cost_of_path}
    PathResult result;

    // initialzie start/goal positions from GridModel object
    const auto start = m_model.startPosition();
    const auto goal = m_model.goalPosition();

    // return empty path if either the goal or start position is not set
    if (start.first == 101 || goal.first == 101) return {};

    // clear previous paths data and reset to calculate new path
    initialize();

    // break up the std::pair for starting position into row and column
    const uint8_t start_row = start.first;
    const uint8_t start_col = start.second;

    // initialize start node to be 0.0 cost (since it doesnt move from start -> start)
    m_costGrid[start_row][start_col] = 0.0;

    // add this starting node to priority queue {row, col, g, f = g + h}
    m_queue.push({start_row, start_col, 0.0, heuristic(start_row, start_col)});

    // Movement directions.
    // delta-row, moving up is -1 and moving down is +1.
    // row changes dont effect right and left so they are 0
    constexpr std::array<int, 4> dr = {-1, 1, 0, 0};

    // delta-column, moving left is -1 and moving right is +1.
    // column changes dont effect up and down so they are 0
    constexpr std::array<int, 4> dc = {0, 0, -1, 1};
    /*
        Great design for implementing directional movement, as one ca easilt have diagonal movement by just expanding the arrow size.
        constexpr is used to initialize values at compile time not runtime, so its more optimized.
    */

    // process nodes in order of their priority (lowest f values first)
    while (!m_queue.empty()) {
        // extract the highest priority Node
        const Node current = m_queue.top();

        // then remove that highest priority node from the queue
        m_queue.pop();

        // if the current node has already been visited, skip this iteration of the while loop (continue)
        // e.g is queue has 2 entries for same node (3, 5) the least one (3) is processed first and the second one (5) should be skipped
        if (m_visited[current.row][current.col]) continue;

        // if the current node has not been processed yet, mark it as processed now (we are going to process it now in this loop)
        m_visited[current.row][current.col] = true;

        // early exit if goal is reached
        if (current.row == goal.first && current.col == goal.second) {
            // construst the PathResult object
            result.path = reconstructPath(goal);
            result.totalCost = m_costGrid[goal.first][goal.second];
            // return this PathResult object
            return result;
        }

        // explore all neighbors.
        // use size_t as the limit for loop is dr.size() so we want our index to be same type.
        // using auto/int will give compiler warning for implicit conversions.
        for (size_t i = 0; i < dr.size(); ++i) {
            // calculate neighbour co-ordinates.
            // cant use uint_8, as we need negative numbers to check boundaries.
            const int nr = current.row + dr[i]; // new row
            const int nc = current.col + dc[i]; // new column

            // boundary check to see if current neighbour is within the grid, if it is not then continue to next neighbour.
            if (nr < 0 || nr >= m_model.rowCount() || nc < 0 || nc >= m_model.colCount()) continue;

            // get the type of cell of the current neighbour.
            const auto cellType = m_model.cellState(nr, nc);

            // get the movement cost of that neighbour cell.
            const double stepCost = getCost(cellType);

            // if the movement cost is < 0 i.e it is a wall (value of -1) then continue to next neighbour.
            if (stepCost < 0) continue;

            // now since neighbour is not a wall we keep going with the calculation.
            // calculate cost from start to current node neighbour.
            const double newCost = current.g + stepCost;

            // if this new cost is better than previous best path, we must update m_costGrid which holds the current best known cost to each node.
            if (newCost < m_costGrid[nr][nc]) {
                // records the previous node that lead to this current node, to be used to reconstruct the path.
                m_previous[nr][nc] = {current.row, current.col};

                // update m_costGrid to hold the new best know cost to the current node.
                m_costGrid[nr][nc] = newCost;

                // estimate cost from this node to goal using heuristic (Manhattan distance).
                const double h = heuristic(nr, nc);

                // add this new node to priority queue {row, col, g, f = g + h}.
                // static_cast converts at compile time for related types like int -> double or in this case int -> uint8_t.
                m_queue.push({static_cast<uint8_t>(nr), static_cast<uint8_t>(nc), newCost, newCost + h});
            }
        }
    }

    // if no path found
    result.totalCost = -1; // Indicate no path with cost -1
    return result;
}

void Pathfinder::initialize() {
    // get number of rows and columns for our GridModel. (100x100)
    const auto rows = m_model.rowCount();
    const auto cols = m_model.colCount();

    // .assign replaces the vector's current contents with new elements (also resizes the vector if needed).
    // Outer Vector (Rows):
    //      m_costGrid is a 2D vector: std::vector<std::vector<double>>.
    //      .assign(rows, ...) resizes m_costGrid to have rows elements.
    // Inner Vector (Columns):
    //      Each row is initialized to std::vector<double>(cols, infinity), which:
    //          Creates a vector of size cols.
    //          Fills it with infinity (a placeholder for "unreachable" cost).
    m_costGrid.assign(rows, std::vector<double>(cols, std::numeric_limits<double>::infinity()));

    // fills m_visited grid with false values (see above for explanation how this is done).
    m_visited.assign(rows, std::vector<bool>(cols, false));

    // fills m_previous grid with {101, 101} values (see above for explanation how this is done).
    m_previous.assign(rows, std::vector<std::pair<uint8_t, uint8_t>>(cols, {101, 101}));
}

double Pathfinder::getCost(GridModel::CellType type) const {
    // simply return the cost associated with each celltype
    switch (type) {
    case GridModel::Wall:    return -1.0;  // Impassable
    case GridModel::Rough:   return 2.0;   // High cost
    case GridModel::Boost:   return 0.5;   // Low cost
    default:                 return 1.0;   // Normal/Start/Goal
    }
}

double Pathfinder::heuristic(uint8_t row, uint8_t col) const {
    // save the position of the goal
    const auto goal = m_model.goalPosition();

    // Manhattan distance multiplied by minimum possible cost per step (0.5 for boost)
    return (std::abs(row - goal.first) + std::abs(col - goal.second)) * 0.5;
}

std::vector<std::pair<uint8_t, uint8_t>> Pathfinder::reconstructPath(const std::pair<uint8_t, uint8_t>& goal) const {
    // create a path variable that will be returned as the reconstructed path.
    std::vector<std::pair<uint8_t, uint8_t>> path;

    // set the current node to be the goal point.
    auto current = goal;

    // m_previous stores the "parent" of each node (where it was reached from).
    // the loop follows these parent pointers until it hits the start node, which has {101, 101} as its parent (indicating no parent).
    while (current.first != 101) {
        path.push_back(current);
        current = m_previous[current.first][current.second];
    }

    // the collected path is in goal -> start order. Reversing it gives start -> goal.
    std::reverse(path.begin(), path.end());

    // return this optimal path
    return path;
}
