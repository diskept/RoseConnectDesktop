#include "toastmsg.h"
#include "common/global.h"

#include <QApplication>
#include <QMessageBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QPropertyAnimation>

const int MSG_DURATION = 2000;


ToastMsg::ToastMsg(QWidget *parent) : QWidget(parent)
{

}


/**
 * @brief ToastMsg::show 안드로이드 Toast 와 같은 메시지 알림창을 띄우고 자동으로 사라진다.
 * @param parent
 * @param p_title 메시지의 타이틀 Text
 * @param p_msgText 메시지 내용 Text
 */
void ToastMsg::show(QWidget *parent, const QString &p_title, const QString &p_msgText){

    Q_UNUSED(p_title);
    //print_debug();
    qDebug() << "-----------------------------------------------------------------------------------------------------" << p_msgText;
    //QString ddd = parent;
    if(global.window_activate_flag) {//c221001_1
        print_debug()
    }else {
        if(parent->isModal() == true || parent->isWindowType()==true){
            print_debug();
        }else{
            print_debug();
            return;
        }
        //return;
    }
    if(global.powerDialogShowFlag) return;
//print_debug();
    QLabel *lb_msg = new QLabel();
    lb_msg->setText(p_msgText);
    lb_msg->setAlignment(Qt::AlignCenter);

    QHBoxLayout *hl_msgBox = new QHBoxLayout();
    hl_msgBox->setContentsMargins(0,0,0,0);
    hl_msgBox->setSpacing(0);
    hl_msgBox->setContentsMargins(35,20,35,15);
    hl_msgBox->addStretch(1);
    hl_msgBox->addWidget(lb_msg);
    hl_msgBox->addStretch(1);
    QWidget *widget_msgBox = new QWidget();
    widget_msgBox->setStyleSheet("background-color:#B18658;color:#FFFFFF;font-size:20px;border-radius:5px;");
    widget_msgBox->setLayout(hl_msgBox);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_msgBox, 0, Qt::AlignCenter);


    QDialog *dialog = new QDialog(parent);
    dialog->setStyleSheet("background-color:#55FF0000;border-radius:20px;");


    //dialog->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);//c221001_1
    dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);//c221001_1
    dialog->setAttribute(Qt::WA_TranslucentBackground);
    dialog->setLayout(vl_total);

    int duration = MSG_DURATION;
    QPropertyAnimation* anim_hide = new QPropertyAnimation(dialog, "windowOpacity");
    anim_hide->setStartValue(1.0);
    anim_hide->setEndValue(0.3);
    anim_hide->setDuration(duration);
    anim_hide->start(QAbstractAnimation::DeleteWhenStopped);

    dialog->show();
    dialog->raise();
    //dialog->activateWindow();
    //dialog->move(dialog->pos().x(), (global.height_mainwindow - dialog->height() - 100));//c220804
    int left = global.left_mainwindow+global.width_mainwindow/2- (dialog->sizeHint().width() / 2);//c220804
    int top = global.top_mainwindow+global.height_mainwindow - 50 ;//- (dialog->sizeHint().height() / 2);//c220804
    //int top = global.sectionBottomBottom - 100 ;//- (dialog->sizeHint().height() / 2);//c22080
    dialog->move(left, top);//c220804

    QTimer::singleShot(duration, dialog, SLOT(hide()));
}


/**
 * @brief ToastMsg::show 안드로이드 Toast 와 같은 메시지 알림창을 띄우고 자동으로 사라진다.
 * @param parent
 * @param p_title 메시지의 타이틀 Text
 * @param p_msgText 메시지 내용 Text
 */
void ToastMsg::show(QWidget *parent, const QString &p_title, const QString &p_msgText, int duration){

    Q_UNUSED(p_title);
qDebug() << "-----------------------------------------------------------------------------------------------------" << p_msgText;
    //print_debug()

    if(global.window_activate_flag) {//c221001_1
        print_debug()
    }else {
        if(parent->isModal()==true || parent->isWindowType()==true){
            print_debug();
        }else{
            print_debug();
            return;
        }
        //return;
    }
    if(global.powerDialogShowFlag) return;

    //print_debug();
    QLabel *lb_msg = new QLabel();
    lb_msg->setText(p_msgText);
    lb_msg->setAlignment(Qt::AlignCenter);

    QHBoxLayout *hl_msgBox = new QHBoxLayout();
    hl_msgBox->setContentsMargins(0,0,0,0);
    hl_msgBox->setSpacing(0);
    hl_msgBox->setContentsMargins(35,20,35,15);
    hl_msgBox->addStretch(1);
    hl_msgBox->addWidget(lb_msg);
    hl_msgBox->addStretch(1);
    QWidget *widget_msgBox = new QWidget();
    widget_msgBox->setStyleSheet("background-color:#B18658;color:#FFFFFF;font-size:20px;border-radius:5px;");
    widget_msgBox->setLayout(hl_msgBox);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_msgBox, 0, Qt::AlignCenter);


    QDialog *dialog = new QDialog(parent);
    //dialog->setFixedSize(200,100);
    dialog->setStyleSheet("background-color:#55FF0000;border-radius:20px;");


    //dialog->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);//c221001_1
    dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);//c221001_1
    dialog->setAttribute(Qt::WA_TranslucentBackground);
    dialog->setLayout(vl_total);

    // int duration = MSG_DURATION;
    QPropertyAnimation* anim_hide = new QPropertyAnimation(dialog, "windowOpacity");
    anim_hide->setStartValue(1.0);
    anim_hide->setEndValue(0.3);
    anim_hide->setDuration(duration);
    anim_hide->start(QAbstractAnimation::DeleteWhenStopped);

    dialog->show();
    dialog->raise();
    int left = global.left_mainwindow+global.width_mainwindow/2- (dialog->sizeHint().width() / 2);//c220818
    int top = global.top_mainwindow+global.height_mainwindow - 50 ;//- (dialog->sizeHint().height() / 2);//c220804
    //int top = global.sectionBottomBottom - 100 ;
    //qDebug() << left << ", " << top;

    dialog->move(left, top);//c220818
    QTimer::singleShot(duration, dialog, SLOT(hide()));
}


/**
 * @brief ToastMsg::show 안드로이드 Toast 와 같은 메시지 알림창을 띄우고 자동으로 사라진다.
 * @param parent
 * @param p_title 메시지의 타이틀 Text
 * @param p_msgText 메시지 내용 Text
 */
void ToastMsg::show(QWidget *parent, const QString &p_title, const QString &p_msgText, int duration, int line2){

    Q_UNUSED(p_title);

qDebug() << "-----------------------------------------------------------------------------------------------------" << p_msgText;
    if(global.pretoastmsg == p_msgText) return;
    global.pretoastmsg = p_msgText;
    //print_debug();
    if(global.window_activate_flag) {//c221001_1
        print_debug();
    }else {
        if(parent->isModal()==true || parent->isWindowType()==true){
            print_debug();
        }else{
            print_debug();
            return;
        }
        //return;
    }
    if(global.powerDialogShowFlag) return;
    //print_debug();
    QLabel *lb_msg = new QLabel();
    lb_msg->setText(p_msgText);
    lb_msg->setAlignment(Qt::AlignCenter);

    QHBoxLayout *hl_msgBox = new QHBoxLayout();
    hl_msgBox->setContentsMargins(0,0,0,0);
    hl_msgBox->setSpacing(0);
    hl_msgBox->setContentsMargins(35,20,35,15);
    hl_msgBox->addStretch(1);
    hl_msgBox->addWidget(lb_msg);
    hl_msgBox->addStretch(1);
    QWidget *widget_msgBox = new QWidget();
    widget_msgBox->setStyleSheet("background-color:#B18658;color:#FFFFFF;font-size:20px;border-radius:5px;");
    widget_msgBox->setLayout(hl_msgBox);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_msgBox, 0, Qt::AlignCenter);


    QDialog *dialog = new QDialog(parent);
    dialog->setStyleSheet("background-color:#55FF0000;border-radius:20px;");


    dialog->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);//c221001_1
    //dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);//c221001_1
    dialog->setAttribute(Qt::WA_TranslucentBackground);
    dialog->setLayout(vl_total);

    // int duration = MSG_DURATION;
    QPropertyAnimation* anim_hide = new QPropertyAnimation(dialog, "windowOpacity");
    anim_hide->setStartValue(1.0);
    anim_hide->setEndValue(0.3);
    anim_hide->setDuration(duration);
    anim_hide->start(QAbstractAnimation::DeleteWhenStopped);

    dialog->show();
    dialog->raise();
    int left = global.left_mainwindow+global.width_mainwindow*0.3;//c220818
    int left2 = global.left_mainwindow+global.width_mainwindow*0.6-50;//c220818
    int top = global.top_mainwindow+global.height_mainwindow - 50 ;//- (dialog->sizeHint().height() / 2);//c220804
    //int top = global.sectionBottomBottom - 100 ;
    if(line2 != 0){
        dialog->move(left2, top);//c220908_2
    }else{
        dialog->move(left, top);//c220908_2
    }

    QTimer::singleShot(duration, dialog, SLOT(hide()));
}

void ToastMsg::show(QWidget *parent, const QString &p_title, const QString &p_msgText, int duration, int line2, int forceflag){//c220930_2

    Q_UNUSED(p_title);
qDebug() << "-----------------------------------------------------------------------------------------------------" << p_msgText;
    if(global.pretoastmsg == p_msgText) return;
    global.pretoastmsg = p_msgText;
    if(forceflag < 0){

    }else{
        if(global.window_activate_flag) {//c221001_1
            print_debug();
        }else {
            if(parent->isModal()==true || parent->isWindowType()==true){
                print_debug();
            }else{
                print_debug();
                return;
            }
            //return;
        }
    }

    if(global.powerDialogShowFlag) return;
    //print_debug();
    QLabel *lb_msg = new QLabel();
    lb_msg->setText(p_msgText);
    lb_msg->setAlignment(Qt::AlignCenter);

    QHBoxLayout *hl_msgBox = new QHBoxLayout();
    hl_msgBox->setContentsMargins(0,0,0,0);
    hl_msgBox->setSpacing(0);
    hl_msgBox->setContentsMargins(35,20,35,15);
    hl_msgBox->addStretch(1);
    hl_msgBox->addWidget(lb_msg);
    hl_msgBox->addStretch(1);
    QWidget *widget_msgBox = new QWidget();
    widget_msgBox->setStyleSheet("background-color:#B18658;color:#FFFFFF;font-size:20px;border-radius:5px;");
    widget_msgBox->setLayout(hl_msgBox);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_msgBox, 0, Qt::AlignCenter);


    QDialog *dialog = new QDialog(parent);
    dialog->setStyleSheet("background-color:#55FF0000;border-radius:20px;");


    //dialog->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);//c221001_1
    dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);//c221001_1
    dialog->setAttribute(Qt::WA_TranslucentBackground);
    dialog->setLayout(vl_total);

    // int duration = MSG_DURATION;
    QPropertyAnimation* anim_hide = new QPropertyAnimation(dialog, "windowOpacity");
    anim_hide->setStartValue(1.0);
    anim_hide->setEndValue(0.3);
    anim_hide->setDuration(duration);
    anim_hide->start(QAbstractAnimation::DeleteWhenStopped);

    dialog->show();
    dialog->raise();
    int left = global.left_mainwindow+global.width_mainwindow*0.3;//c220818
    int left2 = global.left_mainwindow+global.width_mainwindow*0.6-50;//c220818
    int top = global.top_mainwindow+global.height_mainwindow - 50 ;//- (dialog->sizeHint().height() / 2);//c220804
    //int top = global.sectionBottomBottom - 100 ;
    if(line2 != 0){
        dialog->move(left2, top);//c220908_2
    }else{
        dialog->move(left, top);//c220908_2
    }


    QTimer::singleShot(duration, dialog, SLOT(hide()));
}


/*void ToastMsg::slot_message_hide(){

}*/
