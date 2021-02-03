#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include "application.h"
#include "controller/signalmanager.h"
#include "dbmanager/dbmanager.h"
#include "thumbnail/thumbnaillistview.h"
#include "widgets/timelinelist.h"
#include "widgets/timelineitem.h"
#include "importview/importview.h"
#include "searchview/searchview.h"
#include "widgets/statusbar.h"
#include "allpicview/allpicview.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <DListWidget>
#include <DStackedWidget>
#include <DCommandLinkButton>
#include <DApplicationHelper>
#include <QGraphicsOpacityEffect>

class Title : public QWidget
{
public:
    Title() {}
protected:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        qDebug() << "x is " << x();
        qDebug() << "pos.x is " << pos().x();
    }
    void moveEvent(QMoveEvent *event)
    {
        Q_UNUSED(event);
        qDebug() << "moveEvent x is " << x();
        qDebug() << "moveEvent pos.x is " << pos().x();
    }

};
class TimeLineView : public DWidget, public ImageEngineImportObject
{
public:
    TimeLineView();
    ~TimeLineView() override
    {
        void clearAndStop();
    }

    bool imageImported(bool success) override
    {
        Q_UNUSED(success);
        emit dApp->signalM->closeWaitDialog();
        return true;
    }
    void updateStackedWidget();
    int getIBaseHeight();
public slots:
    void on_AddLabel(QString date, QString num);
    void on_DelLabel();
#if 1
    void on_MoveLabel(int y, QString date, QString num, QString choseText);
#endif
    void on_KeyEvent(int key);

protected:
    void resizeEvent(QResizeEvent *ev) override;

private:
    void initTimeLineViewWidget();
    void initConnections();
    void sigImprotPicsIntoThumbnailView();
    void getImageInfos();
//    void updataLayout();
    void clearAndStop();
    void clearAndStartLayout();
    void addTimelineLayout();
    void initMainStackWidget();
    void onKeyDelete();
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;

    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;

    void mousePressEvent(QMouseEvent *e) override;
public:
    void updatePicNum();
    void updateChoseText();

    void restorePicNum();
    void themeChangeSlot(DGuiApplicationHelper::ColorType themeType);

private slots:
    //更新布局（旋转图片时）
    void updataLayout(QStringList updatePathList);
private:
    TimelineList *m_mainListWidget;
    QLayout *m_mainLayout;
    QList<QString> m_timelines;
    QWidget *m_dateItem;
    DCommandLinkButton *pSuspensionChose;
    QWidget *pTimeLineViewWidget;
    ImportView *pImportView;
    int allnum;
    DLabel *m_pDate;
    DLabel *pNum_up;
    DLabel *pNum_dn;

    QList<ThumbnailListView *> m_allThumbnailListView;
    QList<DCommandLinkButton *> m_allChoseButton;

    QGraphicsOpacityEffect *m_oe;
    QGraphicsOpacityEffect *m_oet;

    bool m_ctrlPress;

    int lastClickedIndex;
    int lastRow;
    bool lastChanged;
    QWidget *fatherwidget;

public:
    QStackedWidget *m_pStackedWidget;
    StatusBar *m_pStatusBar;
    SearchView *pSearchView;

    QWidget *m_pwidget;

    int m_index;
    int m_selPicNum;
    DSpinner *m_spinner;
    int currentTimeLineLoad;
};

#endif // TIMELINEVIEW_H
