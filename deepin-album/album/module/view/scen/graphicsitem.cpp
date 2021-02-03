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
#include "graphicsitem.h"
#include <QMovie>
#include <QDebug>
#include <QPainter>
#include <FreeImage.h>
#include "utils/imageutils.h"


GraphicsMovieItem::GraphicsMovieItem(const QString &fileName, const QString &fileSuffix, QGraphicsItem *parent)
    : QGraphicsPixmapItem(fileName, parent)
    , m_qmovie(nullptr)
//    , m_suffix(fileSuffix)
//    , m_index(0)
{
    Q_UNUSED(fileSuffix)
    //gif花屏，全部采用QMovie
    m_qmovie = new QMovie(fileName);
    connect(m_qmovie, &QMovie::frameChanged, this, [ = ]() {
        if (!m_qmovie->currentPixmap().isNull()) {
            setPixmap(m_qmovie->currentPixmap());
        }
    });
    m_qmovie->start();
}

GraphicsMovieItem::~GraphicsMovieItem()
{
    prepareGeometryChange();

    m_qmovie->stop();
    m_qmovie->deleteLater();
    m_qmovie = nullptr;
}

void GraphicsMovieItem::start()
{
    m_qmovie->start();
}

void GraphicsMovieItem::stop()
{
    m_qmovie->stop();
}


GraphicsPixmapItem::GraphicsPixmapItem(const QPixmap &pixmap)
    : QGraphicsPixmapItem(pixmap, nullptr)
{

}

GraphicsPixmapItem::~GraphicsPixmapItem()
{
    prepareGeometryChange();
}

void GraphicsPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

//    const QTransform ts = painter->transform();
    QGraphicsPixmapItem::paint(painter, option, widget);
    return;
//    if (ts.type() == QTransform::TxScale && ts.m11() < 1) {
//        painter->setRenderHint(QPainter::SmoothPixmapTransform,
//                               (transformationMode() == Qt::SmoothTransformation));

//        QPixmap pixmap;

//        if (qIsNull(cachePixmap.first - ts.m11())) {
//            pixmap = cachePixmap.second;
//        } else {
//            pixmap = this->pixmap().transformed(painter->transform(), transformationMode());
//            cachePixmap = qMakePair(ts.m11(), pixmap);
//        }

//        pixmap.setDevicePixelRatio(painter->device()->devicePixelRatioF());
//        painter->resetTransform();
//        painter->drawPixmap(offset() + QPointF(ts.dx(), ts.dy()), pixmap);
//        painter->setTransform(ts);
//    } else {
//        QGraphicsPixmapItem::paint(painter, option, widget);
//    }
}
