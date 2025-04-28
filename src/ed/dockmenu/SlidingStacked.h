#ifndef ED_DOCKMENU_SLIDING_STACKED_H_
#define ED_DOCKMENU_SLIDING_STACKED_H_

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

#include <QEasingCurve>
#include <QParallelAnimationGroup>
#include <QStackedWidget>

#include "ed/dockmenu/ed_menu_globals.h"

namespace ed {

class ED_EXPORT ESlidingStacked : public QStackedWidget {
    Q_OBJECT
public:
    enum SlidingDirection {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT,
        TOP_TO_BOTTOM,
        BOTTOM_TO_TOP,
        AUTOMATIC
    };
    Q_ENUM(SlidingDirection);

    explicit ESlidingStacked(QWidget* parent = nullptr);
    ~ESlidingStacked() override;

public Q_SLOTS:
    void setSpeed(int speed);

    void setAnimation(enum QEasingCurve::Type animationtype);

    void setVerticalMode(bool vertical = true);

    void setWrap(bool wrap);

    bool slideInNext();

    bool slideInPrev();

    void slideInIdx(int idx, SlidingDirection direction = AUTOMATIC);

    void slideInWgt(QWidget* widget, SlidingDirection direction = AUTOMATIC);

Q_SIGNALS:
    //! Animation is finished
    void animationFinished(void);

protected Q_SLOTS:
    void animationDoneSlot(void);

private:
    struct Private;
    Private* d;
};
}  // namespace ed

#endif  // ED_DOCKMENU_SLIDING_STACKED_H_