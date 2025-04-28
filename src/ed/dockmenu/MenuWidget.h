#ifndef ED_DOCKMENU_WIDGET_H
#define ED_DOCKMENU_WIDGET_H

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

#include <QScrollArea>

#include "ed/dockmenu/ed_menu_globals.h"

namespace ed {

class ED_EXPORT EMenuWidget : public QScrollArea {
    Q_OBJECT

public:
    explicit EMenuWidget(const QString& name, QWidget* widget, QWidget* parent = nullptr);
    ~EMenuWidget();

Q_SIGNALS:
    void undockClicked();
    void dockClicked();

private Q_SLOTS:
    void onUndockButtonClicked();
    void onDockButtonClicked();

private:
    struct Private;
    Private* d;
};
}  // namespace ed

#endif  // ED_DOCKMENU_WIDGET_H