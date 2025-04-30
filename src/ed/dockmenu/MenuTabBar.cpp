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

#include "ed/dockmenu/MenuTabBar.h"

#include <QBoxLayout>

#include "ed/dockmenu/MenuButton.h"

namespace ed {

struct EMenuTabBar::Private {
    Private() = default;

    MenuDirection direction;
    QBoxLayout* layout;
};

EMenuTabBar::EMenuTabBar(MenuDirection direction, QWidget* parent) : QScrollArea(parent), d(new Private) {
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setObjectName("EMenuTabBar");

    d->direction = direction;

    if (direction == MenuDirection::Left || direction == MenuDirection::Right) {
        setFixedWidth(50);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        d->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
        d->layout->setAlignment(Qt::AlignTop | Qt::AlignVCenter);
    } else {
        setFixedHeight(50);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        d->layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
        d->layout->setAlignment(Qt::AlignLeft | Qt::AlignHCenter);
    }

    d->layout->setSpacing(2);
    d->layout->setContentsMargins(0, 0, 0, 0);
}

EMenuTabBar::~EMenuTabBar() {
    ED_PRINT("EMenuTabBar::~EMenuTabBar");
    delete d;
}

void EMenuTabBar::setSize(int size) {
    if (d->direction == MenuDirection::Left || d->direction == MenuDirection::Right) {
        setFixedWidth(size);
    } else {
        setFixedHeight(size);
    }
}

void EMenuTabBar::setClose() {
    for (int i = 0; i < d->layout->count(); ++i) {
        EMenuButton* btn = qobject_cast<EMenuButton*>(d->layout->itemAt(i)->widget());
        if (btn && btn->isChecked()) {
            btn->setChecked(false);
            btn->setUpdated();
        }
    }
}

void EMenuTabBar::setSelected(int index) {
    if (index < 0 || index >= d->layout->count()) {
        return;
    }

    EMenuButton* btn = qobject_cast<EMenuButton*>(d->layout->itemAt(index)->widget());
    if (btn) {
        btn->setChecked(true);
        btn->setUpdated();
    }
}

int EMenuTabBar::getSelected() const {
    for (int i = 0; i < d->layout->count(); ++i) {
        EMenuButton* btn = qobject_cast<EMenuButton*>(d->layout->itemAt(i)->widget());
        if (btn && btn->isChecked()) {
            return i;
        }
    }

    return -1;
}

void EMenuTabBar::addMenuButton(EMenuButton* button) {
    d->layout->addWidget(button);
    connect(button, &EMenuButton::clicked, this, [this, button]() {
        for (int i = 0; i < d->layout->count(); ++i) {
            EMenuButton* btn = qobject_cast<EMenuButton*>(d->layout->itemAt(i)->widget());
            if (!btn) {
                continue;
            }

            if (btn == button) {
                if (btn->isChecked()) {
                    btn->setUpdated();
                    Q_EMIT toolSelected(i);
                } else {
                    btn->setUpdated();
                    Q_EMIT toolClosed();
                }
            } else {
                btn->setChecked(false);
                btn->setUpdated();
            }
        }
    });
}

}  // namespace ed
