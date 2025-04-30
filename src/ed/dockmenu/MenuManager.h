#ifndef ED_DOCKMENU_MANAGER_H
#define ED_DOCKMENU_MANAGER_H

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

#include <QFrame>

#include "ed/dockmenu/ed_menu_globals.h"

namespace ed {

class EProvider;
class EMenuTabBar;
class EMenuAreaWidget;
class EMenuFloating;

class ED_EXPORT EMenuManager : public QFrame {
    Q_OBJECT
public:
    explicit EMenuManager(MenuDirection direction, QWidget* parent = nullptr);
    ~EMenuManager() override;

    void addMenu(const QString& name, const QString& iconNormal, const QString& iconActive, const QString& tooltip,
                 QWidget* widget);

    void setCentralWidget(QWidget* widget);

    MenuDirection direction() const;

    QSize getMenuSize() const;

    EMenuTabBar* takeTabBar();
    EMenuAreaWidget* takeMenuAreaWidget();

    void redockMenu(bool closed);
    void updateFloatingState(bool floating);

    void registerFloatingWidget(EMenuFloating* floatingWidget);

    static EProvider& provider();

private:
    void loadStylesheet();
    void setDefaultSize();
    void setClosedSize();
    bool menuVisible();

private Q_SLOTS:
    void onMenuDragFinished();
    void onToolClosed();
    void onToolSelected(int index);
    void onSplitterReady();

private:
    struct Private;
    Private* d;
};
}  // namespace ed

#endif  // ED_DOCKMENU_MANAGER_H