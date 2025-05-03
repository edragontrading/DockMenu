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

#ifdef Q_OS_WIN
#include <windows.h>
#ifdef _MSC_VER
#pragma comment(lib, "User32.lib")
#endif
#endif

#include "ed/dockmenu/MenuAreaWidget.h"
#include "ed/dockmenu/MenuManager.h"
#include "ed/dockmenu/MenuOverlay.h"
#include "ed/dockmenu/MenuTabBar.h"

namespace ed {

struct EMenuFloating::Private {
    EMenuManager* menuManager;
    QBoxLayout* layout;
    EMenuTabBar* tabBar;
    EMenuAreaWidget* menuAreaWidget;

    eDragState draggingState = DraggingInactive;
    QPoint dragStartMousePosition;
    QPoint dragStartPos;

    bool isClosing;
};

EMenuFloating::EMenuFloating(EMenuManager* menuManager) : QWidget(menuManager), d(new Private) {
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint |
                   Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QApplication::windowIcon());
    setWindowTitle(tr("Menu"));

    d->menuManager = menuManager;
    d->isClosing = false;

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
    d->isClosing = true;
    if (d->tabBar != nullptr) {
        disconnect(d->tabBar, &EMenuTabBar::toolSelected, this, &EMenuFloating::onToolSelected);
        disconnect(d->tabBar, &EMenuTabBar::toolClosed, this, &EMenuFloating::onToolClosed);
        d->layout->removeWidget(d->tabBar);
        d->tabBar = nullptr;
    }

    if (d->menuAreaWidget != nullptr) {
        d->layout->removeWidget(d->menuAreaWidget);
        d->menuAreaWidget = nullptr;
    }
}

void EMenuFloating::closeEvent(QCloseEvent* event) {
    if (!d->isClosing) {
        d->menuManager->redockMenu(true);
    }
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
            break;

        case DraggingFloatingWidget:
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

#ifdef Q_OS_WIN
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
bool EMenuFloating::nativeEvent(const QByteArray& eventType, void* message, long* result)
#else
bool EMenuFloating::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
#endif
{
    QWidget::nativeEvent(eventType, message, result);
    MSG* msg = static_cast<MSG*>(message);
    switch (msg->message) {
        case WM_MOVING: {
            if (d->draggingState == DraggingFloatingWidget) {
                this->updateDropOverlays(QCursor::pos());
            }
        } break;

        case WM_NCLBUTTONDOWN:
            if (msg->wParam == HTCAPTION && d->draggingState == DraggingInactive) {
                d->dragStartPos = pos();
                d->draggingState = DraggingMousePressed;
            }
            break;

        case WM_NCLBUTTONDBLCLK:
            d->draggingState = DraggingInactive;
            break;

        case WM_ENTERSIZEMOVE:
            if (d->draggingState == DraggingMousePressed) {
                d->draggingState = DraggingFloatingWidget;
                this->updateDropOverlays(QCursor::pos());
            }
            break;

        case WM_EXITSIZEMOVE:
            if (d->draggingState == DraggingFloatingWidget) {
                if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                    this->handleEscapeKey();
                } else {
                    this->titleMouseReleaseEvent();
                }
            }
            break;
    }
    return false;
}
#endif

#ifdef Q_OS_MACOS
//============================================================================
bool EMenuFloating::event(QEvent* e) {
    switch (d->draggingState) {
        case DraggingInactive: {
            // Normally we would check here, if the left mouse button is pressed.
            // But from QT version 5.12.2 on the mouse events from
            // QEvent::NonClientAreaMouseButtonPress return the wrong mouse button
            // The event always returns Qt::RightButton even if the left button
            // is clicked.
            // It is really great to work around the whole NonClientMouseArea
            // bugs
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 2))
            if (e->type() ==
                QEvent::NonClientAreaMouseButtonPress /*&& QGuiApplication::mouseButtons().testFlag(Qt::LeftButton)*/)
#else
            if (e->type() == QEvent::NonClientAreaMouseButtonPress &&
                QGuiApplication::mouseButtons().testFlag(Qt::LeftButton))
#endif
            {
                ED_PRINT("EMenuFloating::event Event::NonClientAreaMouseButtonPress" << e->type());
                d->dragStartPos = pos();
                d->draggingState = DraggingMousePressed;
            }
        } break;

        case DraggingMousePressed:
            switch (e->type()) {
                case QEvent::NonClientAreaMouseButtonDblClick:
                    ED_PRINT("EMenuFloating::event QEvent::NonClientAreaMouseButtonDblClick");
                    d->draggingState = DraggingInactive;
                    break;

                case QEvent::Resize:
                    // If the first event after the mouse press is a resize event, then
                    // the user resizes the window instead of dragging it around.
                    // But there is one exception. If the window is maximized,
                    // then dragging the window via title bar will cause the widget to
                    // leave the maximized state. This in turn will trigger a resize event.
                    // To know, if the resize event was triggered by user via moving a
                    // corner of the window frame or if it was caused by a windows state
                    // change, we check, if we are not in maximized state.
                    if (!isMaximized()) {
                        d->draggingState = DraggingInactive;
                    }
                    break;

                default:
                    break;
            }
            break;

        case DraggingFloatingWidget:
            if (e->type() == QEvent::NonClientAreaMouseButtonRelease) {
                ED_PRINT("EMenuFloating::event QEvent::NonClientAreaMouseButtonRelease");
                this->titleMouseReleaseEvent();
            }
            break;

        default:
            break;
    }

    ED_PRINT(QTime::currentTime() << "EMenuFloating::event " << e->type());
    return QWidget::event(e);
}

//============================================================================
void EMenuFloating::moveEvent(QMoveEvent* event) {
    QWidget::moveEvent(event);
    switch (d->draggingState) {
        case DraggingMousePressed:
            d->draggingState = DraggingFloatingWidget;
            this->updateDropOverlays(QCursor::pos());
            break;

        case DraggingFloatingWidget:
            this->updateDropOverlays(QCursor::pos());
            // In OSX when hiding the DockAreaOverlay the application would set
            // the main window as the active window for some reason. This fixes
            // that by resetting the active window to the floating widget after
            // updating the overlays.
            activateWindow();
            break;
        default:
            break;
    }
}
#endif

void EMenuFloating::titleMouseReleaseEvent() {
    d->draggingState = DraggingInactive;

    auto dropArea = d->menuManager->menuOverlay()->dropAreaUnderCursor();
    d->menuManager->menuOverlay()->hideOverlay();

    if (dropArea != InvalidMenuWidgetArea) {
        QMetaObject::invokeMethod(
            this, [this]() { this->d->menuManager->redockMenu(false); }, Qt::QueuedConnection);
    }
}

void EMenuFloating::handleEscapeKey() {
    ED_PRINT("EMenuFloating::handleEscapeKey()");
    d->draggingState = DraggingInactive;
    d->menuManager->menuOverlay()->hideOverlay();
}

void EMenuFloating::updateDropOverlays(const QPoint& globalPos) {
    if (!this->isVisible() || !d->menuManager->floating()) {
        return;
    }

    bool inMenuArea = false;
    QPoint mappedPos = d->menuManager->mapFromGlobal(globalPos);
    if (d->menuManager->rect().contains(mappedPos)) {
        inMenuArea = true;
    }

    if (!inMenuArea) {
        d->menuManager->menuOverlay()->hideOverlay();
        setHidden(false);
        return;
    }
    auto dropArea = d->menuManager->menuOverlay()->dropAreaUnderCursor();
    if (dropArea != InvalidMenuWidgetArea) {
        d->menuManager->menuOverlay()->enableDropPreview(true);
    } else {
        d->menuManager->menuOverlay()->enableDropPreview(false);
    }

    d->menuManager->menuOverlay()->showOverlay(d->menuManager);
}

}  // namespace ed
