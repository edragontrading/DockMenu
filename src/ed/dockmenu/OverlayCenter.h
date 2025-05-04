#ifndef ED_DOCKMENU_OVERLAY_CENTER_H
#define ED_DOCKMENU_OVERLAY_CENTER_H

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

#include <QLabel>

#include "ed/dockmenu/ed_menu_globals.h"

namespace ed {
/*!
* You can style the center icon using the property system.
* \code
* ed--EOverlayCenter
{
            qproperty-iconFrameColor: palette(highlight);
            qproperty-iconBackgroundColor: palette(base);
            qproperty-iconOverlayColor: palette(highlight);
            qproperty-iconShadowColor: rgb(0, 0, 0);
    }
* \endcode
* Or you can use the iconColors property to pass in AARRGGBB values as
* hex string like shown in the example below.
* \code
* ed--EOverlayCenter
* {
*     qproperty-iconColors: "Frame=#ff3d3d3d Background=#ff929292 Overlay=#1f3d3d3d Arrow=#ffb4b4b4 Shadow=#40474747";
* }
* \endcode
*/

class EMenuOverlay;

class ED_EXPORT EOverlayCenter : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString iconColors READ iconColors WRITE setIconColors)
    Q_PROPERTY(QColor iconFrameColor READ iconColor WRITE setIconFrameColor)
    Q_PROPERTY(QColor iconBackgroundColor READ iconColor WRITE setIconBackgroundColor)
    Q_PROPERTY(QColor iconOverlayColor READ iconColor WRITE setIconOverlayColor)
    Q_PROPERTY(QColor iconShadowColor READ iconColor WRITE setIconShadowColor)

public:
    enum eIconColor {
        FrameColor,             ///< the color of the frame of the small window icon
        WindowBackgroundColor,  ///< the background color of the small window in the icon
        OverlayColor,           ///< the color that shows the overlay (the dock side) in the icon
        ShadowColor             ///< the color of the shadow rectangle that is painted below the icons
    };

private:
    struct Private;
    Private* d;

protected:
    /**
     * This function returns an empty string and is only here to silence
     * moc
     */
    QString iconColors() const;

    /**
     * This is a dummy function for the property system
     */
    QColor iconColor() const {
        return QColor();
    }

    void setIconFrameColor(const QColor& Color) {
        setIconColor(FrameColor, Color);
    }

    void setIconBackgroundColor(const QColor& Color) {
        setIconColor(WindowBackgroundColor, Color);
    }

    void setIconOverlayColor(const QColor& Color) {
        setIconColor(OverlayColor, Color);
    }

    void setIconShadowColor(const QColor& Color) {
        setIconColor(ShadowColor, Color);
    }

public:
    /**
     * Creates an overlay cross for the given overlay
     */
    EOverlayCenter(EMenuOverlay* overlay);

    /**
     * Virtual destructor
     */
    virtual ~EOverlayCenter();

    /**
     * Sets a certain icon color
     */
    void setIconColor(eIconColor ColorIndex, const QColor& Color);

    /**
     * Returns the icon color given by ColorIndex
     */
    QColor iconColor(eIconColor ColorIndex) const;

    /**
     * Returns the dock widget area depending on the current cursor location.
     * The function checks, if the mouse cursor is inside of any drop indicator
     * widget and returns the corresponding DockWidgetArea.
     */
    MenuWidgetArea cursorLocation() const;

    /**
     * Sets up the overlay cross for the given overlay mode
     */
    void setupOverlayCenter();

    /**
     * Recreates the overlay icons.
     */
    void updateOverlayIcons();

    /**
     * Resets and updates the overlay
     */
    void reset();

    /**
     * Updates the current position
     */
    void updatePosition();

    /**
     * A string with all icon colors to set.
     * You can use this property to style the overly icon via CSS stylesheet
     * file. The colors are set via a color identifier and a hex AARRGGBB value like
     * in the example below.
     * \code
     * ed--EOverlayCenter
     * {
     *     qproperty-iconColors: "Frame=#ff3d3d3d Background=#ff929292 Overlay=#1f3d3d3d Arrow=#ffb4b4b4
     *                  Shadow=#40474747";
     * }
     */
    void setIconColors(const QString& Colors);

protected:
    virtual void showEvent(QShowEvent* e) override;
    void setAreaWidgets(const QHash<MenuWidgetArea, QWidget*>& widgets);

private:
    int areaAlignment(const MenuWidgetArea area);
    QPoint areaGridPosition(const MenuWidgetArea area);
    qreal dropIndicatiorWidth(QLabel* l) const;
    QWidget* createDropIndicatorWidget(MenuWidgetArea widgetArea);
    void updateDropIndicatorIcon(QWidget* DropIndicatorWidget);
    QPixmap createHighDpiDropIndicatorPixmap(const QSizeF& size, MenuWidgetArea widgetArea);
    QColor defaultIconColor(eIconColor ColorIndex) const;
};
}  // namespace ed

#endif  // ED_DOCKMENU_OVERLAY_CENTER_H