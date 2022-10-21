#include "onoffwidget.h"
#include <QPainter>
#include <QThread>

const QString COLOR_BG_ON = "#B18658";
const QString COLOR_BG_OFF = "#787878";
const int CIRCLE_SIZE = 34;     // 설정 VU옵션 기준
const int TOTALAREA_WIDTH = 74; // 설정 VU옵션 기준
/**
 * @brief OnOffWidget::OnOffWidget : On, Off Widget
 * @details 아이폰 스타일의 On, Off 토글
 *      {@link setValue(bool)} 함수로 On, Off 세팅
 *      {@link signal_changeD(bool)} 시그널로 변경내용 수신
 * @param parent
 */
OnOffWidget::OnOffWidget(QWidget *parent)
    : QWidget(parent)
    , flagOn(false)
    , duration(100)
    , flagEnabled(true)
{

    setInit();
    setUIControl();


}

OnOffWidget::~OnOffWidget()
{
    delete anim_circle;
    delete anim_bg;
}

/**
 * @brief OnOffWidget::setInit : 초기 세팅
 */
void OnOffWidget::setInit(){
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(QSize(TOTALAREA_WIDTH, CIRCLE_SIZE));
}

/**
 * @brief OnOffWidget::setUIControl : UI 세팅
 */
void OnOffWidget::setUIControl(){

    lb_offText = new QLabel(this);
    lb_offText->setText("OFF");
    lb_offText->setStyleSheet("font-size:14px; color:#FFFFFF; background-color:transparent;");
    //lb_offText->move(32, 8);  // 음악 홈 기준
    lb_offText->move(37, 10);   // 설정 VU옵션 기준

    wg_bg = new OnOffBackground(this);
    wg_bg->resize(CIRCLE_SIZE, CIRCLE_SIZE);

    lb_onText = new QLabel(this);
    lb_onText->setText("ON");
    lb_onText->setStyleSheet("font-size:14px; color:#FFFFFF; background-color:transparent;");
    //lb_onText->move(10, 8);   // 음악 홈 기준
    lb_onText->move(12, 10);    // 설정 VU옵션 기준

    circle = new OnOffCircle(this);

    anim_circle = new QPropertyAnimation(this);
    anim_circle->setTargetObject(circle);
    anim_circle->setPropertyName("pos");

    anim_bg = new QPropertyAnimation(this);
    anim_bg->setTargetObject(wg_bg);
    anim_bg->setPropertyName("size");
}

/**
 * @brief OnOffWidget::paintEvent : [Override] 페인트 이벤트
 * @details OFF 상태 BG draw
 */
void OnOffWidget::paintEvent(QPaintEvent*)
{
    QPen pen(Qt::NoPen);
    QPainter* painter = new QPainter;
    painter->begin(this);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(QColor(COLOR_BG_OFF));
    painter->drawRoundedRect(0, 0, width(), height(), CIRCLE_SIZE/2, CIRCLE_SIZE/2);
    painter->end();
}

/**
 * @brief OnOffWidget::mousePressEvent : [Override] 마우스 누름 이벤트
 * @details
 */
void OnOffWidget::mousePressEvent(QMouseEvent*)
{
    if (!flagEnabled)
    return;

    anim_circle->stop();
    anim_bg->stop();

    anim_circle->setDuration(duration);
    anim_bg->setDuration(duration);

    QSize initial_size(CIRCLE_SIZE, CIRCLE_SIZE);
    QSize final_size(width(), CIRCLE_SIZE);

    int pos_x_s = 0;
    int pos_y_s  = 0;
    int pos_x_e = width() - CIRCLE_SIZE;

    if (flagOn)
    {
    final_size = QSize(CIRCLE_SIZE, CIRCLE_SIZE);
    initial_size = QSize(width()  , CIRCLE_SIZE);

    pos_x_s = pos_x_e;
    pos_x_e = 0;
    }

    //
    anim_circle->setStartValue(QPoint(pos_x_s, pos_y_s));
    anim_circle->setEndValue(QPoint(pos_x_e, pos_y_s));
    anim_bg->setStartValue(initial_size);
    anim_bg->setEndValue(final_size);
    anim_circle->start();
    anim_bg->start();

    flagOn = !flagOn;
    QThread::msleep(500);
    emit signal_changed(flagOn);
}

/**
 * @brief OnOffWidget::setEnabled : Set Enable
 * @param flag bool if true, enable
 */
void OnOffWidget::setEnabled(bool flag)
{
    flagEnabled = flag;
    circle->setEnabled(flag);
    wg_bg->setEnabled(flag);

    if (flag){
        lb_onText->show();
    }else{
        if (getCurrentValue()){
            lb_onText->show();
        }else{
            lb_onText->hide();
        }
    }
    QWidget::setEnabled(flag);
}

/**
 * @brief OnOffWidget::setDuration : Animatino duration 설정
 * @param time
 */
void OnOffWidget::setDuration(int time)
{
    duration = time;
}

/**
 * @brief OnOffWidget::setValue : Set On, Off
 * @param flag bool if true, on
 */
void OnOffWidget::setValue(bool flag)
{
    if(flag != getCurrentValue()){
        flagOn = flag;
        refreshUI();
    }
}

/**
 * @brief OnOffWidget::getCurrentValue : Get On, Off
 * @return bool if true, on
 */
bool OnOffWidget::getCurrentValue() const
{
    return flagOn;
}

/**
 * @brief OnOffWidget::refreshUI : On, Off 상태값에 따른 UI 갱신
 */
void OnOffWidget::refreshUI()
{
  QSize final_size(CIRCLE_SIZE , CIRCLE_SIZE);

  int pos_y_s = 0;
  int pos_x_e = 0;

  if (flagOn)
  {
    final_size = QSize(width(), CIRCLE_SIZE);
    pos_x_e = width() - CIRCLE_SIZE;
  }

  circle->move(QPoint(pos_x_e, pos_y_s));
  wg_bg->resize(final_size);
}

/**
 * @brief OnOffWidget::OnOffBackground::OnOffBackground : 백그라운드 Widget
 * @param parent
 */
OnOffWidget::OnOffBackground::OnOffBackground(QWidget* parent)
  : QWidget(parent)
{
    setFixedHeight(CIRCLE_SIZE);
}
OnOffWidget::OnOffBackground::~OnOffBackground()
{
}
void OnOffWidget::OnOffBackground::paintEvent(QPaintEvent*)
{
    QPainter* painter = new QPainter;
    painter->begin(this);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QPen pen(Qt::NoPen);
    painter->setPen(pen);

    painter->setBrush(QColor(COLOR_BG_ON));
    painter->drawRoundedRect(0, 0, width(), height(), CIRCLE_SIZE/2, CIRCLE_SIZE/2);

    painter->end();
}

/**
 * @brief OnOffWidget::OnOffCircle::OnOffCircle : Circle Widget
 * @param parent
 */
OnOffWidget::OnOffCircle::OnOffCircle(QWidget* parent)
  : QWidget(parent)
{
    setFixedSize(CIRCLE_SIZE, CIRCLE_SIZE);
}
OnOffWidget::OnOffCircle::~OnOffCircle()
{
}
void OnOffWidget::OnOffCircle::paintEvent(QPaintEvent*)
{
    QPainter* painter = new QPainter;
    painter->begin(this);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QPen pen(Qt::NoPen);
    painter->setPen(pen);

    painter->setBrush(QColor("#FFFFFF"));
    painter->drawEllipse(0, 0, CIRCLE_SIZE, CIRCLE_SIZE);

    painter->end();
}
