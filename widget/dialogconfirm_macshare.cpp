#include "dialogconfirm_macshare.h"
#include "common/gscommon.h"
#include "common/global.h"//c220913_1
#include <QScrollBar>//c220913_2
#include "widget/toastmsg.h"

const int DLG_WIDTH = 600;

/**
 * @brief DialogConfirm_MacShare::DialogConfirm_MacShare : Confirm Dialog
 * @details 제목, 내용으로 구성
 * @param parent
 * @param p_confirmType
 * @note {@link setAlertMode()} 함수로 Alert 스타일 사용 가능
 *
 */
DialogConfirm_MacShare::DialogConfirm_MacShare(QWidget *parent, DlgConfirmType p_confirmType)//c220913_1
    : QDialog(parent)
{
    this->confirmType = p_confirmType;

    setInit();
    setUIControl();
}

/**
 * @brief DialogConfirm_MacShare::setInit : 초기 세팅
 */
void DialogConfirm_MacShare::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedWidth(DLG_WIDTH);
    this->setStyleSheet("background-color:transparent;");
    delegate = new SharefolderDelegate(this);//c220913_1
    this->linker = Linker::getInstance();//c220913_1
}

/**
 * @brief DialogConfirm_MacShare::setUIControl : UI 세팅
 */
void DialogConfirm_MacShare::setUIControl(){
    lb_title = new QLabel;
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setStyleSheet("color:#FFFFFF;font-size:22px;background-color:transparent;");
    lb_title->setFixedHeight(70);

    QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":/images/icon_close_gray.png");
    btn_close->setParent(this);
    btn_close->setFixedSize(QSize(70,70));
    btn_close->setCursor(Qt::PointingHandCursor);

    /*
    QHBoxLayout *box_top = new QHBoxLayout;
    box_top->setContentsMargins(0,0,0,0);
    box_top->setSpacing(0);
    box_top->addWidget(lb_title);
    */
    QGridLayout *box_top = new QGridLayout();
    box_top->setContentsMargins(0,0,0,0);
    box_top->setSpacing(0);
    box_top->addWidget(lb_title, 0, 0);
    if(this->confirmType == DlgConfirmType::OKCancel){
    box_top->addWidget(btn_close, 0, 0, Qt::AlignRight);
    }

    QWidget *wg_top = new QWidget;
    wg_top->setObjectName("wg_top");
    wg_top->setLayout(box_top);
    wg_top->setStyleSheet("#wg_top {background-color:transparent;border-bottom:1px solid #404040;}");

    lb_text = new QLabel;
    lb_text->setAlignment(Qt::AlignCenter);
    lb_text->setStyleSheet("color:#FFFFFF;font-size:20px;background-color:transparent; line-height:24px;");
    //lb_text->setContentsMargins(30,14,30,39);
    lb_text->setContentsMargins(0,0,0,0);
    lb_text->setWordWrap(true);
    //lb_text->setFixedHeight(100);//c220511

    //c220913_1
    listWidget = new QListWidget;
    listWidget->setItemDelegate(delegate);
    listWidget->setContentsMargins(0,0,0,0);
    listWidget->setFixedWidth(500);
    listWidget->setStyleSheet("background-color:#333333;border:none;border-radius:20px; " );
    listWidget->setCursor(Qt::PointingHandCursor);//c220913_2
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->verticalScrollBar()->

            setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
                                            "   width: 8px;"
                                            "   background: rgba (0,0,0,0%);"
                                            "   margin: 0px, 0px, 0px, 0px;"
                                            "   padding-top: 9px;"//set aside 9px for the arrows above and below
                                            "   padding-bottom: 9px;"
                                            "}"
                                            "QScrollBar :: handle: vertical"
                                            "{"
                                            "   width:8px;"
                                            "   background: rgba (0,0,0,25%);"
                                            "   border-radius: 4px;"//Both ends of the scroll bar become ellipses
                                            "   min-height: 20 px;"
                                            "}"
                                            " QScrollBar :: handle: vertical: hover"
                                            " {"
                                            "     width: 8px;"
                                            "     background: rgba (0,0,0,50%);"//When the mouse is placed on the scroll bar, the color becomes darker
                                            "     border-radius: 4px;"
                                            "     min-height: 20 px;"
                                            " }"));
    // 확인 버튼
    QPushButton *btn_save = new QPushButton;
    btn_save->setText(tr("Confirm"));
    //btn_save->setText(tr("확인"));
    btn_save->setObjectName("btn_save");
    btn_save->setFixedSize(140, 40);
    btn_save->setStyleSheet("#btn_save{font-size:18px; color:#FFFFFF; background-color:#B18658; border-radius:20px;} #btn_save:hover{font-size:18px; color:#FFFFFF; background-color:#7D6144; border-radius:20px;}");
    btn_save->setCursor(Qt::PointingHandCursor);

    // 취소 버튼
    btn_cancel = new QPushButton;
    btn_cancel->setText(tr("Cancel"));
    //btn_cancel->setText(tr("취소"));
    btn_cancel->setObjectName("btn_cancel");
    btn_cancel->setFixedSize(140,40);
    btn_cancel->setStyleSheet("#btn_cancel{font-size:18px; color:#FFFFFF; background-color:#777777; border-radius:20px;} #btn_cancel:hover{font-size:18px; color:#FFFFFF; background-color:#4D4D4D; border-radius:20px;}");
    btn_cancel->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_btn = new QHBoxLayout;
    box_btn->setSpacing(30);
    box_btn->setContentsMargins(0,0,0,0);
    box_btn->addStretch(1);
    box_btn->addWidget(btn_save);
    if(this->confirmType == DlgConfirmType::OKCancel){
    box_btn->addWidget(btn_cancel);
    }
    box_btn->addStretch(1);

    wg_body = new QWidget;
    wg_body->setObjectName("wg_body");
    wg_body->setStyleSheet("#wg_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");


    QVBoxLayout *box_body = new QVBoxLayout;//c220709
    box_body->setSpacing(0);
    box_body->setContentsMargins(0,0,0,0);
    box_body->setAlignment(Qt::AlignTop);
    box_body->addWidget(lb_text);
    box_body->addWidget(wg_body);
    box_body->addWidget(listWidget);//c220913_1
    //wg_body->hide();



    QVBoxLayout *box_contents = new QVBoxLayout;
    box_contents->setSpacing(0);
    box_contents->setContentsMargins(25,25,25,30);
    box_contents->setAlignment(Qt::AlignTop);
    box_contents->addLayout(box_body);
    box_contents->addSpacing(20);
    box_contents->addLayout(box_btn);

    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setSpacing(0);
    box_total->setContentsMargins(0,0,0,0);
    box_total->setAlignment(Qt::AlignTop);
    box_total->addWidget(wg_top);
    box_total->addLayout(box_contents);

    wg_total = new QWidget;
    wg_total->setObjectName("wg_total");
    wg_total->setLayout(box_total);
    wg_total->setStyleSheet("#wg_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");

    QVBoxLayout *box_wrap = new QVBoxLayout;
    box_wrap->setContentsMargins(0,0,0,0);
    box_wrap->setSpacing(0);
    box_wrap->setAlignment(Qt::AlignTop);
    box_wrap->addWidget(wg_total);

    setLayout(box_wrap);

    // 커넥션
    //connect(listWidget, SIGNAL(itemClicked()), this, SLOT(slot_itemClicked()));//c220913_1
    connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));//c220913_1
    connect(btn_save, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(reject()));
}


void DialogConfirm_MacShare::setSuggestions(QJsonObject p_json){//c220913_1

    //GSCommon::clearLayout(box_suggestions);
    listWidget->clear();
    print_debug();
    qDebug() << "p_json=" << p_json;


    QString ip_sel = ProcJsonEasy::getString(p_json, "ip");
    QString id_sel = ProcJsonEasy::getString(p_json, "id");
    QString pw_sel = ProcJsonEasy::getString(p_json, "pw");
    QJsonArray jsonShareArray = ProcJsonEasy::getJsonArray(p_json, "shareNameArr");
    QJsonArray jsonOldPathArray = ProcJsonEasy::getJsonArray(p_json, "oldPath");
    qDebug() << "jsonShareArray=" << jsonShareArray;
    qDebug() << "jsonOldPathArray=" << jsonOldPathArray;

    for(int i = 0 ; i < jsonShareArray.size(); i++){
        //print_debug();
        QJsonObject json;
        json.insert("index", i);
        json.insert("ip", ip_sel);
        json.insert("id", id_sel);
        json.insert("pw", pw_sel);
        json.insert("old", false);//c220904_1
        json.insert("suggestion", jsonShareArray.at(i).toString());

        bool old_flag = false;
        for(int j = 0 ; j < jsonOldPathArray.size() ; j ++){
            QString cmp_str = ip_sel+"-"+jsonShareArray.at(i).toString();
            if( cmp_str == jsonOldPathArray.at(j).toString() ){
                old_flag = true;
            }
        }
        json.insert("old_flag", old_flag);


        QListWidgetItem *item1 = new QListWidgetItem;
        item1->setData(Qt::UserRole, json);
        listWidget->addItem(item1);
    }


}

/**
 * @brief DialogConfirm::setTitle : 타이틀 세팅 함수
 * @param p_title
 */
void DialogConfirm_MacShare::slot_delegateClicked(const int &p_index, const int &p_btnType){//c220913_1

    if(p_btnType == SharefolderDelegate::BtnType::etc){//index = 1
        print_debug();
        qDebug() << "p_index=" << p_index;
        QListWidgetItem *item1 = listWidget->item(p_index);
        QMap<QString,QVariant> map = item1->data(Qt::UserRole).toMap();

#if defined(Q_OS_WIN)

#elif defined(Q_OS_MAC)//c220913_1
    print_debug();
    bool tmp_suggestion_old_path_flag = map["old_flag"].toBool();
    if(tmp_suggestion_old_path_flag){
        ToastMsg::show(this,"", tr("It already exists as a shared folder."), 2000, 0);
        qDebug() << "It already exists as a shared folder.";
        return;
    }else{

    }
#endif
        QString tmp_ip = map["ip"].toString();//c220913_1
        QString tmp_id = map["id"].toString();//c220913_1
        QString tmp_pw = map["pw"].toString();//c220913_1
        QString tmp_folderName = map["suggestion"].toString();//c220913_1
        qDebug() << "tmp_ip = " << tmp_ip;
        qDebug() << "tmp_id = " << tmp_id;
        qDebug() << "tmp_pw = " << tmp_pw;
        qDebug() << "tmp_folderName = " << tmp_folderName;
        emit linker->signal_goFolder("folder");//c220913_1
        emit linker->signal_setMountShareFolder(tmp_ip, tmp_id, tmp_pw, tmp_folderName);
    }
    else if(p_btnType == SharefolderDelegate::BtnType::del){//index = 0
        print_debug();
        qDebug() << "p_index=" << p_index;
        //QListWidgetItem *item1 = listWidget->item(p_index);
       // listWidget->removeItemWidget(item1);
        listWidget->takeItem(p_index);
        //linker->signal_searchBarFocusChanged(false);//c220703
        //linker->signal_searchBarFocusChanged(true);//c220703
        //proc_selOldDBSel(p_index);

    }


}

void DialogConfirm_MacShare::slot_itemClicked(){
    print_debug();
}

void DialogConfirm_MacShare::setTitle(const QString &p_title){
    lb_title->setText(p_title);
}

/**
 * @brief DialogConfirm::setText : 텍스트 세팅함수
 * @param p_text
 */
void DialogConfirm_MacShare::setText(const QString &p_text){

    wg_body->hide();
    lb_text->setText(p_text);
    lb_text->show();

}
void DialogConfirm_MacShare::setLayoutMy(QLayout *a){//c220709
    lb_text->hide();
    wg_body->setLayout(a);
    wg_body->show();

}
void DialogConfirm_MacShare::setLayoutMy_withText(QLayout *a){//c220804
    //lb_text->hide();
    wg_body->setLayout(a);
    wg_body->show();

}

void DialogConfirm_MacShare::setTextHeight(const int &h){//c220511
    lb_text->setFixedHeight(h);
}
void DialogConfirm_MacShare::setAlignment(Qt::Alignment a){//c220511
    lb_text->setAlignment(a);
}


void DialogConfirm_MacShare::yellow_setStyleSheet(){//cheon211203
    wg_total->setStyleSheet("#wg_total { background-color:rgb(227,184,138);border-radius:4px;border:1px solid #4D4D4D; }");//#333333

}
/**
 * @brief DialogConfirm::setAlertMode : Alert Mode 변환
 */
void DialogConfirm_MacShare::setAlertMode(){
    btn_cancel->hide();
}
