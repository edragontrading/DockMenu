#ifndef ED_DOCKMENU_TITLEBAR_P_H
#define ED_DOCKMENU_TITLEBAR_P_H

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

#include <QWidget>

#include "ed/dockmenu/ed_menu_globals.h"

namespace ed {

/**
 * This spacer widget is here because of the following problem.
 * The menu area title bar handles mouse dragging and moving the floating widget.
 * The  problem is, that if the title bar becomes invisible, i.e. if the dock
 * area contains only one single menu widget and the menu area is moved
 * into a floating widget, then mouse events are not handled anymore and dragging
 * of the floating widget stops.
 */
class ESpacerWidget : public QWidget {
    Q_OBJECT
public:
    using Super = QWidget;
    ESpacerWidget(QWidget* Parent = nullptr);

    virtual QSize sizeHint() const override {
        return QSize(0, 0);
    }

    virtual QSize minimumSizeHint() const override {
        return QSize(0, 0);
    }
};

}  // namespace ed

#endif  // ED_DOCKMENU_TITLEBAR_P_H