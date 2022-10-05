#include "login/dialog_newplaylist_onRose.h"

#include "common/gscommon.h"

#include "widget/toastmsg.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QStyle>


namespace Dialog{

    Dialog_NewPlaylist_OnRose::Dialog_NewPlaylist_OnRose(QWidget *parent)
        : QDialog(parent)
    {
        setInit();
        setUIControl();
    }


    Dialog_NewPlaylist_OnRose::~Dialog_NewPlaylist_OnRose(){

        this->deleteLater();
    }


    void Dialog_NewPlaylist_OnRose::setInit(){

        this->setModal(true);
        this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        this->setAttribute(Qt::WA_TranslucentBackground);
        this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
        this->setStyleSheet("background-color:transparent;");
    }


    void Dialog_NewPlaylist_OnRose::setUIControl(){

        QWidget *widget_total = new QWidget();
        widget_total->setObjectName("widget_total");
        widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");
        widget_total->setFixedSize(DLG_WIDTH, DLG_HEIGHT);

        // 다이얼로그 타이틀
        QLabel *lb_title = new QLabel();
        lb_title->setText(tr("Add a New Playlist"));
        lb_title->setStyleSheet("font:bold;color:#FFFFFF;font-size:22px;");
        lb_title->setAlignment(Qt::AlignCenter);
        lb_title->setFixedHeight(70);

        // X 버튼
        QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":/images/icon_close_gray.png");
        btn_close->setCursor(Qt::PointingHandCursor);
        btn_close->setFixedSize(QSize(70,70));
        btn_close->setParent(lb_title);
        btn_close->setGeometry(530, 0, 0, 0);
        btn_close->show();

        QHBoxLayout *box_title = new QHBoxLayout;
        box_title->setContentsMargins(0,0,0,0);
        box_title->setSpacing(0);
        box_title->addWidget(lb_title);

        QWidget *widget_title = new QWidget(widget_total);
        widget_title->setObjectName("widget_title");
        widget_title->setStyleSheet("#widget_title {background-color:transparent;border-bottom:1px solid #404040;}");
        widget_title->setFixedSize(600, 70);
        widget_title->setGeometry(0, 0, 0, 0);
        widget_title->setLayout(box_title);


        QString tmp_styleLineEdit = "QLineEdit{font-size:20px;color:#FFFFFF;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;}";
        tmp_styleLineEdit += " QLineEdit[text=\"\"]{color:#777777;}";

        QWidget *widget_body = new QWidget(widget_total);
        widget_body->setObjectName("widget_body");
        widget_body->setStyleSheet("#widget_body {background-color:transparent;border-bottom:1px solid #404040;}");
        widget_body->setFixedSize(600, 210);
        widget_body->setGeometry(0, 100, 0, 0);

        this->edit_title = new QLineEdit(widget_body);
        this->edit_title->setMaxLength(50);
        this->edit_title->setPlaceholderText(tr("Playlist Name"));
        this->edit_title->setFixedSize(550, 50);
        this->edit_title->setStyleSheet(tmp_styleLineEdit);
        this->edit_title->setGeometry(25, 0, 0, 0);

        this->edit_description = new QLineEdit(widget_body);
        this->edit_description->setMaxLength(100);
        this->edit_description->setPlaceholderText(tr("Playlist Description"));
        this->edit_description->setFixedSize(550, 50);
        this->edit_description->setStyleSheet(tmp_styleLineEdit);
        this->edit_description->setGeometry(25, 60, 0, 0);

        QPushButton *btn_ok = new QPushButton(widget_body);
        btn_ok->setText(tr("Confirm"));
        btn_ok->setCursor(Qt::PointingHandCursor);
        btn_ok->setStyleSheet("QPushButton{background:transparent; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777;border-radius:20px;} "
                              "QPushButton:hover{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;}");
        btn_ok->setFixedSize(140,40);
        btn_ok->setGeometry(135, 140, 0, 0);

        QPushButton *btn_cancel = new QPushButton(widget_body);
        btn_cancel->setText(tr("Cancel"));
        btn_cancel->setCursor(Qt::PointingHandCursor);
        btn_cancel->setStyleSheet("QPushButton{background:transparent; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777;border-radius:20px;} "
                                  "QPushButton:hover{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;}");
        btn_cancel->setFixedSize(140,40);
        btn_cancel->setGeometry(325, 140, 0, 0);

        QVBoxLayout *vbox_total = new QVBoxLayout();
        vbox_total->setContentsMargins(0,0,0,0);
        vbox_total->setSpacing(0);
        vbox_total->addWidget(widget_total);

        this->setLayout(vbox_total);

        connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
        connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));
        connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clicked_OK()));
        connect(this->edit_title, &QLineEdit::textChanged, [=]{ style()->polish(this->edit_title);});
        connect(this->edit_description, &QLineEdit::textChanged, [=]{ style()->polish(this->edit_description);});
    }


    bool Dialog_NewPlaylist_OnRose::checkUserInput(){

        bool flagOk = true;

        if(this->edit_title->text().isEmpty()){
            flagOk = false;
            ToastMsg::show(this, "", tr("Please enter a playlist name."));
        }

        return flagOk;
    }


    void Dialog_NewPlaylist_OnRose::slot_clicked_OK(){

        if(checkUserInput()){
            setResult(QDialog::Accepted);
            hide();
        }
    }

    QString Dialog_NewPlaylist_OnRose::getTitle(){

        QString title = this->edit_title->text();
        return title;
    }


    QString Dialog_NewPlaylist_OnRose::getDestription(){

        QString description = this->edit_description->text();
        return description;
    }
}
