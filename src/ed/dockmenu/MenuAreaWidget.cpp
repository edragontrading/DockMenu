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

#include "ed/dockmenu/MenuAreaWidget.h"

#include <QHBoxLayout>

#include "ed/dockmenu/MenuWidget.h"
#include "ed/dockmenu/SlidingStacked.h"

namespace ed {
struct EMenuAreaWidget::Private {
    Private() = default;

    ESlidingStacked* stackedWidget;
    MenuDirection direction;
    QHBoxLayout* layout;
    int currentIndex;
};

EMenuAreaWidget::EMenuAreaWidget(MenuDirection direction, QWidget* parent) : QWidget(parent), d(new Private) {
    setObjectName("EMenuAreaWidget");

    d->stackedWidget = new ESlidingStacked(this);
    d->stackedWidget->setVerticalMode(direction == MenuDirection::Top || direction == MenuDirection::Bottom);
    d->stackedWidget->setWrap(true);
    d->stackedWidget->setAnimation(QEasingCurve::InOutExpo);
    d->stackedWidget->setSpeed(600);

    d->layout = new QHBoxLayout(this);
    d->layout->setContentsMargins(QMargins(0, 0, 0, 0));
    d->layout->setSpacing(0);
    d->layout->addWidget(d->stackedWidget);

    setLayout(d->layout);

    d->direction = direction;
    d->currentIndex = -1;

    connect(d->stackedWidget, &ESlidingStacked::animationFinished, this, [this]() {
        if (d->currentIndex != -1) {
            d->stackedWidget->setCurrentIndex(d->currentIndex);
        }
    });
}

EMenuAreaWidget::~EMenuAreaWidget() {
    ED_PRINT("EMenuAreaWidget::~EMenuAreaWidget");
    delete d;
}

void EMenuAreaWidget::addMenuWidget(EMenuWidget* widget) {
    d->stackedWidget->addWidget(widget);
}

void EMenuAreaWidget::toolSelected(int index) {
    d->currentIndex = index;
    d->stackedWidget->slideInIdx(index, ESlidingStacked::LEFT_TO_RIGHT);
}

int EMenuAreaWidget::getCurrentIndex() const {
    return d->currentIndex;
}

void EMenuAreaWidget::updateState(bool floating) {
    for (int idx = 0; idx < d->stackedWidget->count(); ++idx) {
        EMenuWidget* widget = qobject_cast<EMenuWidget*>(d->stackedWidget->widget(idx));
        if (widget) {
            widget->updateState(floating);
        }
    }
}
}  // namespace ed
