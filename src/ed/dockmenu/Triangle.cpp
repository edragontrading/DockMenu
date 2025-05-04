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

#include "ed/dockmenu/Triangle.h"

#include <QPainter>
#include <QPolygon>

namespace ed {

struct ETriangle::Private {
    Private() = default;

    Type m_type;
    QColor m_color;
};

ETriangle::ETriangle(ETriangle::Type type, QSize size, QWidget* parent) : QWidget(parent), d(new Private) {
    d->m_type = type;
    setFixedSize(size);  // Adjust for triangle size

    d->m_color = internal::getCustomColor(eColor::TooltipArrowColor);
    if (!d->m_color.isValid()) {
        d->m_color = QColor(0x3498db);  // Default color
    }
}

ETriangle::~ETriangle() {
    delete d;
}

void ETriangle::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    QPolygon triangle;

    switch (d->m_type) {
        case Left:
            triangle << QPoint(width(), 0) << QPoint(width(), height()) << QPoint(0, height() / 2);
            break;
        case Right:
            triangle << QPoint(0, 0) << QPoint(0, height()) << QPoint(width(), height() / 2);
            break;
        case Top:
            triangle << QPoint(0, 0) << QPoint(width() / 2, height()) << QPoint(width(), 0);
            break;
        case Bottom:
            triangle << QPoint(0, height()) << QPoint(width() / 2, 0) << QPoint(width(), height());
            break;
        default:
            break;
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(d->m_color);
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(triangle);
    painter.end();
}

QColor ETriangle::iconColor() const {
    return d->m_color;
}

void ETriangle::setIconColor(const QColor& Color) {
    d->m_color = Color;
}

}  // namespace ed
