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

#include "ed/dockmenu/SlidingStacked.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

namespace ed {
struct ESlidingStacked::Private {
    Private() = default;

    int m_speed;
    bool m_vertical;
    int m_now;
    int m_next;
    bool m_wrap;
    bool m_active;
    QPoint m_pnow;

    QWidget *mainWindow;
    QList<QWidget *> blockedPageList;
    QParallelAnimationGroup *animGroup;
    QEasingCurve::Type animationType;
};

ESlidingStacked::ESlidingStacked(QWidget *parent) : QStackedWidget(parent), d(new Private) {
    if (parent != nullptr) {
        d->mainWindow = parent;
    } else {
        d->mainWindow = this;
    }

    d->m_vertical = false;
    d->m_speed = 300;
    d->animationType = QEasingCurve::OutQuart;
    d->m_now = 0;
    d->m_next = 0;
    d->m_wrap = false;
    d->m_pnow = QPoint(0, 0);
    d->m_active = false;
}

ESlidingStacked::~ESlidingStacked() {
    delete d;
}

void ESlidingStacked::setSpeed(int speed) {
    d->m_speed = speed;
}

void ESlidingStacked::setAnimation(enum QEasingCurve::Type animationtype) {
    d->animationType = animationtype;
}

void ESlidingStacked::setVerticalMode(bool vertical) {
    d->m_vertical = vertical;
}

void ESlidingStacked::setWrap(bool wrap) {
    d->m_wrap = wrap;
}

bool ESlidingStacked::slideInNext() {
    int now = currentIndex();
    if (d->m_wrap || (now < count() - 1))
        slideInIdx(now + 1);
    else
        return false;
    return true;
}

bool ESlidingStacked::slideInPrev() {
    int now = currentIndex();
    if (d->m_wrap || (now > 0))
        slideInIdx(now - 1);
    else
        return false;
    return true;
}

void ESlidingStacked::slideInIdx(int idx, SlidingDirection direction) {
    if (idx > count() - 1) {
        direction = d->m_vertical ? TOP_TO_BOTTOM : RIGHT_TO_LEFT;
        idx = (idx) % count();
    } else if (idx < 0) {
        direction = d->m_vertical ? BOTTOM_TO_TOP : LEFT_TO_RIGHT;
        idx = (idx + count()) % count();
    }
    slideInWgt(widget(idx), direction);
}

void ESlidingStacked::slideInWgt(QWidget *newWidget, SlidingDirection direction) {
    if (d->m_active) {
        return;
    } else {
        d->m_active = true;
    }

    SlidingDirection directionhint;
    int now = currentIndex();
    int next = indexOf(newWidget);
    if (now == next) {
        d->m_active = false;
        return;
    } else if (now < next) {
        directionhint = d->m_vertical ? TOP_TO_BOTTOM : RIGHT_TO_LEFT;
    } else {
        directionhint = d->m_vertical ? BOTTOM_TO_TOP : LEFT_TO_RIGHT;
    }

    if (direction == AUTOMATIC) {
        direction = directionhint;
    }

    int offsetx = frameRect().width();
    int offsety = frameRect().height();

    widget(next)->setGeometry(0, 0, offsetx, offsety);
    if (direction == BOTTOM_TO_TOP) {
        offsetx = 0;
        offsety = -offsety;
    } else if (direction == TOP_TO_BOTTOM) {
        offsetx = 0;
    } else if (direction == RIGHT_TO_LEFT) {
        offsetx = -offsetx;
        offsety = 0;
    } else if (direction == LEFT_TO_RIGHT) {
        offsety = 0;
    }

    QPoint pnext = widget(next)->pos();
    QPoint pnow = widget(now)->pos();
    d->m_pnow = pnow;
    widget(next)->move(pnext.x() - offsetx, pnext.y() - offsety);

    widget(next)->show();
    widget(next)->raise();

    QPropertyAnimation *animnow = new QPropertyAnimation(widget(now), "pos");
    animnow->setDuration(d->m_speed);
    animnow->setEasingCurve(d->animationType);
    animnow->setStartValue(QPoint(pnow.x(), pnow.y()));
    animnow->setEndValue(QPoint(offsetx + pnow.x(), offsety + pnow.y()));

    QGraphicsOpacityEffect *animnow_op_eff = new QGraphicsOpacityEffect();
    widget(now)->setGraphicsEffect(animnow_op_eff);
    QPropertyAnimation *animnow_op = new QPropertyAnimation(animnow_op_eff, "opacity");
    animnow_op->setDuration(d->m_speed / 2);
    animnow_op->setStartValue(1);
    animnow_op->setEndValue(0);
    connect(animnow_op, &QPropertyAnimation::finished, [=]() {
        if (animnow_op_eff != nullptr) animnow_op_eff->deleteLater();
    });

    QGraphicsOpacityEffect *animnext_op_eff = new QGraphicsOpacityEffect();
    animnext_op_eff->setOpacity(0);
    widget(next)->setGraphicsEffect(animnext_op_eff);
    QPropertyAnimation *animnext_op = new QPropertyAnimation(animnext_op_eff, "opacity");
    animnext_op->setDuration(d->m_speed / 2);
    animnext_op->setStartValue(0);
    animnext_op->setEndValue(1);
    connect(animnext_op, &QPropertyAnimation::finished, [=]() {
        if (animnext_op_eff != nullptr) animnext_op_eff->deleteLater();
    });

    QPropertyAnimation *animnext = new QPropertyAnimation(widget(next), "pos");
    animnext->setDuration(d->m_speed);
    animnext->setEasingCurve(d->animationType);
    animnext->setStartValue(QPoint(-offsetx + pnext.x(), offsety + pnext.y()));
    animnext->setEndValue(QPoint(pnext.x(), pnext.y()));

    d->animGroup = new QParallelAnimationGroup;
    d->animGroup->addAnimation(animnow);
    d->animGroup->addAnimation(animnext);
    d->animGroup->addAnimation(animnow_op);
    d->animGroup->addAnimation(animnext_op);

    QObject::connect(d->animGroup, SIGNAL(finished()), this, SLOT(animationDoneSlot()));
    d->m_next = next;
    d->m_now = now;
    d->m_active = true;
    d->animGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void ESlidingStacked::animationDoneSlot() {
    setCurrentIndex(d->m_next);
    widget(d->m_now)->hide();
    widget(d->m_now)->move(d->m_pnow);
    d->m_active = false;
    emit animationFinished();
}

}  // namespace ed