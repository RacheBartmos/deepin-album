#ifndef IMPORTTIMELINEVIEW_H
#define IMPORTTIMELINEVIEW_H

#include "application.h"
#include "controller/signalmanager.h"
#include "dbmanager/dbmanager.h"
#include "thumbnail/thumbnaillistview.h"
#include "widgets/timelinelist.h"
#include "widgets/timelineitem.h"
#include "importview/importview.h"
#include "widgets/statusbar.h"
#include "allpicview/allpicview.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <DListWidget>
#include <DCommandLinkButton>
#include <DApplicationHelper>
#include <QGraphicsOpacityEffect>

class ImportTimeLineView : public DWidget, public ImageEngineImportObject
{
    Q_OBJECT
public:
    ImportTimeLineView(DWidget *parent);
    ~ImportTimeLineView() override
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
//signals:
//    void albumviewResize();
signals:
    void sigResizeTimelineBlock();
public slots:
    void on_AddLabel(QString date, QString num);
    void on_DelLabel();
#if 1
    void on_MoveLabel(int y, QString date, QString num, QString choseText);
#endif
    void on_KeyEvent(int key);

protected:
    void resizeEvent(QResizeEvent *ev) override;

    void showEvent(QShowEvent *ev) override;

private:
    void initTimeLineViewWidget();
    void initConnections();
    void sigImprotPicsIntoThumbnailView();
    void getImageInfos();

    void initMainStackWidget();
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;

    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
public:
//    void updataLayout();
    void updateLayout(QStringList updatePathList = QStringList());          //旋转图片更新视图
    void clearAndStartLayout();
    void addTimelineLayout();
    void getFatherStatusBar(DSlider *s);
    void themeChangeSlot(DGuiApplicationHelper::ColorType themeType);
    void resizeHand();  //手动计算大小

#if 1
    QStringList selectPaths();
    void updateChoseText();
#endif
private slots:
    /**
     * @brief updateSize
     * 调整已导入界面的整体大小
     */
    void updateSize();
signals:
    void sigUpdatePicNum();

private:
    void clearAndStop();
    QLayout *m_mainLayout;
    QList<QString> m_timelines;
    DWidget *m_dateItem;
    DCommandLinkButton *pSuspensionChose;
    DWidget *pTimeLineViewWidget;
    ImportView *pImportView;
    QMap<ThumbnailListView *, QStringList> selpicQmap;
    int allnum;
    DLabel *m_pDate;
    DLabel *pNum_up;
    DLabel *pNum_dn;
    DLabel *m_pImportTitle; //add 3975
    DSlider *m_DSlider;
    QList<ThumbnailListView *> m_allThumbnailListView;
    QList<DCommandLinkButton *> m_allChoseButton;

    QGraphicsOpacityEffect *m_oe;
    QGraphicsOpacityEffect *m_oet;

    bool m_ctrlPress;

    int lastClickedIndex;
    int lastRow;
    int m_lastShiftRow;
    int m_lastShiftClickedIndex;
    bool lastChanged;
    int m_iBaseHeight;

    bool m_bshow = false;
public:
    int m_index;
    TimelineList *m_mainListWidget;
    int currentTimeLineLoad;
};

#endif // IMPORTTIMELINEVIEW_H
