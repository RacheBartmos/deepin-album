#include "albumdeletedialog.h"
#include "application.h"
#include "dbmanager/dbmanager.h"
#include "utils/baseutils.h"
#include <DLabel>
#include <DPushButton>
#include <DFontSizeManager>
#include "albumview/albumview.h"


AlbumDeleteDialog::AlbumDeleteDialog(DWidget *parent)
{
    Q_UNUSED(parent);
    iniUI();
    setAttribute(Qt::WA_DeleteOnClose, true);
}

void AlbumDeleteDialog::iniUI()
{
    setModal(true);

    //图标
    QIcon icon = QIcon::fromTheme("deepin-album");
    this->setIcon(icon);

    //label
    DLabel *m_label = new DLabel(this);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setText(tr("Are you sure you want to delete this album?"));

    DWidget *contentWidget = new DWidget(this);
    contentWidget->setFixedHeight(this->height() - 80);
    contentWidget->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->addStretch();
    layout->addWidget(m_label);
    layout->addStretch();

    addContent(contentWidget);

    insertButton(0, tr("Cancel"), false, DDialog::ButtonNormal);
    insertButton(1, tr("Delete"), true, DDialog::ButtonWarning);

    connect(this, &AlbumDeleteDialog::buttonClicked, this, [ = ](int index, const QString & text) {
        Q_UNUSED(text)
        if (0 == index) {
            deleteLater();
        } else if (1 == index) {
            emit deleteAlbum();
            this->close();
        }
    });
}

void AlbumDeleteDialog::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Y:
    case Qt::Key_Delete: {
        emit deleteAlbum();
        this->close();
    }
    break;
    case Qt::Key_N:
    case Qt::Key_Escape: {
        this->deleteLater();
    }
    break;
    default:
        break;
    }
}
