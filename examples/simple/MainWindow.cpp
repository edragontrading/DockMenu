#include "MainWindow.h"

#include <ed/dockmenu/Provider.h>

#include <QAction>
#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPoint>
#include <QPushButton>
#include <QScreen>
#include <QToolBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("MainWindow");
    setObjectName("MainWindow");

    // Set the window icon
    setWindowIcon(QIcon(":/ed/icons/terminal.svg"));

    ed::EMenuManager::provider().registerCustomColor(ed::eColor::MenuButtonHighlight, QColor(255, 0, 0));
    ed::EMenuManager::provider().registerCustomColor(ed::eColor::QSplitterHandleColor, QColor(255, 0, 0));

    this->menuManager = new ed::EMenuManager(ed::MenuDirection::Left, this);

    this->menuManager->addMenu("Terminal 11", ":/ed/icons/light/broadcast.svg", ":/ed/icons/dark/broadcast.svg",
                               "Terminal 11", new QTextEdit("Menu Pane 1", this));

    this->menuManager->addMenu("Terminal 22", ":/ed/icons/light/bookmark.svg", ":/ed/icons/dark/bookmark.svg",
                               "Terminal", new QTextEdit("Menu Pane 2", this));

    this->menuManager->addMenu("Terminal 33", ":/ed/icons/light/bug.svg", ":/ed/icons/dark/bug.svg", "Terminal 33",
                               new QTextEdit("Menu Pane 3", this));

    this->menuManager->setCentralWidget(new QPlainTextEdit("Central Widget", this));
}

MainWindow::~MainWindow() {
    delete menuManager;
}