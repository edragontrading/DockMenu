#ifndef ED_DOCKMENU_GLOBALS_H
#define ED_DOCKMENU_GLOBALS_H

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

#include <QAbstractButton>
#include <QDebug>
#include <QObject>
#include <QStyle>

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
#include <xcb/xcb.h>
#endif

#ifndef ED_DOCKMENU_STATIC
#ifdef ED_DOCKMENU_SHARED_EXPORT
#define ED_EXPORT Q_DECL_EXPORT
#else
#define ED_EXPORT Q_DECL_IMPORT
#endif
#else
#define ED_EXPORT
#endif

#define ED_DEBUG_PRINT
#ifdef ED_DEBUG_PRINT
#define ED_PRINT(s) qDebug() << s
#else
#define ED_PRINT(s)
#endif

namespace ed {
Q_NAMESPACE

enum MenuDirection {
    NoDirection = 0x00,
    Left = 0x01,
    Right = 0x02,
    Top = 0x04,
    Bottom = 0x08,
    Center = 0x10,

    InvalidDirection = NoDirection
};
Q_ENUM_NS(MenuDirection);

/**
 * The different icons used in the UI
 */
enum eIcon {
    MenuUndockIcon,  //!< MenuUndockIcon
    MenuDockIcon,    //!< MenuDockIcon

    IconCount,  //!< just a delimiter for range checks
};
Q_ENUM_NS(eIcon);

enum eColor {
    MenuButtonHighlight,   //!< MenuBackgroundColor
    TooltipArrowColor,     //!< MenuBorderColor
    QSplitterHandleColor,  //!< MenuSplitterHandleColor

    ColorCount,  //!< just a delimiter for range checks
};
Q_ENUM_NS(eColor);

/**
 * The different dragging states
 */
enum eDragState {
    DraggingInactive,       //!< DraggingInactive
    DraggingMousePressed,   //!< DraggingMousePressed
    DraggingFloatingWidget  //!< DraggingFloatingWidget
};

namespace internal {
/**
 * Helper function for settings tooltips without cluttering the code with
 * tests for preprocessor macros
 */
template <class QObjectPtr>
void setToolTip(QObjectPtr obj, const QString& tip) {
#ifndef QT_NO_TOOLTIP
    obj->setToolTip(tip);
#else
    Q_UNUSED(obj);
    Q_UNUSED(tip);
#endif
}

/**
 * Creates a semi transparent pixmap from the given pixmap Source.
 * The Opacity parameter defines the opacity from completely transparent (0.0)
 * to completely opaque (1.0)
 */
QPixmap createTransparentPixmap(const QPixmap& Source, qreal Opacity);

/**
 * Helper function to set the icon of a certain button.
 * Use this function to set the icons for the dock area and dock widget buttons.
 * The function first uses the CustomIconId to get an icon from the
 * CIconProvider. You can register your custom icons with the icon provider, if
 * you do not want to use the default buttons and if you do not want to use
 * stylesheets.
 * If the IconProvider does not return a valid icon (icon is null), the function
 * fetches the given standard pixmap from the QStyle.
 * param[in] Button The button whose icons are to be set
 * param[in] StandardPixmap The standard pixmap to be used for the button
 * param[in] CustomIconId The identifier for the custom icon.
 */
void setButtonIcon(QAbstractButton* Button, QStyle::StandardPixmap StandarPixmap, ed::eIcon CustomIconId);

/**
 * Helper function to get a custom color from the icon provider.
 * The function returns a null color if no custom color is registered.
 */
QColor getCustomColor(eColor ColorId);

}  // namespace internal
}  // namespace ed

#endif