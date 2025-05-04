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

#include "ed/dockmenu/MenuOverlay.h"

#include <QPainter>
#include <QPointer>

#include "ed/dockmenu/MenuManager.h"
#include "ed/dockmenu/OverlayCenter.h"

namespace ed {

struct EMenuOverlay::Private {
    Private() = default;

    bool DropPreviewEnabled = true;

    QRect DropAreaRect;
    EOverlayCenter* Center;
    QPointer<QWidget> TargetWidget;
    MenuWidgetArea LastLocation = InvalidMenuWidgetArea;
};

EMenuOverlay::EMenuOverlay(QWidget* parent) : QFrame(parent), d(new Private()) {
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#else
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
#endif
    setWindowOpacity(1);
    setWindowTitle("DockOverlay");
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    d->Center = new EOverlayCenter(this);
    d->Center->setVisible(false);
    setVisible(false);
}

EMenuOverlay::~EMenuOverlay() {
    delete d;
}

MenuDirection EMenuOverlay::direction() const {
    return qobject_cast<EMenuManager*>(this->parent())->direction();
}

MenuWidgetArea EMenuOverlay::dropAreaUnderCursor() const {
    MenuWidgetArea Result = d->Center->cursorLocation();
    return Result;
}

MenuWidgetArea EMenuOverlay::visibleDropAreaUnderCursor() const {
    if (isHidden() || !d->DropPreviewEnabled) {
        return InvalidMenuWidgetArea;
    } else {
        return dropAreaUnderCursor();
    }
}

MenuWidgetArea EMenuOverlay::showOverlay(QWidget* target) {
    if (d->TargetWidget == target) {
        // Hint: We could update geometry of overlay here.
        MenuWidgetArea da = dropAreaUnderCursor();
        if (da != d->LastLocation) {
            repaint();
            d->LastLocation = da;
        }
        return da;
    }

    d->TargetWidget = target;
    d->LastLocation = InvalidMenuWidgetArea;

    // Move it over the target.
    hide();
    resize(target->size());
    QPoint TopLeft = target->mapToGlobal(target->rect().topLeft());
    move(TopLeft);
    show();
    d->Center->updatePosition();
    d->Center->updateOverlayIcons();
    return dropAreaUnderCursor();
}

void EMenuOverlay::hideOverlay() {
    hide();
    d->TargetWidget.clear();
    d->LastLocation = InvalidMenuWidgetArea;
    d->DropAreaRect = QRect();
}

void EMenuOverlay::enableDropPreview(bool Enable) {
    d->DropPreviewEnabled = Enable;
    update();
}

bool EMenuOverlay::dropPreviewEnabled() const {
    return d->DropPreviewEnabled;
}

void EMenuOverlay::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    // Draw rect based on location
    if (!d->DropPreviewEnabled) {
        d->DropAreaRect = QRect();
        return;
    }

    QRect r = rect();
    double Factor = 4;

    switch (this->direction()) {
        case MenuDirection::Top:
            r.setHeight(r.height() / Factor);
            break;
        case MenuDirection::Right:
            r.setX(r.width() * (1 - 1 / Factor));
            break;
        case MenuDirection::Bottom:
            r.setY(r.height() * (1 - 1 / Factor));
            break;
        case MenuDirection::Left:
            r.setWidth(r.width() / Factor);
            break;
        default:
            return;
    }

    QPainter painter(this);
    QColor Color = palette().color(QPalette::Active, QPalette::Highlight);
    QPen Pen = painter.pen();
    Pen.setColor(Color.darker(120));
    Pen.setStyle(Qt::SolidLine);
    Pen.setWidth(1);
    Pen.setCosmetic(true);
    painter.setPen(Pen);
    Color = Color.lighter(130);
    Color.setAlpha(64);
    painter.setBrush(Color);
    painter.drawRect(r.adjusted(0, 0, -1, -1));
    d->DropAreaRect = r;
}

QRect EMenuOverlay::dropOverlayRect() const {
    return d->DropAreaRect;
}

void EMenuOverlay::showEvent(QShowEvent* e) {
    d->Center->show();
    QFrame::showEvent(e);
}

void EMenuOverlay::hideEvent(QHideEvent* e) {
    d->Center->hide();
    QFrame::hideEvent(e);
}

bool EMenuOverlay::event(QEvent* e) {
    bool Result = Super::event(e);
    if (e->type() == QEvent::Polish) {
        d->Center->setupOverlayCenter();
    }
    return Result;
}

}  // namespace ed