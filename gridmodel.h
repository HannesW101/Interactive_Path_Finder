#ifndef GRIDMODEL_H
#define GRIDMODEL_H

// Prevents header file from being #included multiple times
#pragma once

// header files
#include <QObject> // all Qt objects inherit from this. (enables signals/slots for our class if inherited)
#include <vector>

class GridModel : public QObject // inherits from QObject
{
    // macro to enable signals and slots (+ meta-object features)
    Q_OBJECT

public:
    // enum must be public so we can use it in other classes
    // Enums for wall types (makes it easier as CellType::Wall makes sense and cell_type = 2 isnt intuitive)
    enum CellType { Normal, Wall, Rough, Boost, Start, Goal };

    //  macro to enable meta-object features for reflection (Convert enums to/from strings, Iterate over enum values, Access metadata about enums)
    Q_ENUM(CellType)

    // Constructor/Destructor
    // (explicit) to stop the compiler from converting maybe ints to type gridmodel like "gridmodel(50)" should be rejected but maybe compiler creates "gridmodel(50,50)"
    // (QObject* parent = nullptr) can specify parent object, child will be deleted when parent deleted, good for memory management. Default is nullptr so you will need to manually delete
    // (std::uint8_t) range of 0-255. int is 2 bytes uint8_t is 1 byte.
    explicit GridModel(std::uint8_t rows, std::uint8_t cols, QObject* parent = nullptr);

    // (virtual) intended for classes that will be inherited to avoid memory leaks, will call this destructor then the desctructor for the class that inherits
    // (= default) lets compiler cleanup the memory instead of manually doing it yourself
    virtual ~GridModel() = default;

    // Getters with const correctness
    // (const) promises users function wont change value. Allows also const values to be passed as parameter
    // (noexcept) promises no exceptions (errors) will be thrown during this function, so compiler can optimize the performence of the code *destructors are default noexcept!*
    std::uint8_t rowCount() const noexcept { return m_rows; }
    std::uint8_t colCount() const noexcept { return m_cols; }

    // returns the CellType for a specific cell, const so doesnt change gridmodel object and can accept const types
    CellType cellState(std::uint8_t row, std::uint8_t col) const;

    // simply changes the state of a specific cell
    void setCellState(std::uint8_t row, std::uint8_t col, CellType type);

    // resets whole grid to default states and clears start and goal positions {101, 101} indicating no position
    void clearGrid();

    // simply returns the start/goal position as a pair on integers
    std::pair<std::uint8_t, std::uint8_t> startPosition() const;
    std::pair<std::uint8_t, std::uint8_t> goalPosition() const;

private:
    // Core data members
    const std::uint8_t m_rows;
    const std::uint8_t m_cols;
    std::vector<std::vector<CellType>> m_grid; // grid containing the celltype of each cell

    // positions for start/goal (default is {101, 101} meaning no position) - these are specialpositions
    std::pair<std::uint8_t, std::uint8_t> m_start {101, 101};
    std::pair<std::uint8_t, std::uint8_t> m_goal {101, 101};

    // Validation utilities
    // check if the given cell position is within the grid
    void validateCoordinates(std::uint8_t row, std::uint8_t col) const;

    // updates the position for either the goal/start state
    void updateSpecialPosition(std::pair<std::uint8_t, std::uint8_t>& position, std::uint8_t newRow, std::uint8_t newCol, CellType positionType);

signals:
    // signal emmited when grid is reset
    void gridReset();

    // signal emmited when a specific cell is updated
    void cellUpdated(std::uint8_t row, std::uint8_t col);

    // signal emmited when start position is changed
    void startPositionChanged(std::uint8_t oldRow, std::uint8_t oldCol, std::uint8_t newRow, std::uint8_t newCol);

    // signal emmited when goal position is changed
    void goalPositionChanged(std::uint8_t oldRow, std::uint8_t oldCol, std::uint8_t newRow, std::uint8_t newCol);
};

#endif // GRIDMODEL_H
