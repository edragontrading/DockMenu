/*******************************************************************************
** Qt Dock Menu System
** Copyright (C) 2025 ED Trading
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 3.0 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

//============================================================================
/// \author Phuoc Truong
/// \date   25.04.2025
//============================================================================

#include "ed/dockmenu/MenuFloating.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCloseEvent>

#include "ed/dockmenu/MenuAreaWidget.h"
#include "ed/dockmenu/MenuManager.h"
#include "ed/dockmenu/MenuTabBar.h"

namespace ed {

struct EMenuFloating::Private {
    EMenuManager* menuManager;
    QBoxLayout* layout;
    EMenuTabBar* tabBar;
    EMenuAreaWidget* menuAreaWidget;

    eDragState draggingState = DraggingInactive;
    QPoint dragStartMousePosition;
};

EMenuFloating::EMenuFloating(EMenuManager* menuManager) : QWidget(menuManager), d(new Private) {
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint |
                   Qt::WindowMaximizeButtonHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QApplication::windowIcon());
    setWindowTitle(tr("Menu"));

    d->menuManager = menuManager;

    switch (menuManager->direction()) {
        case MenuDirection::Left:
            d->layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
            break;

        case MenuDirection::Right:
            d->layout = new QBoxLayout(QBoxLayout::RightToLeft, this);
            break;

        case MenuDirection::Top:
            d->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
            break;

        case MenuDirection::Bottom:
            d->layout = new QBoxLayout(QBoxLayout::BottomToTop, this);
            break;

        default:
            break;
    }

    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);

    d->menuAreaWidget = d->menuManager->takeMenuAreaWidget();
    d->tabBar = d->menuManager->takeTabBar();
    connect(d->tabBar, &EMenuTabBar::toolSelected, this, &EMenuFloating::onToolSelected);
    connect(d->tabBar, &EMenuTabBar::toolClosed, this, &EMenuFloating::onToolClosed);

    d->layout->addWidget(d->tabBar, 1);
    d->layout->addWidget(d->menuAreaWidget);

    setLayout(d->layout);

    d->menuManager->registerFloatingWidget(this);
}

EMenuFloating::~EMenuFloating() {
    ED_PRINT("EMenuFloating::~EMenuFloating");
    delete d;
}

void EMenuFloating::removeMenuWidget() {
    disconnect(d->tabBar, &EMenuTabBar::toolSelected, this, &EMenuFloating::onToolSelected);
    disconnect(d->tabBar, &EMenuTabBar::toolClosed, this, &EMenuFloating::onToolClosed);

    d->layout->removeWidget(d->tabBar);
    d->layout->removeWidget(d->menuAreaWidget);

    d->tabBar = nullptr;
    d->menuAreaWidget = nullptr;
}

void EMenuFloating::closeEvent(QCloseEvent* event) {
    d->menuManager->redockMenu(true);
    event->accept();  // Proceed with close
}

void EMenuFloating::onToolSelected(int index) {
    d->menuAreaWidget->toolSelected(index);
}

void EMenuFloating::onToolClosed() {
    int currentIndex = d->menuAreaWidget->getCurrentIndex();
    if (currentIndex >= 0) {
        d->tabBar->setSelected(currentIndex);
    }
}

void EMenuFloating::startFloating(const QPoint& dragStartMousePos, const QSize& size, eDragState dragState) {
    QScreen* screen = this->screen();
    QSize screenSize = screen->geometry().size();
    QSize reSize = QSize(qMin(size.width(), (int)(screenSize.width() * 0.85)),
                         qMin(size.height(), (int)(screenSize.height() * 0.85)));

    resize(reSize);
    d->dragStartMousePosition = dragStartMousePos;
    d->draggingState = dragState;
    moveFloating();
    show();
}

void EMenuFloating::moveFloating() {
    int borderSize = (frameSize().width() - size().width()) / 2;
    const QPoint moveToPos = QCursor::pos() - d->dragStartMousePosition - QPoint(borderSize, 0);
    move(moveToPos);

    switch (d->draggingState) {
        case DraggingMousePressed:
            d->draggingState = DraggingFloatingWidget;
            // d->updateDropOverlays(QCursor::pos());
            break;

        case DraggingFloatingWidget:
            // d->updateDropOverlays(QCursor::pos());
#ifdef Q_OS_MACOS
            // In OSX when hiding the MenuAreaOverlay the application would set
            // the main window as the active window for some reason. This fixes
            // that by resetting the active window to the floating widget after
            // updating the overlays.
            activateWindow();
#endif
            break;
        default:
            break;
    }
}

}  // namespace ed