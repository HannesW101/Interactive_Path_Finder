#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QLabel>
#include "gridmodel.h"
#include "gridview.h"

class MainWindow : public QMainWindow
{
    // macro to enable signals and slots (+ meta-object features)
    Q_OBJECT

public:
    // constructor for MainWindow, usually nullptr as the MainWindow is the base window with no parent
    MainWindow(QWidget *parent = nullptr);

    // simple destructor
    ~MainWindow();

private:
    // create Model and View objects
    GridModel *m_model;
    GridView *m_view;

    // create a variable from our list of enums in GridModel to store the currently selected terrain from the UI (default is normal)
    GridModel::CellType m_currentTool = GridModel::Normal;

    // button group for radio buttons for tools
    QButtonGroup *m_toolGroup;

    // setup tool buttons in UI for the user to choose from
    QWidget* createToolButtons();

    // label to display total cost of optimal path to the user
    QLabel* m_costLabel;
};
#endif // MAINWINDOW_H
