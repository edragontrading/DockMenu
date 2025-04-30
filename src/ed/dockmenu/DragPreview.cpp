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

#include "ed/dockmenu/DragPreview.h"

#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QScreen>

#include "ed/dockmenu/MenuFloating.h"
#include "ed/dockmenu/MenuManager.h"

namespace ed {

struct EDragPreview::Private {
    Private() = default;

    bool dragCanceled;

    EMenuManager* menuManager;
    QPixmap contentPreviewPixmap;
    QPoint dragStartMousePosition;
};

EDragPreview::EDragPreview(EMenuManager* menuManager, QWidget* parent) : QWidget(parent), d(new Private) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    auto Flags = windowFlags();
    Flags |= Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint;
    setWindowFlags(Flags);
#endif

    d->menuManager = menuManager;
    d->dragCanceled = false;
    d->contentPreviewPixmap = menuManager->captureMenuWidgets();

    connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            SLOT(onApplicationStateChanged(Qt::ApplicationState)));

    // The only safe way to receive escape key presses is to install an event
    // filter for the application object
    qApp->installEventFilter(this);
}

EDragPreview::~EDragPreview() {
    ED_PRINT("EDragPreview::~EDragPreview()");
    delete d;
}

void EDragPreview::startFloating(const QPoint& dragStartMousePos, const QSize& size) {
    QScreen* screen = this->screen();
    QSize screenSize = screen->geometry().size();
    QSize reSize = QSize(qMin(size.width(), (int)(screenSize.width() * 0.85)),
                         qMin(size.height(), (int)(screenSize.height() * 0.85)));

    d->contentPreviewPixmap =
        d->contentPreviewPixmap.scaled(reSize.width(), reSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    resize(reSize);
    d->dragStartMousePosition = dragStartMousePos;
    moveFloating();
    show();
}

void EDragPreview::moveFloating() {
    int borderSize = (frameSize().width() - size().width()) / 2;
    const QPoint moveToPos = QCursor::pos() - d->dragStartMousePosition - QPoint(borderSize, 0);
    move(moveToPos);
    // d->updateDropOverlays(QCursor::pos());
}

void EDragPreview::finishDragging() {
    ED_PRINT("EDragPreview::finishDragging");

    QSize size = d->menuManager->getMenuSize();
    QPoint point = this->mapFromGlobal(QCursor::pos());

    EMenuFloating* floating = new EMenuFloating(d->menuManager);
    floating->startFloating(point, size, DraggingInactive);

    this->close();
}

bool EDragPreview::eventFilter(QObject* watched, QEvent* event) {
    if (!d->dragCanceled && event->type() == QEvent::KeyPress) {
        QKeyEvent* e = static_cast<QKeyEvent*>(event);
        if (e->key() == Qt::Key_Escape) {
            watched->removeEventFilter(this);
            this->cancelDragging();
        }
    }

    return false;
}

void EDragPreview::cancelDragging() {
    d->dragCanceled = true;
    Q_EMIT draggingCanceled();

    this->close();
}

void EDragPreview::onApplicationStateChanged(Qt::ApplicationState state) {
    if (state != Qt::ApplicationActive) {
        disconnect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this,
                   SLOT(onApplicationStateChanged(Qt::ApplicationState)));
        this->cancelDragging();
    }
}

void EDragPreview::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setOpacity(0.6);
    painter.drawPixmap(QPoint(0, 0), d->contentPreviewPixmap);
}

}  // namespace ed