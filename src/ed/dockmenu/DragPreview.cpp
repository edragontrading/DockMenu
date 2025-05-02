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

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QScreen>
#include <QWindow>

#include "ed/dockmenu/MenuFloating.h"
#include "ed/dockmenu/MenuManager.h"
#include "ed/dockmenu/MenuOverlay.h"

namespace ed {

struct EDragPreview::Private {
    Private() = default;

    bool dragCanceled;

    QLabel* label;
    QHBoxLayout* layout;

    EMenuManager* menuManager;
    QPixmap contentPreviewPixmap;
    QPoint dragStartMousePosition;
};

EDragPreview::EDragPreview(EMenuManager* menuManager, QWidget* parent) : QWidget(parent), d(new Private) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    auto Flags = windowFlags();
    Flags |= Qt::X11BypassWindowManagerHint;
    setWindowFlags(Flags);
#endif

    d->menuManager = menuManager;
    d->dragCanceled = false;
    d->contentPreviewPixmap = menuManager->captureMenuWidgets();

    d->layout = new QHBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);

    d->label = new QLabel(this);
    d->label->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    d->layout->addWidget(d->label, 1);
    setLayout(d->layout);

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
    QSizeF reSize = QSizeF(qMin((double)size.width(), screenSize.width() * 0.8),
                           qMin((double)size.height(), screenSize.height() * 0.8));

    d->contentPreviewPixmap =
        d->contentPreviewPixmap.scaled(reSize.width(), reSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    d->label->setPixmap(d->contentPreviewPixmap);

    adjustSize();
    d->dragStartMousePosition = dragStartMousePos;
    moveFloating();
    show();
}

void EDragPreview::moveFloating() {
    int borderSize = (frameSize().width() - size().width()) / 2;
    const QPoint moveToPos = QCursor::pos() - d->dragStartMousePosition - QPoint(borderSize, 0);
    move(moveToPos);

    if (d->menuManager->floating()) {
        this->updateDropOverlays(QCursor::pos());
    }
}

void EDragPreview::finishDragging() {
    QSize size = d->menuManager->getMenuSize();
    QPoint point = this->mapFromGlobal(QCursor::pos());

    this->close();
    if (d->menuManager->floating()) {
        auto dropArea = d->menuManager->menuOverlay()->dropAreaUnderCursor();
        d->menuManager->menuOverlay()->hideOverlay();

        if (dropArea != InvalidMenuWidgetArea) {
            d->menuManager->redockMenu(false);
            return;
        }
    }

    EMenuFloating* floating = new EMenuFloating(d->menuManager);
    floating->startFloating(point, size, DraggingInactive);
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

    d->menuManager->menuOverlay()->hideOverlay();
    this->close();
}

void EDragPreview::onApplicationStateChanged(Qt::ApplicationState state) {
    if (state != Qt::ApplicationActive) {
        disconnect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this,
                   SLOT(onApplicationStateChanged(Qt::ApplicationState)));
        this->cancelDragging();
    }
}

void EDragPreview::updateDropOverlays(const QPoint& globalPos) {
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
