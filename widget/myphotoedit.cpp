#include "myphotoedit.h"

#include <qboxlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <common/filedownloader.h>

const int PHOTO_SIZE = 200;

MyPhotoEdit::MyPhotoEdit(QWidget *parent) : QWidget(parent)
{
    setUIControl();
}

/**
 * @brief MyPhotoEdit::setUIControl
 */
void MyPhotoEdit::setUIControl(){

    lb_img = new QLabel();
    lb_img->setFixedSize(PHOTO_SIZE,PHOTO_SIZE);

    QPushButton *btn_delete = new QPushButton(tr("Delete"));
    //QPushButton *btn_delete = new QPushButton(tr("삭제"));
    btn_delete->setFixedSize(PHOTO_SIZE,40);
    btn_delete->setStyleSheet("color:#FFFFFF;font-size:18px;text-align:bottom;padding-bottom:10px;background-color:#77000000;");
    btn_delete->setContentsMargins(0,100,0,0);
    btn_delete->setCursor(Qt::PointingHandCursor);

    QGridLayout *vl_total = new QGridLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(lb_img, 0, 0);
    vl_total->addWidget(btn_delete, 0, 0, Qt::AlignCenter | Qt::AlignBottom);

    // 커넥션
    connect(btn_delete, &QPushButton::clicked, this, &MyPhotoEdit::slot_deletePhoto);

    this->setLayout(vl_total);
}

/**
 * @brief MyPhotoEdit::setDataMyPhotoEdit 이미지 경로 세팅. 외부에서 호출하는 함수.
 * @param p_url
 */
void MyPhotoEdit::setDataMyPhotoEdit(QString p_url){

    if(p_url!=""){
        this->imgPath = p_url;

        FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
        connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImage()));
        tmp_fileDownLoader->setImageURL(QUrl(p_url));
    }
}


/**
 * @brief MyPhotoEdit::slot_loadImage [SLOT] 이미지 세팅
 */
void MyPhotoEdit::slot_loadImage(){

    FileDownloader *tmp_fileDownloader = qobject_cast<FileDownloader*>(sender());

    QImage image;
    bool flagLoad = image.loadFromData(tmp_fileDownloader->downloadedData());
    if(flagLoad){
        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        tmp_pixmap = tmp_pixmap.scaled(PHOTO_SIZE, PHOTO_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        this->lb_img->setPixmap(tmp_pixmap);
    }
    tmp_fileDownloader->deleteLater();
}

/**
 * @brief MyPhotoEdit::slot_deletePhoto [SLOT] 삭제 버튼 클릭시
 */
void MyPhotoEdit::slot_deletePhoto(){

    emit signal_deletePhoto(this->imgPath);
}

