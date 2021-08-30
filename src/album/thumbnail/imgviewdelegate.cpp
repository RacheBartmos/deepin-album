/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     ZhangYong <zhangyong@uniontech.com>
 *
 * Maintainer: ZhangYong <ZhangYong@uniontech.com>
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
#include "imgviewdelegate.h"
#include "utils/imageutils.h"
#include "utils/baseutils.h"
#include "application.h"
#include <QDateTime>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPixmapCache>
#include <QStandardItemModel>
#include <QThread>
#include <QTimer>
#include <QPainterPath>
#include <QMouseEvent>
#include <QImageReader>
#include <QApplication>
#include <DFontSizeManager>
#include <DGuiApplicationHelper>

#include "imgviewlistview.h"
#include "imagedataservice.h"
namespace {
const QString IMAGE_DEFAULTTYPE = "All pics";
}

const int NotSupportedOrDamagedWidth = 40;      //损坏图片宽度
const int NotSupportedOrDamagedHeigh = 40;
const QString LOCMAP_SELECTED_DARK = ":/resources/dark/images/58 drak.svg";
const QString LOCMAP_NOT_SELECTED_DARK = ":/resources/dark/images/imagewithbg-dark.svg";
const QString LOCMAP_SELECTED_LIGHT = ":/resources/light/images/58.svg";
const QString LOCMAP_NOT_SELECTED_LIGHT = ":/resources/light/images/imagewithbg.svg";

const int NORMAL_ITEM_PAINT_OFFSET = 10;//绘制时普通项向下偏移大小
const int SELECT_ITEM_PAINT_OFFSET = 5;//绘制时选中项向下偏移大小

ImgViewDelegate::ImgViewDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_default = utils::base::renderSVG(":/icons/deepin/builtin/icons/light/picture_default_light.svg", QSize(60, 45));
    m_videoDefault = utils::base::renderSVG(":/icons/deepin/builtin/icons/light/video_default_light.svg", QSize(60, 45));
    m_damagePixmap = utils::image::getDamagePixmap(DApplicationHelper::instance()->themeType() == DApplicationHelper::LightType);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this,
            &ImgViewDelegate::onThemeTypeChanged);
}

void ImgViewDelegate::setItemSize(QSize size)
{
//    m_size = size;
}

void ImgViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    QString pixmapstring;
    QPixmap _pixmap;
    const DBImgInfo data = itemData(index);
    if (data.itemType == ItemTypeBlank) {
        painter->restore();
        return;
    }

    if (!ImageDataService::instance()->imageIsLoaded(data.filePath)) {
        _pixmap = m_default;
    } else {
        QImage img = ImageDataService::instance()->getThumnailImageByPath(data.filePath);
        if (img.isNull()) {
            if (data.itemType == ItemTypeVideo) {
                _pixmap = m_videoDefault;
            } else {
                _pixmap = m_damagePixmap;
            }
        } else {
            _pixmap = QPixmap::fromImage(img);
        }
    }


    bool selected = data.isSelected;
    if (/*(option.state & QStyle::State_MouseOver) &&*/
        (option.state & QStyle::State_Selected) != 0) {
        selected = true;
    }
    painter->setRenderHints(QPainter::HighQualityAntialiasing |
                            QPainter::SmoothPixmapTransform |
                            QPainter::Antialiasing);
    QRect backgroundRect = option.rect;
    if (backgroundRect.width() != ImgViewListView::ITEM_CURRENT_WH) {
        backgroundRect.setTopLeft(QPoint(backgroundRect.topLeft() + QPoint(0, NORMAL_ITEM_PAINT_OFFSET)));
        backgroundRect.setBottomRight(QPoint(backgroundRect.bottomRight() + QPoint(0, NORMAL_ITEM_PAINT_OFFSET)));
    } else {
        backgroundRect.setTopLeft(QPoint(backgroundRect.topLeft() + QPoint(0, SELECT_ITEM_PAINT_OFFSET)));
        backgroundRect.setBottomRight(QPoint(backgroundRect.bottomRight() + QPoint(0, SELECT_ITEM_PAINT_OFFSET)));
    }
    QRect pixmapRect;
    QBrush  backbrush;
    //当前显示项
    if (backgroundRect.width() == ImgViewListView::ITEM_CURRENT_WH) {
        QPainterPath backgroundBp;
        backgroundBp.addRoundedRect(backgroundRect, 8, 8);
        painter->setClipPath(backgroundBp);

        backgroundRect.setX(backgroundRect.x() + 1);
        backgroundRect.setWidth(backgroundRect.width() - 1);
        painter->fillRect(backgroundRect, QBrush(DGuiApplicationHelper::instance()->applicationPalette().highlight().color()));

        pixmapstring = "";
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::DarkType) {
            pixmapstring = LOCMAP_SELECTED_DARK;
            backbrush = QBrush(utils::common::DARK_BACKGROUND_COLOR);
        } else {
            pixmapstring = LOCMAP_SELECTED_LIGHT;
            backbrush = QBrush(utils::common::LIGHT_BACKGROUND_COLOR);
        }

        //绘制默认选中背景
        QRect backRect(backgroundRect.x() + 4, backgroundRect.y() + 4, backgroundRect.width() - 8, backgroundRect.height() - 8);
        QPainterPath backBp;
        backBp.addRoundedRect(backRect, 4, 4);
        painter->setClipPath(backBp);
        painter->fillRect(backRect, backbrush);

        QPainterPath bg;
        bg.addRoundedRect(pixmapRect, 4, 4);
        if (!_pixmap.isNull()) {
            pixmapRect.setX(backgroundRect.x() + 4);
            pixmapRect.setY(backgroundRect.y() + 4);
            pixmapRect.setWidth(backgroundRect.width() - 8);
            pixmapRect.setHeight(backgroundRect.height() - 8);
            bg.addRoundedRect(pixmapRect, 4, 4);
            painter->setClipPath(bg);
        }
    } else {
        pixmapRect.setX(backgroundRect.x() + 1);
        pixmapRect.setY(backgroundRect.y() + 0);
        pixmapRect.setWidth(backgroundRect.width() - 2);
        pixmapRect.setHeight(backgroundRect.height() - 0);

        pixmapstring = "";
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::DarkType) {
            pixmapstring = LOCMAP_NOT_SELECTED_DARK;
        } else {
            pixmapstring = LOCMAP_NOT_SELECTED_LIGHT;
        }

        QPixmap pixmap = utils::base::renderSVG(pixmapstring, QSize(32, 40));
        QPainterPath bg;
        bg.addRoundedRect(pixmapRect, 4, 4);
        painter->setClipPath(bg);
    }

    QPainterPath bp1;
    bp1.addRoundedRect(pixmapRect, 4, 4);
    painter->setClipPath(bp1);
    painter->drawPixmap(pixmapRect, _pixmap);

    painter->restore();
}

QSize ImgViewDelegate::sizeHint(const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    Q_UNUSED(option)
    return index.data(Qt::SizeHintRole).value<QSize>();
}

DBImgInfo ImgViewDelegate::itemData(const QModelIndex &index) const
{
    DBImgInfo data = index.data(Qt::DisplayRole).value<DBImgInfo>();
    data.isSelected = index.data(Qt::UserRole).toBool();
    return data;
}

bool ImgViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model);

    return false;
}

void ImgViewDelegate::onThemeTypeChanged(int themeType)
{
    Q_UNUSED(themeType)
    m_damagePixmap = utils::image::getDamagePixmap(DApplicationHelper::instance()->themeType() == DApplicationHelper::LightType);

    if (DApplicationHelper::instance()->themeType() == DApplicationHelper::LightType) {
        m_default = utils::base::renderSVG(":/icons/deepin/builtin/icons/light/picture_default_light.svg", QSize(60, 45));
        m_videoDefault = utils::base::renderSVG(":/icons/deepin/builtin/icons/light/video_default_light.svg", QSize(60, 45));
    } else {
        m_default = utils::base::renderSVG("::/icons/deepin/builtin/icons/dark/picture_default_dark.svg", QSize(60, 45));
        m_videoDefault = utils::base::renderSVG(":/icons/deepin/builtin/icons/dark/video_default_dark.svg", QSize(60, 45));
    }
}
