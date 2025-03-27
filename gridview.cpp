#include "gridview.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>

GridView::GridView(GridModel* model, QWidget* parent)
    : QWidget(parent), // Initialize base QWidget
    m_model(model), // Store reference to data model
    m_cellSize(10), // Set cell size to 30 pixels
    m_currentTool(GridModel::Normal) // Default to "Normal" tool
{
    // Set widget dimensions based on grid size
    setFixedSize(m_model->colCount() * m_cellSize, m_model->rowCount() * m_cellSize);

    // Enable mouse tracking for smooth dragging
    setMouseTracking(true);

    // call makeConnections helper function
    GridView::makeConnections();
}

// Provides a recommended size for the widget by using columns/rows and the set cellsize
QSize GridView::sizeHint() const
{
    return QSize(m_model->colCount() * m_cellSize, m_model->rowCount() * m_cellSize);
}

// grid will max be 100x100 so doesnt need to be extremely optimized
// Draws a grid of cells on the screen, Colors each cell based on its type, and Adds grid lines to visually separate cells
void GridView::paintEvent(QPaintEvent* event) {
    // Creates a QPainter to draw on this widget
    QPainter painter(this);

    // Antialiasing smoothes out edges creating a more natural look when rendering, performence is not big since grid is only 100x100
    painter.setRenderHint(QPainter::Antialiasing);

    // declare variables that will be used later in this function to avoid repeatedly calling methods for m_model.
    // much faster as this loads the data into L1 cache as the compiler knows it wont change as its a local const variable
    const int cs = m_cellSize;
    const int cols = m_model->colCount();
    const int rows = m_model->rowCount();

    // map for the colors of the corresponding celltype
    const std::unordered_map<GridModel::CellType, QColor> color_map = {
        {GridModel::Normal,  Qt::white},
        {GridModel::Wall,    Qt::darkGray},
        {GridModel::Rough,   QColor(139, 69, 19)},
        {GridModel::Boost,   Qt::yellow},
        {GridModel::Start,   Qt::green},
        {GridModel::Goal,    Qt::red}
    };

    // paint the grid cells
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            // create a QRect (x, y, width, height)
            QRect cell_rect(col * cs, row * cs, cs, cs);

            // complicated type of variable so use auto (for both, they will have long variable declarations, this is easier to read and dynamic)
            // get celltype and set it to type.
            const auto type = m_model->cellState(row, col);

            // get iterator to celltype's corresponding color
            const auto it = color_map.find(type);

            // fill the rectangle cell_rect with the correct color using ternary operator.
            // (1) if celltype was found then paints it with the mapped color using it->second.
            // (2) if celltype was not found then paints it default Qt::white
            painter.fillRect(cell_rect, (it != color_map.end()) ? it->second : Qt::white);

            // set the pen color for drawing the cell border
            painter.setPen(Qt::gray);

            // draw outline of cell rectangle using current color and at coordinates cell_rect
            painter.drawRect(cell_rect);
        }
    }

    // Draw the path
    if(!m_animatingPath.empty() && m_currentAnimationStep > 0) {
        // create a painterpath object
        QPainterPath painterPath;

        // set the pen for the painter object (not the painterpath!)
        painter.setPen(QPen(Qt::darkBlue, 4));

        // save cell size
        const int cs = m_cellSize;

        // get first element of animating path
        auto& first = m_animatingPath.front();

        // get the grid point for the starting co-ordinate (x = columns, y = rows).
        // *cs puts us at the cell.
        // +cs/2 ensures we are in the middle of that cell.
        QPoint startPoint(first.second * cs + cs/2, first.first * cs + cs/2);

        // move the painterPath to this starting point
        painterPath.moveTo(startPoint);

        // loop through the path to animate each step
        for(size_t i = 1; i < m_currentAnimationStep && i < m_animatingPath.size(); ++i) {
            // get the co-ordinate of the next node in the path (starting at 1 since we already have start node)
            auto& point = m_animatingPath[i];

            // get the grid point for this node
            QPoint endPoint(point.second * cs + cs/2, point.first * cs + cs/2);

            // make painterPath draw a line from the previous node to this new node
            painterPath.lineTo(endPoint);
        }

        // render our line we constructed
        painter.drawPath(painterPath);
    }
}

void GridView::mousePressEvent(QMouseEvent* event)
{
    handleMouseEvent(event);
}

void GridView::mouseMoveEvent(QMouseEvent* event)
{
    // only allows holding in the left click mouse button
    if (event->buttons() & Qt::LeftButton) {
        handleMouseEvent(event);
    }
}

void GridView::handleMouseEvent(QMouseEvent* event)
{
    // find column and row using m_cellSize and also the position of where the mouse event happened
    int col = event->position().x() / m_cellSize;
    int row = event->position().y() / m_cellSize;

    // only emit signal if a cell is actually clicked
    if (row >= 0 && row < m_model->rowCount() && col >= 0 && col < m_model->colCount()) {
        emit cellClicked(row, col);
    }
}

void GridView::makeConnections()
{
    // *** Lamda function starts with []() and the parameters go into () .
    // [this] captures the enclosing class (GridView) and gives access to its member variables and functions.
    // lamda functions cannot access private things like that and need to be given a certain of access passed into []. ***

    // listens for the cellUpdated signal from GridModel and then calls the update() method to redraw only that specific cell, to avoid redrawing the whole widget.
    // sender obj: m_model.
    // sender signal: &GridModel::cellUpdated.
    // receiver object: this (aka GridView object).
    // receiver slot: lamda function [this](uint8_t row, uint8_t col)
    connect(m_model, &GridModel::cellUpdated, this, [this](uint8_t row, uint8_t col) {
        update(QRect(col * m_cellSize, row * m_cellSize, m_cellSize, m_cellSize));
    });

    // listens for the gridReset signal from GridModel and then calls the update() method to redraw the whole widget (make it empty).
    // sender obj: m_model.
    // sender signal: &GridModel::gridReset.
    // receiver object: this (aka GridView object).
    // receiver slot: lamda function [this]
    connect(m_model, &GridModel::gridReset, this, [this]() {
        m_currentPath.clear();
        m_animatingPath.clear();
        update();
    });

    // m_animationTimer emits signal.
    // QTimer::timeout is signal emmitted.
    // refers to the current GridView instance.
    // advance animation is the slot thats called
    connect(&m_animationTimer, &QTimer::timeout, this, &GridView::advanceAnimation);
}

void GridView::setPath(const std::vector<std::pair<uint8_t, uint8_t> > &path)
{
    //  updates paths and initialize m_currentAnimationStep to 0
    m_currentPath = path;
    m_animatingPath = path;
    m_currentAnimationStep = 0;
    // Update every 50ms
    m_animationTimer.start(50);
    // repaint the screen
    update();
}

void GridView::advanceAnimation() {
    // keep going to next m_currentAnimationStep until we are at the end of m_animatingPath.size() and update widget
    if(m_currentAnimationStep < m_animatingPath.size()) {
        m_currentAnimationStep++;
        update();
    } else { // otherwise stop the animation
        m_animationTimer.stop();
    }
}
