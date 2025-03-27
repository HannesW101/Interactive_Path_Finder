#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <QWidget>
#include <QTimer>
#include "gridmodel.h"

class GridView : public QWidget
{
    Q_OBJECT
public:
    // (explicit) to stop the compiler from allowing other types to be input into the constructor.
    // (QObject* parent = nullptr) can specify parent object, child will be deleted when parent deleted, good for memory management. Default is nullptr so you will need to manually delete.
    explicit GridView(GridModel* model, QWidget* parent = nullptr);

    // overriding the default sizing to achieve a dynamic size that scales correctly and doesnt lead to unexpected behaviour.
    // Returns a QSize the form of (width, height)
    QSize sizeHint() const override;

public slots:
    // set the current path to this path
    void setPath(const std::vector<std::pair<uint8_t, uint8_t>>& path);

// protected as these are protected virtual methods in QWidget class, if private then wouldnt allow overriding.
// recall a virtual method is made to be overriden by derived classes
protected:
    // paintEvent handles all rendering of a widget on the screen, even updating or resizing.
    // the default painEvent gives a blank widget which we dont want.
    // override it to get custom rendering, control performance / optimize for large grids, and easily connect celltypes to be rendered.
    void paintEvent(QPaintEvent* event) override;

    // does something when the user user clicks their mouse in the grid widget
    void mousePressEvent(QMouseEvent* event) override;

    // does something when the user clicks their mouse, holds the button in and drags their mouse around in the grid widget
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    // helper function to do conversions and process mouse events
    void handleMouseEvent(QMouseEvent* event);

    // helper functions to connect signals to slots
    void makeConnections();

    // pointer to GridModel object that is the data storage for frontend GridView
    GridModel* m_model;

    // describes the size of the cells in pixels.
    // must be int and cannot be uint_8 for e.g. because cell pixels can be large like 100px and then for 100x100 grid this is a lot of pixels, so needs to be int for calculations.
    // should not be unsigned int as lots of the Qt operations dealt with m_cellSize uses ints, so would require constant casting
    int m_cellSize;

    // stores the currently selected cell editing tool chosen from UI (Wall/Boost/etc...)
    GridModel::CellType m_currentTool;

    // stores the current optimal path
    std::vector<std::pair<uint8_t, uint8_t>> m_currentPath;

    // QTimer for the animation of painting the line
    QTimer m_animationTimer;
    // new path for animating the line, will be same as normal path
    std::vector<std::pair<uint8_t, uint8_t>> m_animatingPath;
    // used for an index to iterate through the path and animate elements
    size_t m_currentAnimationStep = 0;

private slots:
    // go to the next index for path animation if there is still steps to animate
    void advanceAnimation();

signals:
    // emits a signal when cell is clicked
    void cellClicked(uint8_t row, uint8_t col);

    // Carries the calculated path from pathfinding algorithm to visualization.
    // Works as follows:
    // (1) Pathfinding completes calculation.
    // (2) Algorithm packages path as vector of coordinates.
    // (3) Signal emitted with result emit pathFound(calculatedPath).
    void pathFound(const std::vector<std::pair<uint8_t, uint8_t>>& path);
};

#endif // GRIDVIEW_H
