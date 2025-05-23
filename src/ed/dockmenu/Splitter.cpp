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

#include "ed/dockmenu/Splitter.h"

#include <QPainter>
#include <cmath>

#include "ed/dockmenu/MouseTracker.h"

namespace ed {

struct ESplitterHandle::Private {
    Private() = default;

    bool m_mousePressed = false;
    bool m_hover = false;
    QColor m_color;
};

ESplitterHandle::ESplitterHandle(Qt::Orientation orientation, QSplitter* parent)
    : QSplitterHandle(orientation, parent), d(new Private) {
    setMouseTracking(true);
    d->m_color = internal::getCustomColor(ed::eColor::QSplitterHandleColor);
    if (!d->m_color.isValid()) {
        d->m_color = QColor(0x3498db);
    }
}

ESplitterHandle::~ESplitterHandle() {
    ED_PRINT("ESplitterHandle::~ESplitterHandle()");
    delete d;
}

QSize ESplitterHandle::sizeHint() const {
    return d->m_hover ? (orientation() == Qt::Horizontal ? QSize(4, 0) : QSize(0, 4))
                      : (orientation() == Qt::Horizontal ? QSize(1, 0) : QSize(0, 1));
}

QColor ESplitterHandle::handleColor() const {
    return d->m_color;
}

void ESplitterHandle::setHandleColor(const QColor& Color) {
    d->m_color = Color;
}

bool ESplitterHandle::setVisibleOnHover(bool hover) {
    if (d->m_hover != hover) {
        d->m_hover = hover;
        updateGeometry();  // Important: tell layout to re-calculate size
        update();          // Repaint

        return true;
    }

    return false;
}

bool ESplitterHandle::hovered() {
    return d->m_hover;
}

bool ESplitterHandle::mousePressed() {
    return d->m_mousePressed;
}

void ESplitterHandle::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        d->m_mousePressed = true;
    }
    QSplitterHandle::mousePressEvent(event);
}

void ESplitterHandle::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        d->m_mousePressed = false;
        Q_EMIT dragFinished();
    }
    QSplitterHandle::mouseReleaseEvent(event);
}

void ESplitterHandle::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    if (d->m_hover) {
        painter.fillRect(rect(), d->m_color);
    }
}

//=================================================================================

ESplitter::ESplitter(Qt::Orientation orientation, QWidget* parent) : QSplitter(orientation, parent) {
    setMouseTracking(true);
    QObject::connect(&MouseTracker::instance(), &MouseTracker::mouseMoved, this, &ESplitter::mouseMoved);
}

ESplitter::~ESplitter() {
    ED_PRINT("ESplitter::~ESplitter()");
    QObject::disconnect(&MouseTracker::instance(), &MouseTracker::mouseMoved, this, &ESplitter::mouseMoved);
}

QSplitterHandle* ESplitter::createHandle() {
    auto* handle = new ESplitterHandle(orientation(), this);
    return handle;
}

void ESplitter::mouseMoveEvent(QMouseEvent* event) {
    QSplitter::mouseMoveEvent(event);
    updateHandleVisibility(false, event->pos());
}

void ESplitter::mouseMoved(const QPoint& mousePos) {
    QPoint localPos = mapFromGlobal(mousePos);
    updateHandleVisibility(true, localPos);
}

void ESplitter::showEvent(QShowEvent* event) {
    QSplitter::showEvent(event);
    Q_EMIT splitterReady();
}

void ESplitter::updateHandleVisibility(bool timerEvent, const QPoint& mousePos) {
    bool update = false;
    for (int index = 1; index < this->count(); ++index) {
        ESplitterHandle* handle = static_cast<ESplitterHandle*>(this->handle(index));
        if (handle && handle->mousePressed()) {
            return;
        }
    }

    for (int index = 1; index < this->count(); ++index) {
        ESplitterHandle* handle = static_cast<ESplitterHandle*>(this->handle(index));
        if (handle == nullptr) {
            continue;
        }

        if (timerEvent) {
            if (!handle->hovered()) {
                continue;
            }
        }

        QRect handleRect = handle->geometry();
        int proximity = 2;  // How close mouse needs to be to trigger visibility
        QRect hoverZone = handleRect.adjusted(-proximity, -proximity, proximity, proximity);
        update = handle->setVisibleOnHover(hoverZone.contains(mousePos));
        if (update) {
            handle->setCursor(orientation() == Qt::Horizontal ? Qt::SplitHCursor : Qt::SplitVCursor);
            break;
        }
    }

    if (update) {
        // Force layout update to respect new sizeHints
        updateGeometry();
    }
}

}  // namespace ed