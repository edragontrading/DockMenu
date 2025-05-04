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

#include "ed/dockmenu/MenuTitleBar.h"

#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QStyle>
#include <QToolButton>

#include "ed/dockmenu/DragPreview.h"
#include "ed/dockmenu/MenuFloating.h"
#include "ed/dockmenu/MenuManager.h"
#include "ed/dockmenu/MenuTitleBar_p.h"

namespace ed {

struct EMenuTitleBar::Private {
    Private() = default;

    bool floating;
    eDragState dragState;
    QPoint globalDragStartMousePosition;
    QPoint dragStartMousePosition;

    QLabel* title;
    QBoxLayout* layout;
    QToolButton* undockButton;
    QToolButton* dockButton;
    EDragPreview* dragPreviewWidget;

    EMenuManager* menuManager;
};

EMenuTitleBar::EMenuTitleBar(EMenuManager* manager, const QString& title, QWidget* parent)
    : QFrame(parent), d(new Private()) {
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);

    setObjectName("EMenuTitleBar");

    d->floating = false;
    d->menuManager = manager;
    d->dragState = DraggingInactive;
    d->dragPreviewWidget = nullptr;

    d->layout = new QBoxLayout(QBoxLayout::LeftToRight);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    setLayout(d->layout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    d->undockButton = new QToolButton(this);
    d->undockButton->setObjectName("EMenuDetachMenuButton");
    d->undockButton->setAutoRaise(true);
    internal::setToolTip(d->undockButton, QObject::tr("Detach Menu"));
    internal::setButtonIcon(d->undockButton, QStyle::SP_TitleBarNormalButton, ed::MenuUndockIcon);

    d->undockButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    d->layout->addWidget(d->undockButton, 0);
    this->connect(d->undockButton, SIGNAL(clicked()), SLOT(onUndockButtonClicked()));

    d->dockButton = new QToolButton(this);
    d->dockButton->setObjectName("EMenuAtachMenuButton");
    d->dockButton->setAutoRaise(true);
    internal::setToolTip(d->dockButton, QObject::tr("Attach Menu"));
    internal::setButtonIcon(d->dockButton, QStyle::SP_TitleBarNormalButton, ed::MenuDockIcon);

    d->dockButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    d->layout->addWidget(d->dockButton, 0);
    this->connect(d->dockButton, SIGNAL(clicked()), SLOT(onDockButtonClicked()));

    d->title = new QLabel(title, this);
    d->title->setObjectName("EMenuTitleLabel");

    d->title->setVisible(true);
    d->title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    d->title->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->title->setContentsMargins(0, 0, 0, 0);

    d->layout->insertWidget(0, d->title, 1);
}

EMenuTitleBar::~EMenuTitleBar() {
    delete d;
}

void EMenuTitleBar::updateState(bool floating) {
    d->floating = floating;
    if (floating) {
        d->undockButton->setVisible(false);
        d->dockButton->setVisible(true);
    } else {
        d->dockButton->setVisible(false);
        d->undockButton->setVisible(true);
    }
}

void EMenuTitleBar::mouseDoubleClickEvent(QMouseEvent* event) {
    if (d->floating) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        QSize size = d->menuManager->getMenuSize();
        QPoint point = event->pos();

        EMenuFloating* floating = new EMenuFloating(d->menuManager);
        floating->startFloating(point, size, DraggingInactive);
    }
}

void EMenuTitleBar::mousePressEvent(QMouseEvent* ev) {
    if (ev->button() == Qt::LeftButton) {
        ev->accept();
        this->saveDragStartMousePosition(internal::globalPositionOf(ev));
        d->dragState = DraggingMousePressed;
        return;
    }
    Super::mousePressEvent(ev);
}

void EMenuTitleBar::mouseReleaseEvent(QMouseEvent* ev) {
    if (ev->button() != Qt::LeftButton) {
        Super::mouseReleaseEvent(ev);
        return;
    }

    auto currentDragState = d->dragState;
    d->globalDragStartMousePosition = QPoint();
    d->dragStartMousePosition = QPoint();
    d->dragState = DraggingInactive;

    if (DraggingFloatingWidget == currentDragState) {
        ev->accept();
        d->dragPreviewWidget->finishDragging();
    }
    Super::mouseReleaseEvent(ev);
}

void EMenuTitleBar::mouseMoveEvent(QMouseEvent* ev) {
    if (!(ev->buttons() & Qt::LeftButton) || this->isDraggingState(DraggingInactive)) {
        d->dragState = DraggingInactive;
        Super::mouseMoveEvent(ev);
        return;
    }

    // move floating window
    if (this->isDraggingState(DraggingFloatingWidget)) {
        d->dragPreviewWidget->moveFloating();
        Super::mouseMoveEvent(ev);
        return;
    }

    auto mappedPos = mapToParent(ev->pos());
    bool mouseOutsideBar = (mappedPos.x() < 0) || (mappedPos.x() > parentWidget()->rect().right());

    // Maybe a fixed drag distance is better here ?
    int DragDistanceY = qAbs(d->globalDragStartMousePosition.y() - internal::globalPositionOf(ev).y());
    if (DragDistanceY >= EMenuManager::startDragDistance() || mouseOutsideBar) {
        QSize size = d->menuManager->getMenuSize();

        d->dragState = DraggingFloatingWidget;
        d->dragPreviewWidget = new EDragPreview(d->menuManager);
        this->connect(d->dragPreviewWidget, &EDragPreview::draggingCanceled,
                      [this]() { this->d->dragState = DraggingInactive; });

        d->dragPreviewWidget->startFloating(d->dragStartMousePosition, size);
        return;
    }

    Super::mouseMoveEvent(ev);
}

void EMenuTitleBar::onUndockButtonClicked() {
    QSize size = d->menuManager->getMenuSize();
    QPoint point = this->mapFromGlobal(QCursor::pos());

    EMenuFloating* floating = new EMenuFloating(d->menuManager);
    floating->startFloating(point, size, DraggingInactive);
}

void EMenuTitleBar::onDockButtonClicked() {
    d->menuManager->redockMenu(false);
}

void EMenuTitleBar::saveDragStartMousePosition(const QPoint& globalPos) {
    d->globalDragStartMousePosition = globalPos;
    d->dragStartMousePosition = this->mapFromGlobal(globalPos);
}

bool EMenuTitleBar::isDraggingState(eDragState dragState) const {
    return d->dragState == dragState;
}

ESpacerWidget::ESpacerWidget(QWidget* Parent /*= nullptr*/) : Super(Parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("border: none; background: none;");
}

}  // namespace ed
