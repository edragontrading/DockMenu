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

#include <QColor>
#include <QIcon>

namespace ed {

class ED_EXPORT EProvider {
public:
    static EProvider& instance();

    ~EProvider();

    /**
     * The function returns a custom icon if one is registered and a null Icon
     * if no custom icon is registered
     */
    QIcon customIcon(eIcon IconId) const;

    /**
     * Registers a custom icon for the given IconId
     */
    void registerCustomIcon(eIcon IconId, const QIcon& icon);

    /**
     * The function returns a custom color if one is registered and a null Icon
     * if no custom color is registered
     */
    QColor customColor(eColor ColorId) const;

    /**
     * Registers a custom color for the given ColorId
     */
    void registerCustomColor(eColor ColorId, const QColor& color);

private:
    explicit EProvider();
    inline static EProvider* m_instance = nullptr;

    struct Private;
    Private* d;
};
}  // namespace ed

#endif  // ED_DOCKMENU_MOUSE_TRACKER_H
