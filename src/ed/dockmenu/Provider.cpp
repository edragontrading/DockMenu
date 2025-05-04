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

#include "ed/dockmenu/Provider.h"

#include <QMap>

namespace ed {

struct EProvider::Private {
    Private() = default;

    QMap<eIcon, QIcon> customIcons;
    QMap<eColor, QColor> customColors;
};

EProvider::EProvider() : d(new Private) {
}

EProvider::~EProvider() {
    delete d;
}

EProvider& EProvider::instance() {
    if (!m_instance) {
        m_instance = new EProvider();
    }
    return *m_instance;
}

QIcon EProvider::customIcon(eIcon IconId) const {
    if (!d->customIcons.contains(IconId)) {
        return QIcon();
    }
    return d->customIcons[IconId];
}

void EProvider::registerCustomIcon(eIcon IconId, const QIcon& icon) {
    d->customIcons[IconId] = icon;
}

QColor EProvider::customColor(eColor ColorId) const {
    if (!d->customColors.contains(ColorId)) {
        return QColor();
    }
    return d->customColors[ColorId];
}

void EProvider::registerCustomColor(eColor ColorId, const QColor& color) {
    d->customColors[ColorId] = color;
}

}  // namespace ed
