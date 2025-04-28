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

#include "ed/dockmenu/MenuButton.h"

#include <QHelpEvent>
#include <QPainter>

#include "ed/dockmenu/Tooltip.h"

namespace ed {

struct EMenuButton::Private {
    Private() = default;
    ETooltip* tooltip;
    QIcon normalIcon;
    QIcon checkedIcon;
    MenuDirection direction;
    bool active;
    bool updated;
    QColor colorHighlight;
};

EMenuButton::EMenuButton(MenuDirection direction, const QSize& iconSize, const QString& normalIcon,
                         const QString& checkedIcon, const QString& tooltipText, QWidget* parent)
    : QToolButton(parent), d(new Private) {
    setCheckable(true);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setAutoRaise(true);

    setObjectName("EMenuButton");

    d->normalIcon = QIcon(normalIcon);
    d->checkedIcon = QIcon(checkedIcon);

    setIcon(QIcon(d->normalIcon));
    setIconSize(iconSize);

    if (direction == MenuDirection::Left || direction == MenuDirection::Right) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    } else {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    }

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    d->tooltip = new ETooltip(direction, this);
    d->tooltip->setToolTipText(tooltipText);
    d->direction = direction;
    d->active = false;
    d->updated = false;
    d->colorHighlight = internal::getCustomColor(eColor::MenuButtonHighlight);
}

EMenuButton::~EMenuButton() {
    delete d;
}

void EMenuButton::setUpdated() {
    d->updated = true;
    update();
}

void EMenuButton::paintEvent(QPaintEvent* event) {
    QToolButton::paintEvent(event);

    if (d->updated) {
        setIcon(isChecked() ? d->checkedIcon : d->normalIcon);
        d->updated = false;
    }

    if (isChecked() && d->colorHighlight.isValid()) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(d->colorHighlight);

        QRect lineRect;
        switch (d->direction) {
            case MenuDirection::Left:
                lineRect = QRect(0, 0, 3, height());  // 4px wide on left
                break;
            case MenuDirection::Right:
                lineRect = QRect(width() - 3, 0, 3, height());  // 4px wide on right
                break;
            case MenuDirection::Top:
                lineRect = QRect(0, 0, width(), 3);  // 4px high on top
                break;
            case MenuDirection::Bottom:
                lineRect = QRect(0, height() - 3, width(), 3);  // 4px high on bottom
                break;
            default:
                break;
        }
        painter.drawRect(lineRect);
    }
}

void EMenuButton::enterEvent(QEnterEvent* event) {
    QToolButton::enterEvent(event);
    QPoint globalPos = mapToGlobal(QPoint(width() + 3, (height() - d->tooltip->height()) / 2));
    d->tooltip->showTooltip(globalPos);
    if (!isChecked()) {
        setIcon(d->checkedIcon);
    }
}

void EMenuButton::leaveEvent(QEvent* event) {
    QToolButton::leaveEvent(event);
    d->tooltip->hide();
    if (!isChecked()) {
        setIcon(d->normalIcon);
    }
}
}  // namespace ed
