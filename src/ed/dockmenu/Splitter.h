#ifndef ED_DOCKMENU_SPLITTER_H_
#define ED_DOCKMENU_SPLITTER_H_

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

#include <QMouseEvent>
#include <QSplitter>
#include <QSplitterHandle>
#include <QTimer>

#include "ed/dockmenu/ed_menu_globals.h"

namespace ed {

class ESplitterHandle : public QSplitterHandle {
    Q_OBJECT
    Q_PROPERTY(QColor handleColor READ handleColor WRITE setHandleColor)

public:
    ESplitterHandle(Qt::Orientation orientation, QSplitter* parent);
    ~ESplitterHandle();
    QSize sizeHint() const override;

    bool setVisibleOnHover(bool visible);

    bool hovered();
    bool mousePressed();

Q_SIGNALS:
    void dragFinished();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

protected:
    QColor handleColor() const;
    void setHandleColor(const QColor& Color);

private:
    struct Private;
    Private* d;
};

class ESplitter : public QSplitter {
    Q_OBJECT

public:
    ESplitter(Qt::Orientation orientation, QWidget* parent = nullptr);
    ~ESplitter() override;

protected:
    QSplitterHandle* createHandle() override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void updateHandleVisibility(bool timerEvent, const QPoint& mousePos);
    void showEvent(QShowEvent* event) override;

Q_SIGNALS:
    void splitterReady();

private Q_SLOTS:
    void mouseMoved(const QPoint& mousePos);
};
}  // namespace ed

#endif  // ED_DOCKMENU_SPLITTER_H_