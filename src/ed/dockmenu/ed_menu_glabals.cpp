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

#include <QPainter>

#include "ed/dockmenu/Provider.h"
#include "ed/dockmenu/ed_menu_globals.h"

namespace ed {
namespace internal {

QPixmap createPixmap(QWidget* wLeftTop, QWidget* wRightBottom, Qt::Orientation orient) {
    int totalWidth = 0;
    int totalHeight = 0;
    QPixmap pixmapLeftTop(wLeftTop->size());
    wLeftTop->render(&pixmapLeftTop);

    QPixmap pixmapRightBottom(wRightBottom->size());
    wRightBottom->render(&pixmapRightBottom);

    if (orient == Qt::Horizontal) {
        totalWidth = pixmapLeftTop.width() + pixmapRightBottom.width();
        totalHeight = qMax(pixmapLeftTop.height(), pixmapRightBottom.height());
    } else {
        totalWidth = qMax(pixmapLeftTop.width(), pixmapRightBottom.width());
        totalHeight = pixmapLeftTop.height() + pixmapRightBottom.height();
    }

    QPixmap combinedPixmap(totalWidth, totalHeight);
    combinedPixmap.fill(Qt::transparent);
    QPainter painter(&combinedPixmap);

    if (orient == Qt::Horizontal) {
        painter.drawPixmap(0, 0, pixmapLeftTop);
        painter.drawPixmap(pixmapLeftTop.width(), 0, pixmapRightBottom);
    } else {
        painter.drawPixmap(0, 0, pixmapLeftTop);
        painter.drawPixmap(0, pixmapLeftTop.height(), pixmapRightBottom);
    }

    return combinedPixmap;
}

QPixmap createTransparentPixmap(const QPixmap& Source, qreal Opacity) {
    QPixmap TransparentPixmap(Source.size());
    TransparentPixmap.fill(Qt::transparent);
    QPainter p(&TransparentPixmap);
    p.setOpacity(Opacity);
    p.drawPixmap(0, 0, Source);
    return TransparentPixmap;
}

void setButtonIcon(QAbstractButton* Button, QStyle::StandardPixmap StandarPixmap, ed::eIcon CustomIconId) {
    // First we try to use custom icons if available
    QIcon Icon = ed::EProvider::instance().customIcon(CustomIconId);
    if (!Icon.isNull()) {
        Button->setIcon(Icon);
        return;
    }

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    Button->setIcon(Button->style()->standardIcon(StandarPixmap));
#else
    // The standard icons does not look good on high DPI screens so we create
    // our own "standard" icon here.
    QPixmap normalPixmap = Button->style()->standardPixmap(StandarPixmap, 0, Button);
    Icon.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25), QIcon::Disabled);
    Icon.addPixmap(normalPixmap, QIcon::Normal);
    Button->setIcon(Icon);
#endif
}

QColor getCustomColor(eColor ColorId) {
    if (ColorId < 0 || ColorId >= ed::eColor::ColorCount) {
        return QColor();
    }
    return ed::EProvider::instance().customColor(ColorId);
}

}  // namespace internal
}  // namespace ed