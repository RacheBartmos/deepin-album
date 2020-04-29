#include "leftlistwidget.h"

#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include "widgets/albumlefttabitem.h"

LeftListWidget::LeftListWidget()
{
    setViewportMargins(8, 0, 8, 0);
    setAcceptDrops(true);
}

void LeftListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QModelIndex index = this->indexAt(event->pos());
    if (index.isValid()) {
        AlbumLeftTabItem *item = (AlbumLeftTabItem *)this->itemWidget(this->item(index.row()));
        QString leftTabListName = item->m_albumNameStr;
        QString leftTabListType = item->m_albumTypeStr;
        // qDebug()<<"leftTabListName: "<<leftTabListName<<" ;leftTabListType: "<<leftTabListType;

        if ((COMMON_STR_RECENT_IMPORTED == leftTabListName)
                || (COMMON_STR_TRASH == leftTabListName)
                || (COMMON_STR_FAVORITES == leftTabListName)
                || (ALBUM_PATHTYPE_BY_PHONE == leftTabListType)
                || (ALBUM_PATHTYPE_BY_U == leftTabListType)) {
            qDebug() << "Can not drop!";
            return event->ignore();
        } else {
            return event->accept();
        }
    }
}

void LeftListWidget::dropEvent(QDropEvent *event)
{
    QModelIndex index = this->indexAt(event->pos());
    if (index.isValid()) {
        qDebug() << "emit signalDropEvent:" << index;
        emit signalDropEvent(index);
    }

    DListWidget::dropEvent(event);
}

void LeftListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "xxxxxxxxxxxxxx";
    if (event->mimeData()->hasFormat("TestListView/text-icon-icon_hover")) {
//        const QMimeData *mimeData = event->mimeData();
//        if (!utils::base::checkMimeData(mimeData)) {
//            return;
//        }
        event->acceptProposedAction();
    } else {
        event->ignore();
        QListWidget::dragEnterEvent(event);
    }
}

void LeftListWidget::mousePressEvent(QMouseEvent *e)
{
    QModelIndex index = indexAt(e->pos());
    if (!index.isValid()) {
        emit sigMousePressIsNoValid();
    }
    //    qDebug()<<this->currentRow();
    //    if (!index.isValid())
    //    {
    //        qDebug()<<"111111111"<<index;
    //    }
    //    else {
    //        qDebug()<<index.row();
    //    }

    //    AlbumLeftTabItem *item = (AlbumLeftTabItem*)this->itemWidget(this->currentItem());
    //    item->onCheckNameValid();

    DListWidget::mousePressEvent(e);
}

QStyleOptionViewItem LeftListWidget::viewOptions() const
{
    QStyleOptionViewItem option = QAbstractItemView::viewOptions();

    if (this->viewMode() == QListView::IconMode) {
        option.showDecorationSelected = false;
        option.decorationPosition = QStyleOptionViewItem::Top;
        option.displayAlignment = Qt::AlignCenter;
    } else {
        option.decorationPosition = QStyleOptionViewItem::Left;
    }

    if (this->gridSize().isValid()) {
        option.rect.setSize(this->gridSize());
    }

    return option;
}

QModelIndex LeftListWidget::getModelIndex(QListWidgetItem *pItem)
{
    return indexFromItem(pItem);
}
