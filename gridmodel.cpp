#include "gridmodel.h"
#include <stdexcept>
#include <algorithm>

// Constructor: Initializes grid dimensions and creates the grid
GridModel::GridModel(std::uint8_t rows, std::uint8_t cols, QObject* parent)
    : QObject(parent), m_rows(rows), m_cols(cols)
{
    // Initialize the grid with 'rows' rows
    m_grid.resize(m_rows);

    // Each row has 'cols' columns, initialized to Normal (std::vector.resize(new size, values to set for the new size))
    for (auto& row : m_grid) {
        row.resize(m_cols, CellType::Normal);
    }
}

// Returns the state of a specific cell
GridModel::CellType GridModel::cellState(std::uint8_t row, std::uint8_t col) const {
    // ensure valid co-ordinates
    validateCoordinates(row, col);

    // return the celltype
    return m_grid[row][col];
}

// Sets the state of a cell and handles special positions
void GridModel::setCellState(std::uint8_t row, std::uint8_t col, CellType type) {
    // ensure valid co-ordinates
    validateCoordinates(row, col);

    if (type == CellType::Start || type == CellType::Goal) {
        // Update special positions
        if (type == CellType::Start) {
            updateSpecialPosition(m_start, row, col, type);
        } else {
            updateSpecialPosition(m_goal, row, col, type);
        }
    } else {
        // For normal cell types
        m_grid[row][col] = type;
        // emit cellUpdated signal
        emit cellUpdated(row, col);
    }
}

// Resets the entire grid to Normal and clears special positions
void GridModel::clearGrid() {
    for (auto& row : m_grid) {
        // fill each row with normal celltype, std::fill is efficient and saves using a nested for loop
        std::fill(row.begin(), row.end(), CellType::Normal);
    }
    // reset start and goal position to default
    m_start = {101, 101};
    m_goal = {101, 101};
    // emit gridReset signal
    emit gridReset();
}

// Returns the current start/goal positions as a (std::pair)
std::pair<std::uint8_t, std::uint8_t> GridModel::startPosition() const { return m_start; }
std::pair<std::uint8_t, std::uint8_t> GridModel::goalPosition() const { return m_goal; }

// Validates if coordinates are within grid bounds
void GridModel::validateCoordinates(std::uint8_t row, std::uint8_t col) const {
    if (row >= m_rows || col >= m_cols) {
        // throw signals an error or exceptional condition. std::out_of_range is a predefined exception type. And then of course the custom error msg. You can catch and do something after error
        throw std::out_of_range("Coordinates out of grid bounds");
    }
}

// Updates special positions (start/goal) and handles cleanup
void GridModel::updateSpecialPosition(std::pair<std::uint8_t, std::uint8_t>& position, std::uint8_t newRow, std::uint8_t newCol, CellType positionType) {
    // Capture old position.
    // structured binding (splits objects like pairs and tuples into its elements so saves positions.first to oldRow and positions.second to oldCol
    const auto [oldRow, oldCol] = position;

    // Clear previous position if valid
    if (oldRow != 101 || oldCol != 101) {
        m_grid[oldRow][oldCol] = CellType::Normal;
        emit cellUpdated(oldRow, oldCol);
    }

    // Update to new position
    position = {newRow, newCol};
    m_grid[newRow][newCol] = positionType;
    emit cellUpdated(newRow, newCol);

    // Emit position change signal
    if (positionType == CellType::Start) {
        emit startPositionChanged(oldRow, oldCol, newRow, newCol);
    } else {
        emit goalPositionChanged(oldRow, oldCol, newRow, newCol);
    }
}
