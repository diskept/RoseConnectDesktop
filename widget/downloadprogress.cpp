#include "downloadprogress.h"
#include "common/gscommon.h"
#include <QVariantAnimation>
#include <QMovie>
#include <QThread>
#include <QDebug>

/**
 * @brief DownloadProgress::DownloadProgress : DB 다운로드 프로그레스
 * @param p_max qint64 DB FILE SIZE
 * @param parent
 */
DownloadProgress::DownloadProgress(qint64 p_max, QWidget *parent)
    : QWidget(parent)
    ,currentValue(0)
    ,maxValue(p_max)
{
    setInit();
    setUIControl();
    this->setMinimumWidth(300);
}

DownloadProgress::~DownloadProgress(){
    //delete movieProgress;

    this->deleteLater();
}

/**
 * @brief DownloadProgress::setInit : 초기 세팅
 */
void DownloadProgress::setInit(){

    this->setStyleSheet("background-color:#171717;");
}

/**
 * @brief DownloadProgress::setUIControl : UI 세팅
 */
void DownloadProgress::setUIControl(){

    this->setStyleSheet("background-color:#171717;");

    QPixmap pixmap(":/images/home_update_ico.png");
    //movieProgress = new QMovie(":images/rolling.gif");
    lb_icon = new QLabel;
    //lb_icon->setMovie(movieProgress);
    lb_icon->setPixmap(pixmap);

    lb_text = new QLabel;
    lb_text->setText(tr("DB is being updated."));
    //lb_text->setText(tr("DB를 업데이트 하는 중입니다."));
    //lb_text->setStyleSheet("background-color:transparent;font-size:18px;color:#FFFFFF;");
    lb_text->setStyleSheet("background-color:#171717;font-size:18px;color:#FFFFFF;");

    QString btn_style = "QPushButton{background-color:#B18658;font-size:18px;color:#FFFFFF;border-radius:20px;}";

    btn_refresh = new QPushButton();
    btn_refresh->setText(tr("Refresh"));
    //btn_refresh->setText(tr("새로고침"));
    btn_refresh->setStyleSheet(btn_style);
    btn_refresh->setCursor(Qt::PointingHandCursor);
    btn_refresh->setFixedSize(QSize(120,40));
    btn_refresh->hide();

    QHBoxLayout *box_text = new QHBoxLayout;
    box_text->setSpacing(0);
    box_text->setContentsMargins(0,0,0,0);
    box_text->addWidget(lb_icon,0);
    box_text->addWidget(lb_text,1);
    box_text->addWidget(btn_refresh,0);

    QString progressStyle = "QProgressBar {background-color:#4D4D4D;}";
    progressStyle += "QProgressBar::chunk:horizontal{background:qlineargradient(x1:0,y1:0.5,x2:1,y2:0.5,stop:0 green, stop:1 white);}";
    //"QProgressBar {background-color:#4D4D4D;} QProgressBar::chunk:horizontal{background-color:#B18658;}"
    progressBar = new QProgressBar;
    progressBar->setContentsMargins(0,0,0,0);
    progressBar->setFixedHeight(4);
    progressBar->setStyleSheet("QProgressBar {background-color:#4D4D4D;} QProgressBar::chunk:horizontal{background-color:#B18658;}");
    progressBar->setMaximum(maxValue);
    progressBar->setTextVisible(false);

    QWidget *widget_progress = new QWidget;
    widget_progress->setFixedSize(QSize(300,5));    //500 -> 300 fixed
    widget_progress->setStyleSheet("background-color:#171717");

    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setSpacing(0);
    box_total->setContentsMargins(0,0,0,0);
    box_total->addLayout(box_text);
    //box_total->addSpacing(5);
    //box_total->addWidget(widget_progress);
    box_total->addWidget(progressBar);

    this->setLayout(box_total);

    //connect
    connect(btn_refresh, SIGNAL(clicked()), SIGNAL(signal_clickedRefresh()));
}

void DownloadProgress::showEvent(QShowEvent *event){
    Q_UNUSED(event);
    //movieProgress->start();
    lb_text->setText(tr("DB is being updated."));
    //lb_text->setText(tr("DB를 업데이트 하는 중입니다."));
    btn_refresh->hide();
    progressBar->setValue(0);
}

void DownloadProgress::hideEvent(QHideEvent *event){
    Q_UNUSED(event);
    //movieProgress->stop();
}

/**
 * @brief DownloadProgress::setCurrentValue : 현재 값 세팅
 * @param p_currentValue
 */
void DownloadProgress::setCurrentValue(const qint64 &p_currentValue, const qint64 &p_totalValue){
    progressBar->setValue((int)((p_currentValue*maxValue) / p_totalValue));

    if(p_currentValue >= p_totalValue){
        //qDebug() << "download completed\n";
        emit signal_progressCompleted();
    }
}

void DownloadProgress::setMaxValue(const qint64 &p_maxValue){
    maxValue = p_maxValue;
    progressBar->setMaximum(maxValue);
}

void DownloadProgress::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    lb_text->setText(GSCommon::getTextCutFromLabelWidth(lb_text->text(), this->width()-40, lb_text->font()));
}

void DownloadProgress::showRefreshBtn(){
    //lb_text->setText(tr("DB download complete"));
    //lb_text->setText(tr("DB 다운로드 완료"));
    //btn_refresh->show();

    emit signal_clickedRefresh();
}
