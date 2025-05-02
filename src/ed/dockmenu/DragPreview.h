#ifndef ED_DOCKMENU_DRAG_PREVIEW_H
#define ED_DOCKMENU_DRAG_PREVIEW_H

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

#include "ed/dockmenu/ed_menu_globals.h"

namespace ed {

class EMenuManager;

class ED_EXPORT EDragPreview : public QWidget {
    Q_OBJECT

public:
    explicit EDragPreview(EMenuManager* manager, QWidget* parent = nullptr);
    ~EDragPreview() override;

    /**
     * We filter the events of the assigned content widget to receive
     * escape key presses for canceling the drag operation
     */
    bool eventFilter(QObject* watched, QEvent* event) override;

public:
    void startFloating(const QPoint& dragStartMousePos, const QSize& size);

    /**
     * Moves the widget to a new position relative to the position given when
     * startFloating() was called
     */
    void moveFloating();

    /**
     * Finishes dragging.
     * Hides the dock overlays and executes the real undocking and docking
     * of the assigned Content widget
     */
    void finishDragging();

Q_SIGNALS:
    /**
     * This signal is emitted, if dragging has been canceled by escape key
     * or by active application switching via task manager
     */
    void draggingCanceled();

private Q_SLOTS:
    /**
     * Cancel non opaque undocking if application becomes inactive
     */
    void onApplicationStateChanged(Qt::ApplicationState state);

protected:
    /**
     * Cares about painting the
     */
    void paintEvent(QPaintEvent* e) override;

private:
    void cancelDragging();
    void updateDropOverlays(const QPoint& globalPos);

private:
    struct Private;
    Private* d;
};

}  // namespace ed

#endif  // ED_DOCKMENU_DRAG_PREVIEW_H