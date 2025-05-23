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

#include "ed/dockmenu/MouseTracker.h"

#include <QCursor>
#include <QDebug>

namespace ed {

MouseTracker::MouseTracker(QObject* parent) : QObject(parent), m_timer(new QTimer(this)) {
    connect(m_timer, &QTimer::timeout, this, &MouseTracker::trackMouse);
    start();
}

MouseTracker::~MouseTracker() {
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    delete m_timer;
}

MouseTracker& MouseTracker::instance() {
    if (!m_instance) {
        m_instance = new MouseTracker();
    }
    return *m_instance;
}

void MouseTracker::start(int intervalMs) {
    if (!m_timer->isActive()) {
        m_timer->start(intervalMs);
    }
}

void MouseTracker::stop() {
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

void MouseTracker::trackMouse() {
    QPoint pos = QCursor::pos();
    Q_EMIT mouseMoved(pos);  // Optional: emit if you want observers
}
}  // namespace ed
