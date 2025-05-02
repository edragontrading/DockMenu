#ifndef ED_DOCKMENU_MENU_OVERLAY_H
#define ED_DOCKMENU_MENU_OVERLAY_H

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

/*!
 * EMenuOverlay paints a translucent rectangle over another widget. The geometry
 * of the rectangle is based on the mouse location.
 */
class ED_EXPORT EMenuOverlay : public QFrame {
    Q_OBJECT
private:
    struct Private;
    Private* d;

public:
    using Super = QFrame;

    /**
     * Creates a dock overlay
     */
    EMenuOverlay(QWidget* parent);

    /**
     * Virtual destructor
     */
    virtual ~EMenuOverlay();

    /**
     * Return menu direction
     */
    MenuDirection direction() const;

    /**
     * Returns the drop area under the current cursor location
     */
    MenuWidgetArea dropAreaUnderCursor() const;

    /**
     * This function returns the same like dropAreaUnderCursor() if this
     * overlay is not hidden and if drop preview is enabled and returns
     * InvalidDockWidgetArea if it is hidden or drop preview is disabled.
     */
    MenuWidgetArea visibleDropAreaUnderCursor() const;

    /**
     * Show the drop overly for the given target widget
     */
    MenuWidgetArea showOverlay(QWidget* target);

    /**
     * Hides the overlay
     */
    void hideOverlay();

    /**
     * Enables / disables the semi transparent overlay rectangle that represents
     * the future area of the dropped widget
     */
    void enableDropPreview(bool Enable);

    /**
     * Returns true if drop preview is enabled
     */
    bool dropPreviewEnabled() const;

    /**
     * The drop overlay rectangle for the target area
     */
    QRect dropOverlayRect() const;

    /**
     * Handle polish events
     */
    virtual bool event(QEvent* e) override;

protected:
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void showEvent(QShowEvent* e) override;
    virtual void hideEvent(QHideEvent* e) override;
};
}  // namespace ed

#endif  //  ED_DOCKMENU_MENU_OVERLAY_H