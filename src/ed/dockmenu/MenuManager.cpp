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

#include <QApplication>
#include <QBoxLayout>
#include <QFile>
#include <QList>
#include <QMainWindow>
#include <QPainter>

#include "ed/dockmenu/MenuAreaWidget.h"
#include "ed/dockmenu/MenuButton.h"
#include "ed/dockmenu/MenuFloating.h"
#include "ed/dockmenu/MenuOverlay.h"
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

    MenuDirection direction;
    bool toolClosed;
    bool splitterReady;
    bool floating;

    QBoxLayout *layout;
    EMenuTabBar *styleBar;
    ESplitter *splitter;
    QList<int> splitterState;

    EMenuOverlay *menuOverlay;
    EMenuAreaWidget *menuArea;
    EMenuFloating *floatingWidget = nullptr;
};

EMenuManager::EMenuManager(MenuDirection direction, QWidget *parent) : QFrame(parent), d(new Private) {
    setObjectName("EDockMenuManager");
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);

    d->toolClosed = true;
    d->splitterReady = false;
    d->floating = false;
    d->direction = direction;
    d->splitterState = QList<int>();
    d->styleBar = new EMenuTabBar(direction, this);
    d->menuArea = new EMenuAreaWidget(direction, this);
    d->menuOverlay = new EMenuOverlay(this);

    connect(d->styleBar, &EMenuTabBar::toolSelected, this, &EMenuManager::onToolSelected);
    connect(d->styleBar, &EMenuTabBar::toolClosed, this, &EMenuManager::onToolClosed);

    switch (direction) {
        case MenuDirection::Left:
            d->layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
            d->splitter = new ESplitter(Qt::Horizontal, this);
            break;

        case MenuDirection::Right:
            d->layout = new QBoxLayout(QBoxLayout::RightToLeft, this);
            d->splitter = new ESplitter(Qt::Horizontal, this);
            break;

        case MenuDirection::Top:
            d->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
            d->splitter = new ESplitter(Qt::Vertical, this);
            break;

        case MenuDirection::Bottom:
            d->layout = new QBoxLayout(QBoxLayout::BottomToTop, this);
            d->splitter = new ESplitter(Qt::Vertical, this);
            break;
        default:
            break;
    }

    connect(d->splitter, &ESplitter::splitterReady, this, &EMenuManager::onSplitterReady);

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

    EMenuWidget *menuWidget = new EMenuWidget(this, name, widget, this);

    d->menuArea->addMenuWidget(menuWidget);
}

void EMenuManager::setCentralWidget(QWidget *widget) {
    switch (d->direction) {
        case MenuDirection::Left:
        case MenuDirection::Top:
            d->splitter->addWidget(d->menuArea);
            d->splitter->addWidget(widget);
            d->splitter->setCollapsible(0, true);
            d->splitter->setCollapsible(1, false);
            break;

        case MenuDirection::Right:
        case MenuDirection::Bottom:
            d->splitter->addWidget(widget);
            d->splitter->addWidget(d->menuArea);
            d->splitter->setCollapsible(0, false);
            d->splitter->setCollapsible(1, true);
            break;

        default:
            break;
    }

    connect(qobject_cast<ESplitterHandle *>(d->splitter->handle(1)), &ESplitterHandle::dragFinished, this,
            &EMenuManager::onMenuDragFinished);

    d->toolClosed = false;
    d->styleBar->setSelected(0);
    d->menuArea->toolSelected(0);
    updateFloatingState(false);
}

MenuDirection EMenuManager::direction() const {
    return d->direction;
}

EMenuTabBar *EMenuManager::takeTabBar() {
    int index = d->layout->indexOf(d->styleBar);
    if (index != -1) {
        disconnect(d->styleBar, &EMenuTabBar::toolSelected, this, &EMenuManager::onToolSelected);
        disconnect(d->styleBar, &EMenuTabBar::toolClosed, this, &EMenuManager::onToolClosed);
        d->layout->removeWidget(d->styleBar);
    }
    return d->styleBar;
}

EMenuAreaWidget *EMenuManager::takeMenuAreaWidget() {
    int index = d->splitter->indexOf(d->menuArea);
    if (index != -1) {
        disconnect(qobject_cast<ESplitterHandle *>(d->splitter->handle(1)), &ESplitterHandle::dragFinished, this,
                   &EMenuManager::onMenuDragFinished);
        d->splitter->widget(index)->setParent(nullptr);  // Detach
    }
    return d->menuArea;
}

EMenuOverlay *EMenuManager::menuOverlay() const {
    return d->menuOverlay;
}

void EMenuManager::registerFloatingWidget(EMenuFloating *floatingWidget) {
    if (d->floatingWidget != nullptr) {
        d->floatingWidget->removeMenuWidget();
        d->floatingWidget->close();
    }

    d->floatingWidget = floatingWidget;
    updateFloatingState(true);
}

void EMenuManager::redockMenu(bool closed) {
    if (d->floatingWidget == nullptr) {
        return;
    }

    d->floatingWidget->removeMenuWidget();
    if (!closed) {
        d->floatingWidget->close();
    }
    d->floatingWidget = nullptr;

    if (d->direction == MenuDirection::Left || d->direction == MenuDirection::Top) {
        d->splitter->insertWidget(0, d->menuArea);
        d->splitter->setCollapsible(0, true);
    } else {
        d->splitter->addWidget(d->menuArea);
        d->splitter->setCollapsible(1, true);
    }

    if (!closed) {
        int currentIndex = d->menuArea->getCurrentIndex();
        d->styleBar->setSelected(currentIndex);
        setDefaultSize();
        d->toolClosed = false;
    } else {
        setClosedSize();
        d->styleBar->setClose();
        d->toolClosed = true;
    }

    connect(qobject_cast<ESplitterHandle *>(d->splitter->handle(1)), &ESplitterHandle::dragFinished, this,
            &EMenuManager::onMenuDragFinished);

    d->layout->insertWidget(0, d->styleBar);
    connect(d->styleBar, &EMenuTabBar::toolSelected, this, &EMenuManager::onToolSelected);
    connect(d->styleBar, &EMenuTabBar::toolClosed, this, &EMenuManager::onToolClosed);

    updateFloatingState(false);
    this->update();
}

void EMenuManager::updateFloatingState(bool floating) {
    d->floating = floating;
    d->menuArea->updateState(floating);
}

bool EMenuManager::floating() const {
    return d->floating;
}

QSize EMenuManager::getMenuSize() const {
    int maxWidth = d->styleBar->size().width() + d->menuArea->size().width();
    int maxHeight = d->styleBar->size().height() + d->menuArea->size().height();

    if (d->direction == MenuDirection::Left || d->direction == MenuDirection::Right) {
        return QSize(maxWidth, d->styleBar->size().height());
    }

    return QSize(d->styleBar->size().width(), maxHeight);
}

QPixmap EMenuManager::captureMenuWidgets() {
    QPixmap pixmaptabBar(d->styleBar->size());
    d->styleBar->render(&pixmaptabBar);

    QPixmap pixmapMenuArea(d->menuArea->size());
    d->menuArea->render(&pixmapMenuArea);

    if (d->direction == MenuDirection::Left) {
        return internal::createPixmap(d->styleBar, d->menuArea, Qt::Horizontal);
    } else if (d->direction == MenuDirection::Right) {
        return internal::createPixmap(d->menuArea, d->styleBar, Qt::Horizontal);
    } else if (d->direction == MenuDirection::Top) {
        return internal::createPixmap(d->styleBar, d->menuArea, Qt::Vertical);
    }

    return internal::createPixmap(d->menuArea, d->styleBar, Qt::Vertical);
}

EProvider &EMenuManager::provider() {
    return ed::EProvider::instance();
}

int EMenuManager::startDragDistance() {
    return QApplication::startDragDistance() * 1.5;
}

void EMenuManager::onMenuDragFinished() {
    if (!menuVisible(d->splitter->sizes())) {
        d->toolClosed = true;
        d->styleBar->setClose();
        return;
    }
    d->splitterState = d->splitter->sizes();

    if (d->toolClosed) {
        int currentIndex = d->menuArea->getCurrentIndex();
        d->styleBar->setSelected(currentIndex);
        d->toolClosed = false;
    }
}

void EMenuManager::onToolClosed() {
    if (d->splitter->count() < 2) {
        return;
    }

    d->toolClosed = true;
    setClosedSize();
}

void EMenuManager::onToolSelected(int index) {
    if (d->splitter->count() < 2) {
        return;
    }

    d->toolClosed = false;

    if (menuVisible(d->splitter->sizes())) {
        d->menuArea->toolSelected(index);
        return;
    }

    if (menuVisible(d->splitterState)) {
        d->splitter->setSizes(d->splitterState);
        d->menuArea->toolSelected(index);
        return;
    }

    setDefaultSize();
    d->menuArea->toolSelected(index);
}

void EMenuManager::onSplitterReady() {
    if (!d->splitterReady) {
        setDefaultSize();
        d->splitterReady = true;
    }
    d->splitterState = d->splitter->sizes();
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

void EMenuManager::setDefaultSize() {
    QSize size = d->splitter->size();
    int menu_width = size.width() / 5;
    int menu_height = size.height() / 5;

    switch (d->direction) {
        case MenuDirection::Left:
            d->splitter->setSizes({menu_width, size.width() - menu_width});
            break;
        case MenuDirection::Right:
            d->splitter->setSizes({size.width() - menu_width, menu_width});
            break;

        case MenuDirection::Top:
            d->splitter->setSizes({menu_height, size.height() - menu_height});
            break;

        case MenuDirection::Bottom:
            d->splitter->setSizes({size.height() - menu_height, menu_height});
            break;

        default:
            break;
    }
}

void EMenuManager::setClosedSize() {
    QSize size = d->splitter->size();

    switch (d->direction) {
        case MenuDirection::Left:
            d->splitter->setSizes({0, size.width()});
            break;
        case MenuDirection::Right:
            d->splitter->setSizes({size.width(), 0});
            break;

        case MenuDirection::Top:
            d->splitter->setSizes({0, size.height()});
            break;

        case MenuDirection::Bottom:
            d->splitter->setSizes({size.height(), 0});
            break;

        default:
            break;
    }
}

bool EMenuManager::menuVisible(const QList<int> &sizes) {
    if (sizes.isEmpty() || sizes.count() < 2) {
        return false;
    }

    if (d->direction == MenuDirection::Left || d->direction == MenuDirection::Top) {
        return (sizes[0] != 0);
    }

    // Right & Bottom
    return (sizes[1] != 0);
}

}  // namespace ed