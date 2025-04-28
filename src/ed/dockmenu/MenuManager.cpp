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

#include "ed/dockmenu/MenuManager.h"

#include <QBoxLayout>
#include <QByteArray>
#include <QFile>
#include <QMainWindow>

#include "ed/dockmenu/MenuAreaWidget.h"
#include "ed/dockmenu/MenuButton.h"
#include "ed/dockmenu/MenuTabBar.h"
#include "ed/dockmenu/MenuWidget.h"
#include "ed/dockmenu/Provider.h"
#include "ed/dockmenu/Splitter.h"

/**
 * Initializes the resources specified by the .qrc file with the specified base
 * name. Normally, when resources are built as part of the application, the
 * resources are loaded automatically at startup. The Q_INIT_RESOURCE() macro
 * is necessary on some platforms for resources stored in a static library.
 * Because GCC causes a linker error if we put Q_INIT_RESOURCE into the
 * loadStyleSheet() function, we place it into a function outside of the ads
 * namespace
 */
static void initDockMenuResource() {
    Q_INIT_RESOURCE(dockmenu);
}
Q_COREAPP_STARTUP_FUNCTION(initDockMenuResource)

namespace ed {

struct EMenuManager::Private {
    Private() = default;

    QBoxLayout *layout;
    EMenuTabBar *styleBar;
    ESplitter *splitter;
    QByteArray splitterState;
    EMenuAreaWidget *menuArea;
    MenuDirection direction;
    bool toolClosed;
};

EMenuManager::EMenuManager(MenuDirection direction, QWidget *parent) : QFrame(parent), d(new Private) {
    setObjectName("EDockMenuManager");
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);

    d->toolClosed = true;
    d->direction = direction;
    d->styleBar = new EMenuTabBar(direction, this);
    d->menuArea = new EMenuAreaWidget(direction, this);

    connect(d->styleBar, &EMenuTabBar::toolSelected, this, &EMenuManager::onToolSelected);
    connect(d->styleBar, &EMenuTabBar::toolClosed, this, &EMenuManager::onToolClosed);

    d->splitter = new ESplitter(Qt::Horizontal, this);
    connect(d->splitter, &ESplitter::splitterReady, this, &EMenuManager::onSplitterReady);

    switch (direction) {
        case MenuDirection::Left:
            d->layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
            break;

        case MenuDirection::Right:
            d->layout = new QBoxLayout(QBoxLayout::RightToLeft, this);
            break;

        case MenuDirection::Top:
            d->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
            break;

        case MenuDirection::Bottom:
            d->layout = new QBoxLayout(QBoxLayout::BottomToTop, this);
            break;
        default:
            break;
    }

    d->layout->setContentsMargins(QMargins(0, 0, 0, 0));
    d->layout->setSpacing(0);
    d->layout->addWidget(d->styleBar);
    d->layout->addWidget(d->splitter, 1);

    setLayout(d->layout);

    QMainWindow *MainWindow = qobject_cast<QMainWindow *>(parent);
    if (MainWindow) {
        MainWindow->setCentralWidget(this);
    }

    this->loadStylesheet();
}

EMenuManager::~EMenuManager() {
    delete d;
}

void EMenuManager::addMenu(const QString &name, const QString &iconNormal, const QString &iconActive,
                           const QString &tooltip, QWidget *widget) {
    EMenuButton *button = new EMenuButton(d->direction, QSize(30, 30), iconNormal, iconActive, tooltip, this);
    d->styleBar->addMenuButton(button);

    EMenuWidget *menuWidget = new EMenuWidget(name, widget, this);
    connect(menuWidget, &EMenuWidget::undockClicked, this, &EMenuManager::onUndockButtonClicked);
    connect(menuWidget, &EMenuWidget::dockClicked, this, &EMenuManager::onDockButtonClicked);

    d->menuArea->addMenuWidget(menuWidget);
}

void EMenuManager::setCentralWidget(QWidget *widget) {
    d->splitter->addWidget(d->menuArea);
    d->splitter->setCollapsible(0, true);

    d->splitter->addWidget(widget);
    d->splitter->setCollapsible(1, false);

    d->splitter->setStretchFactor(0, 25);
    d->splitter->setStretchFactor(1, 75);

    connect(qobject_cast<ESplitterHandle *>(d->splitter->handle(1)), &ESplitterHandle::dragFinished, this,
            &EMenuManager::onMenuDragFinished);

    d->toolClosed = false;
    d->styleBar->setSelected(0);
    d->menuArea->toolSelected(0);
}

EProvider &EMenuManager::provider() {
    return ed::EProvider::instance();
}

void EMenuManager::onUndockButtonClicked() {
    ED_PRINT("EMenuManager::onUndockButtonClicked()");
}

void EMenuManager::onDockButtonClicked() {
    ED_PRINT("EMenuManager::onDockButtonClicked()");
}

void EMenuManager::onMenuDragFinished() {
    if (d->splitter->sizes()[0] == 0) {
        d->toolClosed = true;
        d->styleBar->setClose();
        return;
    }
    d->splitterState = d->splitter->saveState();

    if (d->toolClosed) {
        int currentIndex = d->menuArea->getCurrentIndex();
        if (currentIndex >= 0) {
            d->styleBar->setSelected(currentIndex);
            d->toolClosed = false;
        }
    }
}

void EMenuManager::onToolClosed() {
    if (d->splitter->count() < 2) {
        return;
    }

    d->toolClosed = true;

    QSize size = d->splitter->size();
    if (d->direction == MenuDirection::Left || d->direction == MenuDirection::Right) {
        d->splitter->setSizes({0, size.width()});
    } else {
        d->splitter->setSizes({0, size.height()});
    }
}

void EMenuManager::onToolSelected(int index) {
    if (d->splitter->count() < 2) {
        return;
    }

    d->toolClosed = false;

    if (d->splitter->sizes()[0] != 0) {
        d->menuArea->toolSelected(index);
        return;
    }

    if (!d->splitterState.isEmpty()) {
        d->splitter->restoreState(d->splitterState);
        d->menuArea->toolSelected(index);
        return;
    }

    QSize size = d->splitter->size();
    if (d->direction == MenuDirection::Left || d->direction == MenuDirection::Right) {
        int menuSize = size.width() / 5;
        d->splitter->setSizes({menuSize, size.width() - menuSize});
    } else {
        int menuSize = size.height() / 5;
        d->splitter->setSizes({menuSize, size.height() - menuSize});
    }

    d->menuArea->toolSelected(index);
}

void EMenuManager::onSplitterReady() {
    d->splitterState = d->splitter->saveState();
}

void EMenuManager::loadStylesheet() {
    QString Result;
    QString FileName = ":ed/stylesheets/";
    FileName += "default";

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    FileName += "_linux";
#endif
    FileName += ".css";

    ED_PRINT("Loading stylesheet: " << FileName);

    QFile StyleSheetFile(FileName);
    StyleSheetFile.open(QIODevice::ReadOnly);
    QTextStream StyleSheetStream(&StyleSheetFile);
    Result = StyleSheetStream.readAll();
    StyleSheetFile.close();
    this->setStyleSheet(Result);
}

}  // namespace ed