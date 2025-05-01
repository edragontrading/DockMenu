#ifndef ED_DOCKMENU_TOOLTIP_H
#define ED_DOCKMENU_TOOLTIP_H

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

#include "ed/dockmenu/Triangle.h"
#include "ed/dockmenu/ed_menu_globals.h"

namespace ed {

class ED_EXPORT ETooltip : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor iconArrowColor READ iconArrowColor WRITE setIconArrowColor)

private:
    struct Private;
    Private* d;

public:
    explicit ETooltip(MenuDirection direct, QWidget* parent = nullptr);
    ~ETooltip() override;

    // Show the tooltip at a global screen position
    void showText(const QPoint& globalPos, const QString& text);
    void showTooltip(const QPoint& globalPos);

    void setToolTipText(const QString& text);

protected:
    QColor iconArrowColor() const;
    void setIconArrowColor(const QColor& Color);

private:
    ETriangle::Type getDirection(MenuDirection direct) const;
};
}  // namespace ed

#endif  // ED_DOCKMENU_TOOLTIP_H
