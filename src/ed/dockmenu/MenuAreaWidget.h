#ifndef ED_DOCKMENU_AREA_WIDGET_H_
#define ED_DOCKMENU_AREA_WIDGET_H_

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

class EMenuWidget;

class ED_EXPORT EMenuAreaWidget : public QWidget {
    Q_OBJECT

public:
    explicit EMenuAreaWidget(MenuDirection direction, QWidget* parent = nullptr);
    ~EMenuAreaWidget() override;

    void addMenuWidget(EMenuWidget* widget);

    int getCurrentIndex() const;

    void updateState(bool floating);

public Q_SLOTS:
    void toolSelected(int index);

private:
    struct Private;
    Private* d;
};
}  // namespace ed

#endif  // ED_DOCKMENU_AREA_WIDGET_H_