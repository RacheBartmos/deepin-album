/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "viewpanel.h"
#include "application.h"
#include "controller/configsetter.h"
#include "controller/wallpapersetter.h"
#include "navigationwidget.h"
#include "scen/imageview.h"
#include "utils/baseutils.h"
#include "utils/imageutils.h"
#include "utils/unionimage.h"
#include "widgets/printhelper.h"
#include <DMenu>
#include <QKeySequence>
#include <QJsonArray>
#include <QJsonDocument>
#include <QShortcut>
#include <QStyleFactory>
#include <QFileInfo>
#include "widgets/dialogs/imgdeletedialog.h"

namespace {
//LMH 500改200
const int SWITCH_IMAGE_DELAY = 200;     //上一张下一张时间间隔
const int DELETE_IMAGE_DELAY = 400;     //删除时间间隔
const QString SHORTCUTVIEW_GROUP = "SHORTCUTVIEW";
const int VIEW_MAINWINDOW_POPVIEW = 4;

QString ss(const QString &text, const QString &defaultValue)
{
    QString str = dApp->setter->value(SHORTCUTVIEW_GROUP, text, defaultValue).toString();
    str.replace(" ", "");
    return str;
}

enum MenuItemId {
    IdFullScreen,
    IdExitFullScreen,
    IdStartSlideShow,
    IdPrint,
    IdAddToAlbum,
    IdExport,
    IdCopy,
    IdMoveToTrash,
    IdRemoveFromTimeline,
    IdRemoveFromAlbum,
    IdAddToFavorites,
    IdRemoveFromFavorites,
    IdShowNavigationWindow,
    IdHideNavigationWindow,
    IdRotateClockwise,
    IdRotateCounterclockwise,
    IdSetAsWallpaper,
    IdDisplayInFileManager,
    IdImageInfo,
    IdSubMenu,
    IdDrawingBoard//lmh0407
};

}  // namespace

void ViewPanel::initPopupMenu()
{
    m_menu = new QMenu;
    connect(this, &ViewPanel::customContextMenuRequested, this, [ = ] {
        if (! m_filepathlist.isEmpty()
#ifdef LITE_DIV
                && !m_currentpath.isEmpty()
#endif
           )
        {
            updateMenuContent();
            this->setCursor(Qt::ArrowCursor);
            m_menu->popup(QCursor::pos());
        }
    });
    connect(m_menu, &DMenu::aboutToHide, this, [ = ] {
        dApp->getDAppNew()->restoreOverrideCursor();
    });
    connect(m_menu, &DMenu::triggered, this, &ViewPanel::onMenuItemClicked);
    connect(dApp->setter, &ConfigSetter::valueChanged, this, [ = ] {
        if (/*this && */this->isVisible())
        {
            updateMenuContent();
        }
    });
//    QShortcut *sc = new QShortcut(QKeySequence("Alt+Enter"), this);
//    sc->setContext(Qt::WindowShortcut);
//    connect(sc, &QShortcut::activated, this, [ = ] {
//        if (m_isInfoShowed)
//            emit dApp->signalM->hideExtensionPanel();
//        else
//            emit dApp->signalM->showExtensionPanel();
//        // Update panel info
//        m_info->setImagePath(m_infos.at(m_current).filePath);
//    });
}

void ViewPanel::appendAction(int id, const QString &text, const QString &shortcut)
{
    QAction *ac = new QAction(m_menu);
    addAction(ac);
    ac->setText(text);
    ac->setProperty("MenuID", id);
    ac->setShortcut(QKeySequence(shortcut));
    m_menu->addAction(ac);
}
void ViewPanel::appendAction_darkmenu(int id, const QString &text, const QString &shortcut)
{
    QAction *ac = new QAction(m_menu);
    addAction(ac);
    ac->setDisabled(true);
    ac->setText(text);
    ac->setProperty("MenuID", id);
    ac->setShortcut(QKeySequence(shortcut));
    m_menu->addAction(ac);
}

void ViewPanel::onMenuItemClicked(QAction *action)
{
    using namespace utils::base;
    using namespace utils::image;

    const QString path = m_currentpath;
    const int id = action->property("MenuID").toInt();

    switch (MenuItemId(id)) {
    case IdFullScreen:
    case IdExitFullScreen:
        if (m_bFirstFullScreen)
            emit dApp->signalM->hideImageView();
        else {
            toggleFullScreen();
        }
        break;
    case IdStartSlideShow: {
        auto vinfo = m_vinfo;
        vinfo.fullScreen = window()->isFullScreen();
//        vinfo.lastPanel = this;
        vinfo.lastPanel = nullptr;
        vinfo.path = path;
//        vinfo.paths = paths();
        vinfo.paths = m_filepathlist;
        vinfo.viewMainWindowID = VIEW_MAINWINDOW_POPVIEW;

        QStringList pathlist;
        pathlist.clear();
        for (auto path : m_vinfo.paths) {
            if (QFileInfo(path).exists()) {
                pathlist << path;
            }
        }

        m_vinfo.paths = pathlist;
        emit dApp->signalM->startSlideShow(vinfo, m_vinfo.inDatabase);
        emit dApp->signalM->showSlidePanel(VIEW_MAINWINDOW_POPVIEW);
        break;
    }
    case IdPrint: {
        PrintHelper::showPrintDialog(QStringList(path), this);
        break;
    }
    case IdDrawingBoard: {
        emit dApp->signalM->sigDrawingBoard(QStringList(path));
        break;
    }

#if 1
    //添加到相册
    case IdAddToAlbum: {
        const QString album = action->data().toString();
        if (album != "Add to new album") {
            if (! DBManager::instance()->isImgExistInAlbum(album, path)) {
                emit dApp->signalM->sigAddToAlbToast(album);
            }
            DBManager::instance()->insertIntoAlbum(album, QStringList(path));
        } else {
            emit dApp->signalM->viewCreateAlbum(path, false);
        }
    }
    break;
    //收藏
    case IdAddToFavorites: {
        DBManager::instance()->insertIntoAlbum(COMMON_STR_FAVORITES, QStringList(path), AlbumDBType::Favourite);
        emit dApp->signalM->insertedIntoAlbum(COMMON_STR_FAVORITES, QStringList(path));
    }
    break;
    //取消收藏
    case IdRemoveFromFavorites: {
        DBManager::instance()->removeFromAlbum(COMMON_STR_FAVORITES, QStringList(path), AlbumDBType::Favourite);
    }
    break;
    //导出
    case IdExport:
        emit dApp->signalM->exportImage(QStringList(path));
        break;
#endif
    case IdCopy:
        copyImageToClipboard(QStringList(path));
        break;
    case IdMoveToTrash: {
        emit SignalManager::instance()->deleteByMenu();
//        SignalManager::ViewInfo info;
        if (m_vinfo.fullScreen == true) {
            emit SignalManager::instance()->hideBottomToolbar();
        }
//        if (utils::common::VIEW_ALLPIC_SRN != m_viewType
//                && utils::common::VIEW_TIMELINE_SRN != m_viewType
//                && utils::common::VIEW_SEARCH_SRN != m_viewType
//                && COMMON_STR_RECENT_IMPORTED != m_viewType
//                && COMMON_STR_TRASH != m_viewType
//                && COMMON_STR_FAVORITES != m_viewType)
//        {
//            DBManager::instance()->removeFromAlbum(m_vinfo.viewType, QStringList(m_infos.at(m_current).filePath));
//            removeCurrentImage();
//        }
//        else if (COMMON_STR_TRASH == m_viewType)
//        {
//            ImgDeleteDialog *dialog = new ImgDeleteDialog(1);
//            dialog->show();
//            connect(dialog,&ImgDeleteDialog::imgdelete,this,[=]
//            {
//                dApp->m_imagetrashmap.remove(m_infos.at(m_current).filePath);
//                DBManager::instance()->removeTrashImgInfos(QStringList(m_infos.at(m_current).filePath));
//                removeCurrentImage();
//            });
//        }
//        else
//        {
//        DBImgInfoList infos;
//        DBImgInfo info;
//        info = DBManager::instance()->getInfoByPath(m_infos.at(m_current).filePath);
//#if 1
////        info.time = QDateTime::currentDateTime();
//        info.changeTime = QDateTime::currentDateTime();
//#endif
//        infos << info;
//        dApp->m_imageloader->addTrashImageLoader(QStringList(m_infos.at(m_current).filePath));
////        dApp->m_imagemap.remove(m_infos.at(m_current).filePath);
//        DBManager::instance()->insertTrashImgInfos(infos);
//        DBManager::instance()->removeImgInfos(QStringList(m_infos.at(m_current).filePath));


//        removeCurrentImage();
//        }
    }
    break;
    case IdRemoveFromAlbum:
        DBManager::instance()->removeFromAlbum(m_vinfo.viewType, QStringList(m_currentpath));
        removeCurrentImage();
        break;
    case IdShowNavigationWindow:
        m_nav->setAlwaysHidden(false);
        break;
    case IdHideNavigationWindow:
        m_nav->setAlwaysHidden(true);
        break;
    case IdRotateClockwise:
        rotateImage(true);
        break;
    case IdRotateCounterclockwise:
        rotateImage(false);
        break;
    case IdSetAsWallpaper:
        dApp->wpSetter->setBackground(path);
        break;
    case IdDisplayInFileManager:
        emit dApp->signalM->showInFileManager(path);
        break;
    case IdImageInfo:
        emit dApp->signalM->showImageInfo(path);
        // Update panel info
        break;
    default:
        break;
    }

    updateMenuContent();
}

void ViewPanel::updateMenuContent()
{
    m_menu->clear();
    qDeleteAll(this->actions());

    if (m_vinfo.viewType.compare(COMMON_STR_TRASH) == 0) {
        return;
    }

    if (m_filepathlist.isEmpty()) {
        return;
    }


    // 如果该图片原文件不存在,则不显示右键菜单
    if (!QFileInfo(m_currentpath).exists()) {
        return;
    }

    if (window()->isFullScreen()) {

        appendAction(IdExitFullScreen, tr("Exit fullscreen"), ss("Fullscreen", "F11"));

    } else {

        appendAction(IdFullScreen, tr("Fullscreen"), ss("Fullscreen", "F11"));
    }
    appendAction(IdPrint, tr("Print"), ss("Print", "Ctrl+P"));
    appendAction(IdStartSlideShow, tr("Slide show"), ss("Slide show", "F5"));
#ifndef LITE_DIV
    appendAction(IdStartSlideShow, tr("Slide show"), ss("Slide show"));
#endif
//    appendAction(IdPrint, tr("Print"), ss("Print", "Ctrl+P"));
#ifndef LITE_DIV
    if (m_vinfo.inDatabase) {
        DMenu *am = createAlbumMenu();
        if (am) {
            m_menu->addMenu(am);
        }
    }
#endif
    m_menu->addSeparator();
    /**************************************************************************/
#if 1
    m_menu->addMenu(createAblumMenu());                                         //添加到相册
    appendAction(IdExport, tr("Export"), ss("Export", "Ctrl+E"));   //导出
#endif
    appendAction(IdCopy, tr("Copy"), ss("Copy", "Ctrl+C"));
    if (COMMON_STR_TRASH == m_vinfo.viewType) {
//        appendAction(IdMoveToTrash, tr("Delete"), ss("Throw to trash", "Delete"));
    } else {
        appendAction(IdMoveToTrash, tr("Delete"), ss("Throw to trash", "Delete"));
    }
    if (utils::common::VIEW_ALLPIC_SRN != m_vinfo.viewType
            && utils::common::VIEW_TIMELINE_SRN != m_vinfo.viewType
            && utils::common::VIEW_SEARCH_SRN != m_vinfo.viewType
            && COMMON_STR_RECENT_IMPORTED != m_vinfo.viewType
            && COMMON_STR_TRASH != m_vinfo.viewType
            && COMMON_STR_FAVORITES != m_vinfo.viewType) {
        appendAction(IdRemoveFromAlbum, tr("Remove from album"), ss("Remove from album", ""));
    }
    m_menu->addSeparator();
    /**************************************************************************/
#if 1
    if (DBManager::instance()->isImgExistInAlbum(COMMON_STR_FAVORITES, m_currentpath, AlbumDBType::Favourite)) {
        appendAction(IdRemoveFromFavorites, tr("Unfavorite"), "");    //取消收藏
    } else {
        appendAction(IdAddToFavorites, tr("Favorite"), "");       //收藏
    }
    m_menu->addSeparator();
#endif
    if (! m_viewB->isWholeImageVisible() && m_nav->isAlwaysHidden()) {
        appendAction(IdShowNavigationWindow,
                     tr("Show navigation window"), ss("Show navigation window", ""));
    } else if (! m_viewB->isWholeImageVisible() && !m_nav->isAlwaysHidden()) {
        appendAction(IdHideNavigationWindow,
                     tr("Hide navigation window"), ss("Hide navigation window", ""));
    }
    /**************************************************************************/
    if (UnionImage_NameSpace::isImageSupportRotate(m_currentpath)) {
        m_menu->addSeparator();
        if (QFileInfo(m_currentpath).isReadable() &&
                !QFileInfo(m_currentpath).isWritable()) {
            appendAction_darkmenu(IdRotateClockwise, tr("Rotate clockwise"), ss("Rotate clockwise", "Ctrl+R"));
            appendAction_darkmenu(IdRotateCounterclockwise, tr("Rotate counterclockwise"), ss("Rotate counterclockwise", "Ctrl+Shift+R"));
        } else {
            appendAction(IdRotateClockwise, tr("Rotate clockwise"), ss("Rotate clockwise", "Ctrl+R"));
            appendAction(IdRotateCounterclockwise, tr("Rotate counterclockwise"), ss("Rotate counterclockwise", "Ctrl+Shift+R"));
        }
    }
    /**************************************************************************/
    appendAction(IdSetAsWallpaper, tr("Set as wallpaper"), ss("Set as wallpaper", "Ctrl+F9"));
    appendAction(IdDisplayInFileManager, tr("Display in file manager"), ss("Display in file manager", "Alt+D"));
    appendAction(IdImageInfo, tr("Photo info"), ss("Photo info", "Ctrl+I"));
}
#if 1
DMenu *ViewPanel::createAblumMenu()
{
    DMenu *am = new DMenu(tr("Add to album"));

    QStringList albums = DBManager::instance()->getAllAlbumNames();
    albums.removeAll(COMMON_STR_FAVORITES);
    albums.removeAll(COMMON_STR_TRASH);
    albums.removeAll(COMMON_STR_RECENT_IMPORTED);

    QAction *ac = new QAction(am);
    ac->setProperty("MenuID", IdAddToAlbum);
    ac->setText(tr("New album"));
    ac->setData("Add to new album");
    ac->setShortcut(QKeySequence("Ctrl+Shift+N"));
    am->addAction(ac);
    am->addSeparator();

    for (QString album : albums) {
        QAction *ac = new QAction(am);
        ac->setProperty("MenuID", IdAddToAlbum);
        ac->setText(fontMetrics().elidedText(QString(album).replace("&", "&&"), Qt::ElideMiddle, 200));
        ac->setData(album);
        am->addAction(ac);
    }

    return am;
}
#endif

void ViewPanel::initShortcut()
{
    // Delay image toggle delete
    m_deletetimer = new QTimer(this);
    m_deletetimer->setSingleShot(true);
    m_deletetimer->setInterval(DELETE_IMAGE_DELAY);

    QShortcut *sc = nullptr;
    // Previous
    sc = new QShortcut(QKeySequence(Qt::Key_Left), this);
    sc->setContext(Qt::WindowShortcut);
    connect(sc, &QShortcut::activated, this, [ = ] {
//        if (! m_dt->isActive())
//        {
//            dt->start();
        m_ttbc->onPreButton();
//    }
    });
// Next
    sc = new QShortcut(QKeySequence(Qt::Key_Right), this);
    sc->setContext(Qt::WindowShortcut);
    connect(sc, &QShortcut::activated, this, [ = ] {
        m_ttbc->onNextButton();
    });

// Zoom out (Ctrl++ Not working, This is a confirmed bug in Qt 5.5.0)
    connect(dApp->signalM, &SignalManager::sigCtrlADDKeyActivated, this, [ = ] {
        m_viewB->setScaleValue(1.1);
    });

    connect(dApp->signalM, &SignalManager::sigCtrlSubtractKeyActivated, this, [ = ] {
        m_viewB->setScaleValue(0.9);
    });

//    sc = new QShortcut(QKeySequence(Qt::Key_Up), this);
//    sc->setContext(Qt::WindowShortcut);
//    connect(sc, &QShortcut::activated, this, [=] {
//        qDebug() << "Qt::Key_Up:";
//        m_viewB->setScaleValue(1.1);
//    });
//    sc = new QShortcut(QKeySequence("Ctrl+="), this);
//    sc->setContext(Qt::WidgetShortcut);
//    connect(sc, &QShortcut::activated, this, [=] {
//        m_viewB->setScaleValue(1.1);
//    });
// Zoom in
//    sc = new QShortcut(QKeySequence(Qt::Key_Down), this);
//    sc->setContext(Qt::WindowShortcut);
//    connect(sc, &QShortcut::activated, this, [=] {
//        qDebug() << "Qt::Key_Down:";
//        m_viewB->setScaleValue(0.9);
//    });
//    sc = new QShortcut(QKeySequence("Ctrl+-"), this);
//    sc->setContext(Qt::WindowShortcut);
//    connect(sc, &QShortcut::activated, this, [=] {
//        m_viewB->setScaleValue(0.9);
//    });
// Esc
//    QShortcut *esc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
//    esc->setContext(Qt::WindowShortcut);
//    connect(esc, &QShortcut::activated, this, [=] {
//        if (window()->isFullScreen()) {
//            toggleFullScreen();
//        }
//        else {
//            if (m_vinfo.inDatabase) {
//                backToLastPanel();
//            }
//            else {
//                dApp->quit();
//            }
//        }
//        emit dApp->signalM->hideExtensionPanel(true);
//    });
//1:1 size
    QShortcut *adaptImage = new QShortcut(QKeySequence("Ctrl+0"), this);
    adaptImage->setContext(Qt::WindowShortcut);
    connect(adaptImage, &QShortcut::activated, this, [ = ] {
        m_viewB->fitImage();
    });

}

void ViewPanel::popupDelDialog(const QString path)
{
#ifndef LITE_DIV
    const QStringList paths(path);
    FileDeleteDialog *fdd = new FileDeleteDialog(paths);
    fdd->showInCenter(window());
#else
    Q_UNUSED(path)
#endif
}

void ViewPanel::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Period) {
        if (!DBManager::instance()->isImgExistInAlbum(COMMON_STR_FAVORITES, m_currentpath, AlbumDBType::Favourite)) {
            DBManager::instance()->insertIntoAlbum(COMMON_STR_FAVORITES, QStringList(m_currentpath), AlbumDBType::Favourite);
            emit dApp->signalM->insertedIntoAlbum(COMMON_STR_FAVORITES, QStringList(m_currentpath));
        } else {
            DBManager::instance()->removeFromAlbum(COMMON_STR_FAVORITES, QStringList(m_currentpath), AlbumDBType::Favourite);
        }
    }
}
