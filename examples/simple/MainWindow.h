#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ed/dockmenu/MenuManager.h>

#include <QMainWindow>

/**
 * This example shows, how to place a dock widget container and a static
 * sidebar into a QMainWindow
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    /// The menu manager for the dock widgets.
    ed::EMenuManager *menuManager;
};

#endif  // MAINWINDOW_H
