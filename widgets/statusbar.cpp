#include "statusbar.h"
#include <QGraphicsDropShadowEffect>
#include <QItemSelectionModel>

StatusBar::StatusBar(QWidget *parent)
    : DBlurEffectWidget(parent)
{

//    QPalette palette;
//    palette.setColor(QPalette::Background, QColor(0, 0, 0, 0)); // 最后一项为透明度
//    setPalette(palette);
    initUI();
   // setMaskColor(MaskColorType::CustomColor);
   // setMaskAlpha(0.7);

}

void StatusBar::initUI()
{
//    setFixedHeight(27);
    setFixedHeight(27);

//    QString str = QObject::tr("%1 photo(s)");
    m_allPicNum = DBManager::instance()->getImgsCount();

    m_pAllPicNumLabel = new DLabel();
    m_pAllPicNumLabel->setEnabled(false);
//    m_pAllPicNumLabel->setText(str.arg(QString::number(m_allPicNum)));
    m_pAllPicNumLabel->setFont(DFontSizeManager::instance()->get(DFontSizeManager::T8,QFont::Normal));
    m_pAllPicNumLabel->setAlignment(Qt::AlignCenter);

    m_pimporting = new DWidget(this);
    TextLabel = new DLabel();
    TextLabel->setFont(DFontSizeManager::instance()->get(DFontSizeManager::T8));

    TextLabel->setText("");
    TextLabel->adjustSize();
    TextLabel->setEnabled(false);
    loadingicon = new DSpinner(m_pimporting);
    loadingicon->hide();
    loadingicon->setFixedSize(20, 20);

    m_pStackedWidget = new DStackedWidget(this);
    m_pStackedWidget->addWidget(m_pAllPicNumLabel);
    m_pStackedWidget->addWidget(TextLabel);

    m_pSlider = new DSlider(Qt::Horizontal, this);
    m_pSlider->setFixedWidth(180);
    m_pSlider->setFixedHeight(27);
    m_pSlider->setMinimum(0);
    m_pSlider->setMaximum(9);
    m_pSlider->slider()->setSingleStep(1);
    m_pSlider->slider()->setTickInterval(1);
    m_pSlider->setValue(2);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout();
    pHBoxLayout->setContentsMargins(0, 0, 0, 3);
    pHBoxLayout->addWidget(m_pStackedWidget, Qt::AlignCenter);
    this->setLayout(pHBoxLayout);

    initConnections();
}

void StatusBar::initConnections()
{
    qRegisterMetaType<QStringList>("QStringList &");
    connect(dApp->signalM, &SignalManager::updateStatusBarImportLabel, this, [ = ](QStringList paths, int count) {
        if (isVisible()) {
            imgpaths = paths;
            pic_count = count;

            QString string = tr("Importing photos: '%1'");
            TextLabel->setAlignment(Qt::AlignCenter);
            TextLabel->setText(string.arg(imgpaths[0]));
            TextLabel->adjustSize();

            m_pStackedWidget->setCurrentIndex(1);
//                loadingicon->move(TextLabel->x()+102, 0);
//                loadingicon->show();
//                loadingicon->start();
            interval = startTimer(3);

        }
    });
    connect(dApp->signalM, &SignalManager::sigExporting, this, [ = ](QString path) {
        if (isVisible()) {
            m_pStackedWidget->setCurrentIndex(1);
            QString string = tr("Exporting photos: '%1'");
            TextLabel->setAlignment(Qt::AlignCenter);
            TextLabel->setText(string.arg(path));
            TextLabel->adjustSize();
            QTime time;
            time.start();
            while (time.elapsed() < 5)
                QCoreApplication::processEvents();
        }
    });
    connect(dApp->signalM, &SignalManager::sigExporting, this, [ = ](QString path) {
        Q_UNUSED(path);
        if (isVisible()) {
            m_pStackedWidget->setCurrentIndex(0);
        }
    });
}

void StatusBar::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    m_pSlider->move(width() - 214, -1);
}

void StatusBar::paintEvent(QPaintEvent *event)
{
    setMaskColor(MaskColorType::AutoColor);

    QPalette palette=m_pAllPicNumLabel->palette();
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::DarkType) {
        palette.setColor(QPalette::WindowText, QColor(192,198,212)); // 最后一项为透明度
    }
    else{
        palette.setColor(QPalette::WindowText, QColor(98,110,136)); // 最后一项为透明度
    }

    m_pAllPicNumLabel->setPalette(palette);
    return DBlurEffectWidget::paintEvent(event);
}





void StatusBar::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == interval) {
        loadingicon->move(TextLabel->x() + 102, 0);

//        qDebug()<<TextLabel->x();
        m_pStackedWidget->setCurrentIndex(1);


        QString string = tr("Importing photos: '%1'");
//        TextLabel->setAlignment(Qt::AlignCenter);
//        TextLabel->adjustSize();

        if (imgpaths.count() == 1) {
            i = 0;
            killTimer(interval);
            interval = 0;
            m_pStackedWidget->setCurrentIndex(0);
            if (1 == pic_count) {
                emit dApp->signalM->ImportSuccess();
            } else {
                emit dApp->signalM->ImportFailed();
            }
        } else {
            TextLabel->setText(string.arg(imgpaths[i + 1]));
//            TextLabel->setMinimumSize(TextLabel->sizeHint());
//            TextLabel->adjustSize();       
            TextLabel->setFont(DFontSizeManager::instance()->get(DFontSizeManager::T8,QFont::Normal));
            i ++;
            if (i == imgpaths.count() - 1) {
                i = 0;
                killTimer(interval);
                interval = 0;

                if (1 == pic_count) {
                    emit dApp->signalM->ImportSuccess();
                } else {
                    emit dApp->signalM->ImportFailed();
                }

                QTime time;
                time.start();
                while (time.elapsed() < 500)
                    QCoreApplication::processEvents();

                m_pStackedWidget->setCurrentIndex(0);
            }
        }
    }
}

