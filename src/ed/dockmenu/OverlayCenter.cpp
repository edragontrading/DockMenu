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

#include "ed/dockmenu/OverlayCenter.h"

#include <QGridLayout>
#include <QPainter>
#include <QWindow>

#include "ed/dockmenu/MenuOverlay.h"

namespace ed {

struct EOverlayCenter::Private {
    QColor IconColors[4];
    bool UpdateRequired = false;
    double LastDevicePixelRatio = 0.1;

    QGridLayout* GridLayout;
    EMenuOverlay* MenuOverlay;
    QHash<MenuWidgetArea, QWidget*> DropIndicatorWidgets;
};

EOverlayCenter::EOverlayCenter(EMenuOverlay* overlay) : QWidget(overlay->parentWidget()), d(new Private()) {
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#else
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
#endif
    setWindowTitle("MenuOverlayCenter");
    setAttribute(Qt::WA_TranslucentBackground);

    d->MenuOverlay = overlay;
    d->GridLayout = new QGridLayout();
    d->GridLayout->setSpacing(0);
    setLayout(d->GridLayout);
}

EOverlayCenter::~EOverlayCenter() {
    delete d;
}

void EOverlayCenter::setupOverlayCenter() {
    QHash<MenuWidgetArea, QWidget*> areaWidgets;
    areaWidgets.insert(CenterMenuWidgetArea, this->createDropIndicatorWidget(CenterMenuWidgetArea));

#if QT_VERSION >= 0x050600
    d->LastDevicePixelRatio = devicePixelRatioF();
#else
    d->LastDevicePixelRatio = devicePixelRatio();
#endif
    setAreaWidgets(areaWidgets);
    d->UpdateRequired = false;
}

void EOverlayCenter::updateOverlayIcons() {
    if (windowHandle()->devicePixelRatio() == d->LastDevicePixelRatio) {
        return;
    }

    for (auto Widget : d->DropIndicatorWidgets) {
        this->updateDropIndicatorIcon(Widget);
    }

#if QT_VERSION >= 0x050600
    d->LastDevicePixelRatio = devicePixelRatioF();
#else
    d->LastDevicePixelRatio = devicePixelRatio();
#endif
}

void EOverlayCenter::setIconColor(eIconColor ColorIndex, const QColor& Color) {
    d->IconColors[ColorIndex] = Color;
    d->UpdateRequired = true;
}

/**
 * Palette based default icon colors
 */
QColor EOverlayCenter::defaultIconColor(EOverlayCenter::eIconColor ColorIndex) const {
    QPalette pal = this->palette();
    switch (ColorIndex) {
        case EOverlayCenter::FrameColor:
            return pal.color(QPalette::Active, QPalette::Highlight);

        case EOverlayCenter::WindowBackgroundColor:
            return pal.color(QPalette::Active, QPalette::Base);

        case EOverlayCenter::OverlayColor: {
            QColor Color = pal.color(QPalette::Active, QPalette::Highlight);
            Color.setAlpha(64);
            return Color;
        }

        case EOverlayCenter::ShadowColor:
            return QColor(0, 0, 0, 64);
        default:
            return QColor();
    }

    return QColor();
}

QColor EOverlayCenter::iconColor(EOverlayCenter::eIconColor ColorIndex) const {
    QColor Color = d->IconColors[ColorIndex];
    if (!Color.isValid()) {
        Color = defaultIconColor(ColorIndex);
        d->IconColors[ColorIndex] = Color;
    }
    return Color;
}

void EOverlayCenter::setAreaWidgets(const QHash<MenuWidgetArea, QWidget*>& widgets) {
    // Delete old widgets.
    QMutableHashIterator<MenuWidgetArea, QWidget*> i(d->DropIndicatorWidgets);
    while (i.hasNext()) {
        i.next();
        QWidget* widget = i.value();
        d->GridLayout->removeWidget(widget);
        delete widget;
        i.remove();
    }

    // Insert new widgets into grid.
    d->DropIndicatorWidgets = widgets;
    QHashIterator<MenuWidgetArea, QWidget*> i2(d->DropIndicatorWidgets);
    while (i2.hasNext()) {
        i2.next();
        const MenuWidgetArea area = i2.key();
        QWidget* widget = i2.value();
        QPoint p = this->areaGridPosition(area);
        d->GridLayout->addWidget(widget, p.x(), p.y(), (Qt::Alignment)areaAlignment(area));
    }

    d->GridLayout->setContentsMargins(0, 0, 0, 0);
    d->GridLayout->setRowStretch(0, 1);
    d->GridLayout->setRowStretch(1, 0);
    d->GridLayout->setRowStretch(2, 0);
    d->GridLayout->setRowStretch(3, 0);
    d->GridLayout->setRowStretch(4, 1);

    d->GridLayout->setColumnStretch(0, 1);
    d->GridLayout->setColumnStretch(1, 0);
    d->GridLayout->setColumnStretch(2, 0);
    d->GridLayout->setColumnStretch(3, 0);
    d->GridLayout->setColumnStretch(4, 1);

    reset();
}

MenuWidgetArea EOverlayCenter::cursorLocation() const {
    const QPoint pos = mapFromGlobal(QCursor::pos());
    QHashIterator<MenuWidgetArea, QWidget*> i(d->DropIndicatorWidgets);

    while (i.hasNext()) {
        i.next();
        if (i.value() && i.value()->isVisible() && i.value()->geometry().contains(pos)) {
            return i.key();
        }
    }
    return InvalidMenuWidgetArea;
}

void EOverlayCenter::showEvent(QShowEvent*) {
    if (d->UpdateRequired) {
        setupOverlayCenter();
    }
    this->updatePosition();
}

void EOverlayCenter::updatePosition() {
    resize(d->MenuOverlay->size());
    QPoint TopLeft = d->MenuOverlay->pos();

    QPoint Offest((this->width() - d->MenuOverlay->width()) / 2, (this->height() - d->MenuOverlay->height()) / 2);
    QPoint CrossTopLeft = TopLeft - Offest;

    move(CrossTopLeft);
}

void EOverlayCenter::reset() {
    QList<MenuWidgetArea> allAreas;
    allAreas << CenterMenuWidgetArea;

    // Update visibility of area widgets based on allowedAreas.
    for (int i = 0; i < allAreas.count(); ++i) {
        QPoint p = this->areaGridPosition(allAreas.at(i));

        QLayoutItem* item = d->GridLayout->itemAtPosition(p.x(), p.y());
        QWidget* w = nullptr;

        if (item && (w = item->widget()) != nullptr) {
            w->setVisible(true);
        }
    }
}

void EOverlayCenter::setIconColors(const QString& Colors) {
    static const QMap<QString, int> ColorCompenentStringMap{{"Frame", EOverlayCenter::FrameColor},
                                                            {"Background", EOverlayCenter::WindowBackgroundColor},
                                                            {"Overlay", EOverlayCenter::OverlayColor},
                                                            {"Shadow", EOverlayCenter::ShadowColor}};

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    auto SkipEmptyParts = QString::SkipEmptyParts;
#else
    auto SkipEmptyParts = Qt::SkipEmptyParts;
#endif

    auto ColorList = Colors.split(' ', SkipEmptyParts);
    for (const auto& ColorListEntry : ColorList) {
        auto ComponentColor = ColorListEntry.split('=', SkipEmptyParts);
        int Component = ColorCompenentStringMap.value(ComponentColor[0], -1);
        if (Component < 0) {
            continue;
        }
        d->IconColors[Component] = QColor(ComponentColor[1]);
    }

    d->UpdateRequired = true;
}

QString EOverlayCenter::iconColors() const {
    return QString();
}

/**
 * Helper function that returns the drop indicator width depending on the
 * operating system
 */
qreal EOverlayCenter::dropIndicatiorWidth(QLabel* l) const {
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    Q_UNUSED(l)
    return 45;
#else
    return static_cast<qreal>(l->fontMetrics().height()) * 3.f;
#endif
}

QWidget* EOverlayCenter::createDropIndicatorWidget(MenuWidgetArea widgetArea) {
    QLabel* l = new QLabel();
    l->setObjectName("menuWidgetAreaLabel");

    qreal metric = dropIndicatiorWidth(l);
    QSizeF size(metric, metric);

    l->setPixmap(createHighDpiDropIndicatorPixmap(size, widgetArea));
    l->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    l->setAttribute(Qt::WA_TranslucentBackground);
    l->setProperty("menuWidgetArea", widgetArea);
    return l;
}

int EOverlayCenter::areaAlignment(const MenuWidgetArea area) {
    switch (area) {
        case CenterMenuWidgetArea:
            return (int)Qt::AlignCenter;
        default:
            return Qt::AlignCenter;
    }
}

QPoint EOverlayCenter::areaGridPosition(const MenuWidgetArea area) {
    if (area == CenterMenuWidgetArea) {
        return QPoint(2, 2);
    }

    return QPoint();
}

void EOverlayCenter::updateDropIndicatorIcon(QWidget* DropIndicatorWidget) {
    QLabel* l = qobject_cast<QLabel*>(DropIndicatorWidget);
    const qreal metric = dropIndicatiorWidth(l);
    const QSizeF size(metric, metric);

    int Area = l->property("menuWidgetArea").toInt();
    l->setPixmap(createHighDpiDropIndicatorPixmap(size, (MenuWidgetArea)Area));
}

QPixmap EOverlayCenter::createHighDpiDropIndicatorPixmap(const QSizeF& size, MenuWidgetArea widgetArea) {
    QColor borderColor = iconColor(EOverlayCenter::FrameColor);
    QColor backgroundColor = iconColor(EOverlayCenter::WindowBackgroundColor);
    QColor overlayColor = iconColor(EOverlayCenter::OverlayColor);

    if (overlayColor.alpha() == 255) {
        overlayColor.setAlpha(64);
    }

#if QT_VERSION >= 0x050600
    double DevicePixelRatio = this->window()->devicePixelRatioF();
#else
    double DevicePixelRatio = this->window()->devicePixelRatio();
#endif

    QSizeF PixmapSize = size * DevicePixelRatio;
    QPixmap pm(PixmapSize.toSize());
    pm.fill(QColor(0, 0, 0, 0));

    QPainter p(&pm);
    QPen pen = p.pen();
    QRectF ShadowRect(pm.rect());
    QRectF baseRect;
    baseRect.setSize(ShadowRect.size() * 0.7);
    baseRect.moveCenter(ShadowRect.center());

    // Fill
    QColor ShadowColor = iconColor(EOverlayCenter::ShadowColor);
    if (ShadowColor.alpha() == 255) {
        ShadowColor.setAlpha(64);
    }
    p.fillRect(ShadowRect, ShadowColor);

    // Drop area rect
    p.save();
    QRectF areaRect;
    QLineF areaLine;
    QRectF nonAreaRect;
    switch (d->MenuOverlay->direction()) {
        case MenuDirection::Top:
            areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
            nonAreaRect = QRectF(baseRect.x(), ShadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
            areaLine = QLineF(areaRect.bottomLeft(), areaRect.bottomRight());
            break;
        case MenuDirection::Right:
            areaRect = QRectF(ShadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
            nonAreaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
            areaLine = QLineF(areaRect.topLeft(), areaRect.bottomLeft());
            break;
        case MenuDirection::Bottom:
            areaRect = QRectF(baseRect.x(), ShadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
            nonAreaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
            areaLine = QLineF(areaRect.topLeft(), areaRect.topRight());
            break;
        case MenuDirection::Left:
            areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
            nonAreaRect = QRectF(ShadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
            areaLine = QLineF(areaRect.topRight(), areaRect.bottomRight());
            break;
        default:
            break;
    }

    QSizeF baseSize = baseRect.size();
    p.fillRect(baseRect, backgroundColor);

    if (areaRect.isValid()) {
        pen = p.pen();
        pen.setColor(borderColor);
        p.setBrush(overlayColor);
        p.setPen(Qt::NoPen);
        p.drawRect(areaRect);

        pen = p.pen();
        pen.setWidth(1);
        pen.setColor(borderColor);
        pen.setStyle(Qt::DashLine);
        p.setPen(pen);
        p.drawLine(areaLine);
    }
    p.restore();

    // Draw outer border
    p.save();
    pen = p.pen();
    pen.setColor(borderColor);
    pen.setWidth(1);
    p.setBrush(Qt::NoBrush);
    p.setPen(pen);
    p.drawRect(baseRect);

    // draw window title bar
    p.setBrush(borderColor);
    QRectF FrameRect(baseRect.topLeft(), QSizeF(baseRect.width(), baseSize.height() / 10));
    p.drawRect(FrameRect);
    p.restore();

    pm.setDevicePixelRatio(DevicePixelRatio);
    return pm;
}

}  // namespace ed