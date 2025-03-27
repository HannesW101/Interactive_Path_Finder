#include "mainwindow.h"
#include "pathfinder.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QRadioButton>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // create new GridModel object and set the grid size to be 100x100 (also set pointer to its parent, MainWindow).
    m_model = new GridModel(100, 100, this);

    // create new GridView object and input the GridModel object into it (also set pointer to its parent, MainWindow).
    m_view = new GridView(m_model, this);

    // create total cost display
    m_costLabel = new QLabel("Path cost: --", this);
    m_costLabel->setAlignment(Qt::AlignCenter);
    m_costLabel->setStyleSheet("QLabel { font: bold 14px; color: black; }");

    // create centralWidget to fill MainWindow
    QWidget *centralWidget = new QWidget(this);

    // Create vertical container as the main layout
    QVBoxLayout* container = new QVBoxLayout(centralWidget); // This becomes centralWidget's layout

    // Create horizontal layout for grid + tools
    QHBoxLayout *mainLayout = new QHBoxLayout();

    // add components to layouts
    container->addWidget(m_costLabel);    // Label at top
    container->addLayout(mainLayout);     // Grid + tools below label
    mainLayout->addWidget(m_view);        // Grid on left
    mainLayout->addWidget(createToolButtons()); // Tools on right

    // when signal cellClicked is received, it changes the state of the cell clicked to the celltype that is currently chosen from the UI
    connect(m_view, &GridView::cellClicked, this, [this](uint8_t row, uint8_t col) {
            m_model->setCellState(row, col, m_currentTool);
        }
    );

    // set central widget of MainWindow to the centralWidget created above
    setCentralWidget(centralWidget);

    // set window title
    setWindowTitle("Pathfinding Visualizer");

    // set size of window in pixels (width, height)
    resize(800, 600);
}

QWidget* MainWindow::createToolButtons()
{
    // create a panel and add it to a layout
    QWidget *toolPanel = new QWidget;
    QVBoxLayout *toolLayout = new QVBoxLayout(toolPanel);

    // Create radio button group
    m_toolGroup = new QButtonGroup(this);

    // Create radio buttons
    QRadioButton *normalBtn = new QRadioButton("Normal", toolPanel);
    QRadioButton *wallBtn = new QRadioButton("Wall", toolPanel);
    QRadioButton *roughBtn = new QRadioButton("Rough", toolPanel);
    QRadioButton *boostBtn = new QRadioButton("Boost", toolPanel);
    QRadioButton *startBtn = new QRadioButton("Start", toolPanel);
    QRadioButton *goalBtn = new QRadioButton("Goal", toolPanel);

    // Add to button group
    m_toolGroup->addButton(normalBtn, GridModel::Normal);
    m_toolGroup->addButton(wallBtn, GridModel::Wall);
    m_toolGroup->addButton(roughBtn, GridModel::Rough);
    m_toolGroup->addButton(boostBtn, GridModel::Boost);
    m_toolGroup->addButton(startBtn, GridModel::Start);
    m_toolGroup->addButton(goalBtn, GridModel::Goal);

    // Set default selection
    normalBtn->setChecked(true);

    // connections for each radio button, checks if it is toggled and if it is then updates the current tool to the correct value
    connect(normalBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if(checked) m_currentTool = GridModel::Normal;
    });
    connect(wallBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if(checked) m_currentTool = GridModel::Wall;
    });
    connect(roughBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if(checked) m_currentTool = GridModel::Rough;
    });
    connect(boostBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if(checked) m_currentTool = GridModel::Boost;
    });
    connect(startBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if(checked) m_currentTool = GridModel::Start;
    });
    connect(goalBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if(checked) m_currentTool = GridModel::Goal;
    });

    // Action buttons used to either clear the grid or find the optimal path
    QPushButton *clearBtn = new QPushButton("Clear Grid", toolPanel);
        // connect the clearBtn to the method that clears the grid and resets the total cost label
        connect(clearBtn, &QPushButton::clicked, this, [this]() {
            m_model->clearGrid();
            m_costLabel->setText("Path cost: --");
        });
    QPushButton *pathBtn = new QPushButton("Find Path", toolPanel);
        // connect pathBtn to a lamda that finds the optimal path from start -> goal
        connect(pathBtn, &QPushButton::clicked, this, [this]() {
            // save the values for the start and goal position into local variables
            auto start = m_model->startPosition();
            auto goal = m_model->goalPosition();

            // check if start and goal are set
            if (start.first == 101 || goal.first == 101) {
                // if either doesnt exist then display an error message
                QMessageBox::warning(this, "Error", "Set start and goal positions first!");
                return;
            }

            // create Pathfinder object to find optimal path
            Pathfinder pathfinder(*m_model);

            // return the optimal path to variable path
            auto result = pathfinder.findPath();

            // update GridView with the new path
            m_view->setPath(result.path);

            // update the total cost label for the path
            if (result.totalCost >= 0) {
                m_costLabel->setText(QString("Optimal path cost: %1").arg(result.totalCost, 0, 'f', 2));
            } else {
                m_costLabel->setText("No valid path found!");
            }
        });

    // adding all the elements to the layout
    toolLayout->addWidget(normalBtn);
    toolLayout->addWidget(wallBtn);
    toolLayout->addWidget(roughBtn);
    toolLayout->addWidget(boostBtn);
    toolLayout->addWidget(startBtn);
    toolLayout->addWidget(goalBtn);
    toolLayout->addSpacing(20);
    toolLayout->addWidget(clearBtn);
    toolLayout->addWidget(pathBtn);
    toolLayout->addStretch();

    // returning this layout to be added to the main layout
    return toolPanel;
}

MainWindow::~MainWindow()
{
    // Automatic cleanup through parent-child hierarchy
}
