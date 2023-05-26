#include "setting/settingOutputUSBDAC.h"



const int DLG_WIDTH = 600;
const int DLG_HEIGHT = 100;
const int WIDGET_HEIGHT = 70;


settingOuputUSBDAC::settingOuputUSBDAC(QJsonObject jsonObj, QWidget *parent)
    : QDialog(parent)
{
    this->radio_style = "QRadioButton {background-color:transparent; border:none;}";
    this->radio_style += " QRadioButton::indicator {width:40px;height:40px;}";
    this->radio_style += " QRadioButton::indicator::unchecked {image: url(:/images/icon_che_off.png);}";
    this->radio_style += " QRadioButton::indicator::checked {image: url(:/images/icon_che_on.png);}";

    this->usbList_sytle = "QPushButton{background-color:#333333; border-bottom:1px solid #4D4D4D;}";
    this->usbList_sytle += "QPushButton:hover{background-color:#4D4D4D; border-bottom:1px solid #4D4D4D;}";

    this->jsonArray_usbDAC = QJsonArray();

    QJsonObject tmpUnuse = QJsonObject();
    tmpUnuse.insert("mProductId", 0);
    tmpUnuse.insert("mProductName", tr("Not in Use"));
    tmpUnuse.insert("mVendorId", 0);

    this->jsonArray_usbDAC = ProcJsonEasy::getJsonArray(jsonObj, "arr");
    this->jsonArray_usbDAC.insert(0, tmpUnuse);

//    for(int i = 2; i < 10; i++){
//        QJsonObject tmpUnuse = QJsonObject();
//        tmpUnuse.insert("mProductId", 0);
//        tmpUnuse.insert("mProductName", QString("Not in Use %1").arg(i - 1));
//        tmpUnuse.insert("mVendorId", 0);

//        this->jsonArray_usbDAC.insert(i, tmpUnuse);
//    }

    this->curr_usbDAC = ProcJsonEasy::getString(jsonObj, "currentName");

//    QJsonDocument doc(jsonObj);
//    QString strJson(doc.toJson(QJsonDocument::Compact));
//    qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << this->jsonArray_usbDAC;

    this->setInit();
    this->setUIControl();

    this->setData_usbDACList();
}


void settingOuputUSBDAC::setInit(){

    int count = (this->jsonArray_usbDAC.size() < 3 ? 4 : 5);
    int height = (count * WIDGET_HEIGHT) + DLG_HEIGHT;

    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("background-color:transparent;");
    this->setFixedSize(DLG_WIDTH, height);
    this->setProperty("RESULT", 0);
}


void settingOuputUSBDAC::setUIControl(){

    this->usbDACList = new QWidget();
    this->usbDACList->setStyleSheet("background-color:#333333; border:1px solid #4D4D4D;");
    this->usbDACList->setFixedSize(DLG_WIDTH, this->size().height());

    QWidget *widget_title = new QWidget(this->usbDACList);
    widget_title->setStyleSheet("background-color:#333333; border:1px solid #4D4D4D;");
    widget_title->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    widget_title->setGeometry(0, 0, 0, 0);

    QLabel *label_title = new QLabel(widget_title);
    label_title->setStyleSheet("background-color:transparent; border:none; font-size:22px; font-weight:normal; font-style:normal; line-height:0.77; text-align:center; color:#FFFFFF;");
    label_title->setText("USB DAC");
    label_title->setAlignment(Qt::AlignCenter);
    label_title->setFixedSize(DLG_WIDTH, WIDGET_HEIGHT);
    label_title->setGeometry(0, 0, 0, 0);

    QPushButton *btn_close = new QPushButton(widget_title);
    btn_close->setStyleSheet("background-color:transparent; border:none;");
    btn_close->setCursor(Qt::PointingHandCursor);
    btn_close->setFixedSize(60, 60);
    btn_close->setGeometry(530, 0, 0, 0);

    QLabel *img_close = new QLabel(btn_close);
    img_close->setStyleSheet("background-color:transparent; background-image: url(:/images/setting/icon_close_gray.png); border:none;");
    img_close->setFixedSize(60, 60);
    img_close->setGeometry(0, 0, 0, 0);


    this->vBox_usbDACList = new QVBoxLayout();
    this->vBox_usbDACList->setSpacing(0);
    this->vBox_usbDACList->setContentsMargins(0, 0, 0, 0);
    this->vBox_usbDACList->setAlignment(Qt::AlignTop);

    GSCommon::clearLayout(this->vBox_usbDACList);
    this->vBox_usbDACList->setAlignment(Qt::AlignTop);

    QWidget *widget_usbDACList = new QWidget();
    widget_usbDACList->setStyleSheet("background-color:transparent; border:none;");
    widget_usbDACList->setLayout(this->vBox_usbDACList);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(widget_usbDACList);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("QScrollBar:vertical {border: none; background-color: transparent; width: 6px; margin: 12px 0px 12px 0px; }"
                              "QScrollBar::handle:vertical {background-color: #b18658; min-height: 60px; border-radius: 3px; }"
                              "QScrollBar::add-line:vertical {height: 0 px; subcontrol-position: bottom; subcontrol-origin: margin; }"
                              "QScrollBar::sub-line:vertical {height: 0 px; subcontrol-position: top; subcontrol-origin: margin; }"
                              "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: none; }");

    QVBoxLayout *vBox_scrollArea = new QVBoxLayout();
    vBox_scrollArea->setSpacing(0);
    vBox_scrollArea->setContentsMargins(0, 0, 0, 0);
    vBox_scrollArea->setAlignment(Qt::AlignTop);
    vBox_scrollArea->addWidget(scrollArea);

    int count = (this->jsonArray_usbDAC.size() < 3 ? 3 : 4);
    int height = (count * WIDGET_HEIGHT);

    QWidget *widget_scrollArea = new QWidget(this->usbDACList);
    widget_scrollArea->setStyleSheet("background-color:transparent; border:none;");
    widget_scrollArea->setFixedSize(DLG_WIDTH - 2, height);
    widget_scrollArea->setGeometry(1, 70, 0, 0);
    widget_scrollArea->setLayout(vBox_scrollArea);


    height = (count * WIDGET_HEIGHT);

    QWidget *widget_buttonArea = new QWidget(this->usbDACList);
    widget_buttonArea->setStyleSheet("background-color:#333333; border:1px solid #4D4D4D;");
    widget_buttonArea->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    widget_buttonArea->setGeometry(0, (70 + height), 0, 0);

    QPushButton *btn_apply = new QPushButton(widget_buttonArea);
    btn_apply->setStyleSheet("background-color:#B18658; border: 1px solid #B18658; border-radius:20px;");
    btn_apply->setCursor(Qt::PointingHandCursor);
    btn_apply->setFixedSize(140, 40);
    btn_apply->setGeometry(145, 30, 0, 0);

    QLabel *label_apply = new QLabel(btn_apply);
    label_apply->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:1.39; text-align:center; color:#FFFFFF;");
    label_apply->setText(tr("Apply"));
    label_apply->setAlignment(Qt::AlignCenter);
    label_apply->setFixedSize(140, 37);
    label_apply->setGeometry(0, 0, 0, 0);

    QPushButton *btn_cancel = new QPushButton(widget_buttonArea);
    btn_cancel->setStyleSheet("background-color:#333333; border: 1px solid #777777; border-radius:20px;");
    btn_cancel->setCursor(Qt::PointingHandCursor);
    btn_cancel->setFixedSize(140, 40);
    btn_cancel->setGeometry(315, 30, 0, 0);

    QLabel *label_cancel = new QLabel(btn_cancel);
    label_cancel->setStyleSheet("background-color:transparent; border:none; font-size:18px; font-weight:normal; font-style:normal; line-height:1.39; text-align:center; color:#FFFFFF;");
    label_cancel->setText(tr("Cancel"));
    label_cancel->setAlignment(Qt::AlignCenter);
    label_cancel->setFixedSize(140, 37);
    label_cancel->setGeometry(0, 0, 0, 0);

    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));

    connect(btn_apply, SIGNAL(clicked()), this, SLOT(slot_clickedApply()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));


    QVBoxLayout *vBox_usbDAC_total= new QVBoxLayout();
    vBox_usbDAC_total->setContentsMargins(0, 0, 0, 0);
    vBox_usbDAC_total->setSpacing(0);
    vBox_usbDAC_total->setAlignment(Qt::AlignTop);
    vBox_usbDAC_total->addWidget(this->usbDACList);

    this->setLayout(vBox_usbDAC_total);
}


void settingOuputUSBDAC::setData_usbDACList(){

    for(int i = 0; i < this->jsonArray_usbDAC.size(); i++){
        QJsonObject tmpData = this->jsonArray_usbDAC.at(i).toObject();

        QString usbDAC = ProcJsonEasy::getString(tmpData, "mProductName");

        QPushButton *btn_usbDAC_Info = new QPushButton();
        btn_usbDAC_Info->setProperty("IDX", i);
        btn_usbDAC_Info->setStyleSheet(this->usbList_sytle);
        btn_usbDAC_Info->setFixedSize(DLG_WIDTH - 2, WIDGET_HEIGHT);

        connect(btn_usbDAC_Info, SIGNAL(clicked()), this, SLOT(slot_clickedBtnRadio()));

        this->btnUSBDAC[i] = new QRadioButton(btn_usbDAC_Info);
        this->btnUSBDAC[i]->setProperty("IDX", i);
        this->btnUSBDAC[i]->setStyleSheet(this->radio_style);
        this->btnUSBDAC[i]->setCursor(Qt::PointingHandCursor);
        this->btnUSBDAC[i]->setFixedSize(DLG_WIDTH - 22, WIDGET_HEIGHT);
        this->btnUSBDAC[i]->setGeometry(20, 0, 0, 0);
        this->btnUSBDAC[i]->setChecked(false);

        if(this->curr_usbDAC == usbDAC){
            this->btnUSBDAC[i]->setChecked(true);
        }

        connect(this->btnUSBDAC[i], SIGNAL(clicked()), this, SLOT(slot_clickedBtnRadio()));

        QLabel *label_usbDAC = new QLabel(this->btnUSBDAC[i]);
        label_usbDAC->setStyleSheet("background-color:transparent; border:none; font-size:20px; font-weight:normal; font-style:normal; line-height:0.85; text-align:center; color:#CCCCCC;");
        label_usbDAC->setText(usbDAC);
        label_usbDAC->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        label_usbDAC->setFixedSize(DLG_WIDTH - 115, 40);
        label_usbDAC->setGeometry(55, 15, 0, 0);

        this->vBox_usbDACList->addWidget(btn_usbDAC_Info);
    }
}


void settingOuputUSBDAC::slot_clickedBtnRadio(){

    int idx = sender()->property("IDX").toInt();

    for(int i = 0; i < this->jsonArray_usbDAC.size(); i++){
        this->btnUSBDAC[i]->setChecked(false);
    }

    this->btnUSBDAC[idx]->setChecked(true);
}


void settingOuputUSBDAC::slot_clickedApply(){

    int mProductId = 0;
    QString mProductName = "";
    int mVendorId = 0;

    for(int i = 0; i < this->jsonArray_usbDAC.size(); i++){
        if(this->btnUSBDAC[i]->isChecked()){
            QJsonObject tmpData = this->jsonArray_usbDAC.at(i).toObject();

            mProductId = ProcJsonEasy::getInt(tmpData, "mProductId");
            mProductName = ProcJsonEasy::getString(tmpData, "mProductName");
            mVendorId = ProcJsonEasy::getInt(tmpData, "mVendorId");

            emit signal_loading_stateConfig(mProductId, mProductName, mVendorId);

            this->setProperty("IDX", i);
            this->setProperty("RESULT", 1);
            break;
        }
    }

    this->setResult(QDialog::Accepted);
    this->close();
}
