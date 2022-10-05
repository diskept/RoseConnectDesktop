#include "videodelegate.h"
#include <QEvent>
#include <QMouseEvent>
#include <QFontMetrics>

const int thumbnail_width = 220;
const int space = 20;
const int stretch_width = 1620;        // 1920 화면 디자인 기준
const int static_width = 185;          // 1920 화면 디자인 기준으로 여백등의 고정 값
const int title_width = 1034;            // 1920 화면 디자인 기준
const int duration_width = 132;         // 1920 화면 디자인 기준
const int duration_width_min = 70;

/**
 * @brief VideoDelegate::VideoDelegate : 비디오 목록 delegate
 * @details 비디오 - 비디오 상세
 * @param parent
 */
VideoDelegate::VideoDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    setInit();
}

void VideoDelegate::setInit(){
    pixmapHeart.load(":/images/list_fav_icon.png");
    pixmapMore.load(":/images/icon_menu.png");
    pixmapPlay.load(":/images/album_play_icon.png");
}

/**
 * @brief VideoDelegate::paint : 페인트 Draw
 * @param painter
 * @param option
 * @param index
 */
void VideoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    int width = rect.width();

    // TEXT Width 조정
    // 1920 디자인 시안 고려(각 항목 spacing 20 고려)
    // duration part만 min 적용
    // duration part width 계산 후 track명, album명 width 계산
    int tmp_adjustWidth = 0;
    int tmp_durationWidth = (width-static_width-thumbnail_width)*duration_width/(stretch_width-static_width-thumbnail_width);
    if(tmp_durationWidth < duration_width_min){
        tmp_adjustWidth = duration_width_min-tmp_durationWidth;
        tmp_durationWidth = duration_width_min;
    }
    int tmp_titleWidth = (width-static_width-thumbnail_width)*title_width/(stretch_width-static_width-thumbnail_width) - tmp_adjustWidth/2;


    // Get Data
    QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
    QString tmp_title = map["title"].toString();
    QString tmp_duration = map["duration"].toString();
//    int tmp_index = map["index"].toInt();                     // 사용하지 않는 값으로 주석처리함 by sunnyfish


    // Set Hover Background color
    if(option.state & QStyle::State_MouseOver){
        painter->fillRect(rect, QColor(51,51,51));
    }


    // pen 정의
    QPen linePen(QColor::fromRgb(42,42,42), 1, Qt::SolidLine);
    QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);
    QPen authorPen(QColor::fromRgb(153,153,153), 1, Qt::SolidLine);


    // Font 설정 : 16px 고정
    QFont font_16;
    font_16.setPixelSize(16);

    QFont font_18;
    font_18.setPixelSize(16);


    // Line Draw
    painter->setPen(linePen);
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());

    // 썸네일 Draw
    QIcon thumbNail = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
    if(!thumbNail.isNull()){

        // 1. 썸네일 Draw
        rect = option.rect.adjusted(5,5,0,-5);
        thumbNail.paint(painter, rect, Qt::AlignVCenter);

        // 2. 썸네일 위 duration Draw
        // Duration
        QFontMetrics *fm = new QFontMetrics(font_18);
        int tmp_durationWidth =fm->boundingRect(tmp_duration).width();
        int tmp_durationHeigth = fm->boundingRect(tmp_duration).height();

        painter->setPen(titlePen);
        painter->setFont(font_18);

        rect = option.rect.adjusted(7,97,0,0);;
        painter->fillRect(rect.left(),rect.top(), tmp_durationWidth+32,tmp_durationHeigth+10, QColor(0,0,0,153));
        painter->drawText(rect.left(), rect.top(), tmp_durationWidth+32,tmp_durationHeigth+10, Qt::AlignCenter, tmp_duration, &rect);

        // 3. hover 상태 Draw
        if(option.state & QStyle::State_MouseOver){
            rect = option.rect.adjusted(5,5,0,-5);
            painter->fillRect(rect, QColor(51,51,51,100));
            rect = option.rect.adjusted(86,35,0,-35);
            painter->drawPixmap(rect.left(), rect.top(),60,60 , pixmapPlay );
        }
    }


    // 폰트 재설정
    painter->setFont(font_16);


    // 타이틀
    rect = option.rect.adjusted(thumbnail_width+space,12,0,-5);
    painter->setPen(titlePen);
    painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                      , Qt::AlignVCenter|Qt::AlignLeft, ellipseString(tmp_title, tmp_titleWidth, font_16), &rect);


    // Duration
    rect = option.rect.adjusted(thumbnail_width+space+tmp_titleWidth+space,0,0,0);
    painter->drawText(rect.left(), rect.top(), tmp_durationWidth, rect.height(), Qt::AlignCenter, tmp_duration, &rect);


    // --------------------------------------------------
    //  API 미지원 HIDE
    // --------------------------------------------------
    /*
    // 하트
    rect = option.rect.adjusted(width-120,31,-60,-31);
    painter->drawPixmap(rect.left(), rect.top(),rect.width(),rect.height() , pixmapHeart );


    // 더보기
    rect = option.rect.adjusted(width-60,31,0,-31);
    painter->drawPixmap(rect.left(), rect.top(),60,70 , pixmapMore );
    */


}

/**
 * @brief VideoDelegate::sizeHint : 싸이즈 힌트
 * @note width 값은 min 값 세팅
 * @param option
 * @param index
 * @return
 */
QSize VideoDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option);
    Q_UNUSED(index);
    //return QSize(944,132);
    return QSize(option.rect.width(),132);
}

/**
 * @brief VideoDelegate::ellipseString : Text ellipse
 * @details Text가 정해진 영역보다 긴 경우 ... 표기
 * @param p_str
 * @param p_width
 * @param p_font
 * @return
 */
QString VideoDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
    QFontMetrics metrics(p_font);
    return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
}

/**
 * @brief VideoDelegate::editorEvent : 마우스 이벤트 처리
 * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
 * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
 * @param event
 * @param model
 * @param option
 * @param index
 * @return
 */
bool VideoDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(option);           // 미사용으로 주석처리함 by sunnyfish
    QMouseEvent *eventMouse = (QMouseEvent*)event;

    if(eventMouse->button() == Qt::LeftButton){

        QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
        int tmp_index = map["index"].toInt();
        int tmp_pressBtn = index.data(Qt::UserRole+1).toInt();

        // --------------------------------------------------
        //  API 미지원 HIDE
        // --------------------------------------------------
        //QRect rectBtn_1 = option.rect.adjusted(option.rect.width()-120,31,-60,-31);
        //QRect rectBtn_2 = option.rect.adjusted(option.rect.width()-60,31,0,-31);

        if (event->type() == QEvent::MouseButtonPress){

            /*
            if(rectBtn_1.contains(eventMouse->pos())){
                model->setData(index, BtnType::trackHeart, Qt::UserRole+1);
            }
            else if(rectBtn_2.contains(eventMouse->pos())){
                model->setData(index, BtnType::more, Qt::UserRole+1);
            }else{
                model->setData(index, BtnType::etc, Qt::UserRole+1);
            }
            */
            model->setData(index, BtnType::etc, Qt::UserRole+1);
        }else if(event->type() == QEvent::MouseButtonRelease){
            /*
            if(rectBtn_1.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::trackHeart){
                emit signal_clickedBtn(tmp_index, BtnType::trackHeart);
            }
            else if(rectBtn_2.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::more){
                emit signal_clickedBtn(tmp_index, BtnType::more);
            }
            else if(tmp_pressBtn==BtnType::etc){
                emit signal_clickedBtn(tmp_index, BtnType::etc);
            }
            */

            if(tmp_pressBtn==BtnType::etc){
                emit signal_clickedBtn(tmp_index, BtnType::etc);
            }

            model->setData(index, BtnType::none, Qt::UserRole+1);
        }

    }
     return true;
}
