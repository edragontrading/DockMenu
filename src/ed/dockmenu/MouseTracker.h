#ifndef ED_DOCKMENU_MOUSE_TRACKER_H
#define ED_DOCKMENU_MOUSE_TRACKER_H

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

#include <ed/dockmenu/ed_menu_globals.h>

#include <QObject>
#include <QPoint>
#include <QTimer>

namespace ed {

class ED_EXPORT MouseTracker : public QObject {
    Q_OBJECT

public:
    static MouseTracker& instance();
    void stop();

    ~MouseTracker() override;

Q_SIGNALS:
    void mouseMoved(const QPoint& pos);  // Optional signal if others need to observe

public Q_SLOTS:
    void trackMouse();

private:
    explicit MouseTracker(QObject* parent = nullptr);
    void start(int intervalMs = 120);

private:
    inline static MouseTracker* m_instance = nullptr;
    QTimer* m_timer;
};
}  // namespace ed

#endif  // ED_DOCKMENU_MOUSE_TRACKER_H
