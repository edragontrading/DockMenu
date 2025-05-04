#ifndef ED_DOCKMENU_FLOATING_H
#define ED_DOCKMENU_FLOATING_H

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

#include <QWidget>

#include "ed/dockmenu/ed_menu_globals.h"

namespace ed {

class EMenuManager;

class ED_EXPORT EMenuFloating : public QWidget {
    Q_OBJECT

public:
    explicit EMenuFloating(EMenuManager* menuManager);
    ~EMenuFloating() override;

    void startFloating(const QPoint& dragStartMousePos, const QSize& size, eDragState dragState);
    void moveFloating();

    void removeMenuWidget();

protected:
    void closeEvent(QCloseEvent* event) override;

#ifdef Q_OS_WIN
    /**
     * Native event filter for handling WM_MOVING messages on Windows
     */
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
#else
    virtual bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#endif
#endif

#ifdef Q_OS_MACOS
    virtual bool event(QEvent* e) override;
    virtual void moveEvent(QMoveEvent* event) override;
#endif

private:
    void titleMouseReleaseEvent();
    void handleEscapeKey();
    void updateDropOverlays(const QPoint& globalPos);

private Q_SLOTS:
    void onToolSelected(int index);
    void onToolClosed();

private:
    struct Private;
    Private* d;
};
}  // namespace ed

#endif  // ED_DOCKMENU_FLOATING_H
