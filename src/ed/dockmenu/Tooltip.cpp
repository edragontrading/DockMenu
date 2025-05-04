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

#include "ed/dockmenu/Tooltip.h"

#include <QColor>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QVBoxLayout>

#include "ed/dockmenu/Triangle.h"

namespace ed {

struct ETooltip::Private {
    Private() = default;
    QLabel* label;
    ETriangle* triangleWidget;
};

ETooltip::ETooltip(MenuDirection direct, QWidget* parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint), d(new Private) {
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);

    d->label = new QLabel(this);
    d->triangleWidget = new ETriangle(getDirection(direct), QSize(8, 24), this);

    d->label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->label->setWordWrap(true);
    d->label->setObjectName("ETooltip");

    auto layout = new QHBoxLayout(this);
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    if (getDirection(direct) == ETriangle::Left) {
        layout->addWidget(d->triangleWidget);
        layout->addWidget(d->label);
    } else {
        layout->addWidget(d->label);
        layout->addWidget(d->triangleWidget);
    }

    setLayout(layout);

    this->setMinimumHeight(30);
    this->setMinimumWidth(200);
}

ETooltip::~ETooltip() {
    delete d;
}

void ETooltip::setToolTipText(const QString& text) {
    d->label->setText(text);
}

void ETooltip::showText(const QPoint& globalPos, const QString& text) {
    d->label->setText(text);
    adjustSize();
    move(globalPos);
    show();
}

void ETooltip::showTooltip(const QPoint& globalPos) {
    adjustSize();
    move(globalPos);
    show();
}

ETriangle::Type ETooltip::getDirection(MenuDirection direct) const {
    switch (direct) {
        case MenuDirection::Left:
        case MenuDirection::Top:
        case MenuDirection::Bottom:
            return ETriangle::Left;

        case MenuDirection::Right:
            return ETriangle::Right;

        default:
            return ETriangle::Left;  // Default to left if invalid
    }
}

QColor ETooltip::iconArrowColor() const {
    return d->triangleWidget->iconColor();
}

void ETooltip::setIconArrowColor(const QColor& Color) {
    d->triangleWidget->setIconColor(Color);
}

}  // namespace ed