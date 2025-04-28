#ifndef ED_DOCKMENU_TABBAR_H
#define ED_DOCKMENU_TABBAR_H

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

class EMenuButton;

class ED_EXPORT EMenuTabBar : public QScrollArea {
    Q_OBJECT

public:
    explicit EMenuTabBar(MenuDirection direction, QWidget* parent = nullptr);
    ~EMenuTabBar();

    void setSize(int size);
    void setClose();
    void setSelected(int index);
    int getSelected() const;

    void addMenuButton(EMenuButton* button);

Q_SIGNALS:
    void toolSelected(int index);
    void toolClosed();

private:
    struct Private;
    Private* d;
};
}  // namespace ed

#endif  // ED_DOCKMENU_TABBAR_H