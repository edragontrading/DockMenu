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

#include "ed/dockmenu/MenuWidget.h"

#include <QBoxLayout>

#include "ed/dockmenu/MenuTitleBar.h"

namespace ed {
struct EMenuWidget::Private {
    Private() = default;

    QBoxLayout* layout;
    EMenuTitleBar* titleBar;
};

EMenuWidget::EMenuWidget(EMenuManager* manager, const QString& name, QWidget* widget, QWidget* parent)
    : QScrollArea(parent), d(new Private()) {
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFocusPolicy(Qt::NoFocus);

    d->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);

    d->titleBar = new EMenuTitleBar(manager, name, this);

    d->layout->addWidget(d->titleBar);
    d->layout->addWidget(widget, 1);
}

void EMenuWidget::updateState(bool floating) {
    d->titleBar->updateState(floating);
}

EMenuWidget::~EMenuWidget() {
    ED_PRINT("EMenuWidget::~EMenuWidget");
    delete d;
}

}  // namespace ed
