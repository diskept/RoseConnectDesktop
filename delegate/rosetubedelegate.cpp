#include "rosetubedelegate.h"
#include <QEvent>
#include <QMouseEvent>
#include <QFontMetrics>

const int thumbnail_width = 220;
const int space = 20;
const int stretch_width = 1620;        // 1920 화면 디자인 기준
const int static_width = 245;          // 1920 화면 디자인 기준으로 여백등의 고정 값
const int title_width = 1034;            // 1920 화면 디자인 기준
const int duration_width = 132;         // 1920 화면 디자인 기준
const int duration_width_min = 70;

/**
 * @brief RosetubeDelegate::RosetubeDelegate : 로즈튜브 트랙 목록 delegate
 * @details 로즈튜브- 플레이리스트 상세
 * @param parent
 */
RosetubeDelegate::RosetubeDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    setInit();
}

/**
 * @brief RosetubeDelegate::setInit : 초기값 세팅
 */
void RosetubeDelegate::setInit(){
    pixmapPlaylist.load(":images/play_list_add_icon.png");
    pixmapHeartOn.load(":images/list_fav_icon_on.png");
    pixmapHeartOff.load(":images/list_fav_icon.png");
    pixmapMore.load(":images/icon_menu.png");
    pixmapPlay.load(":images/album_play_icon.png");
}

/**
 * @brief RosetubeDelegate::paint : 페인트 Draw
 * @param painter
 * @param option
 * @param index
 */
void RosetubeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
    QString tmp_artist = map["artist"].toString();
    QString tmp_viewCount = map["viewCount"].toString();
    QString tmp_duration = map["duration"].toString();
    QString tmp_favorites = map["favorites"].toString();


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
        if(!tmp_duration.isEmpty()){
            QFontMetrics *fm = new QFontMetrics(font_18);
            int tmp_durationWidth =fm->boundingRect(tmp_duration).width();
            int tmp_durationHeigth = fm->boundingRect(tmp_duration).height();

            painter->setPen(titlePen);
            painter->setFont(font_18);

            rect = option.rect.adjusted(5,101,0,0);;
            painter->fillRect(rect.left(),rect.top(), tmp_durationWidth+32,tmp_durationHeigth+10, QColor(0,0,0,153));
            painter->drawText(rect.left(), rect.top(), tmp_durationWidth+32,tmp_durationHeigth+10, Qt::AlignCenter, tmp_duration, &rect);
        }

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
    rect = option.rect.adjusted(thumbnail_width+space,34,0,-5);
    painter->setPen(titlePen);
    painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                      , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_title, tmp_titleWidth, font_16), &rect);

    // 아티스트
    rect = option.rect.adjusted(thumbnail_width+space,57,0,0);
    painter->setPen(authorPen);
    painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                      , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_artist, tmp_titleWidth, font_16), &rect);

    // 조회수
    rect = option.rect.adjusted(thumbnail_width+space,80,0,0);
    painter->setPen(authorPen);
    painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                      , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_viewCount, tmp_titleWidth, font_16), &rect);


    // Duration
    rect = option.rect.adjusted(thumbnail_width+space+tmp_titleWidth+space,0,0,0);
    painter->drawText(rect.left(), rect.top(), tmp_durationWidth, rect.height(), Qt::AlignCenter, tmp_duration, &rect);


    // 플레이리스트 담기
    rect = option.rect.adjusted(width-180,31,-120,-31);
    painter->drawPixmap(rect.left(), rect.top(),rect.width(), rect.height() , pixmapPlaylist );

    // 하트
    rect = option.rect.adjusted(width-120,31,-60,-31);
    painter->drawPixmap(rect.left(), rect.top(),rect.width(),rect.height() , tmp_favorites=="1" ? pixmapHeartOn : pixmapHeartOff );


    // 더보기
    rect = option.rect.adjusted(width-60,31,0,-31);
    painter->drawPixmap(rect.left(), rect.top(),60,70 , pixmapMore );


}

/**
 * @brief RosetubeDelegate::sizeHint : 싸이즈 힌트
 * @note width 값은 min 값 세팅
 * @param option
 * @param index
 * @return
 */
QSize RosetubeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option);
    Q_UNUSED(index);
    //return QSize(883,132);
    return QSize(option.rect.width(), 132);
}

/**
 * @brief RosetubeDelegate::ellipseString : Text ellipse
 * @details Text가 정해진 영역보다 긴 경우 ... 표기
 * @param p_str
 * @param p_width
 * @param p_font
 * @return
 */
QString RosetubeDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
    QFontMetrics metrics(p_font);
    return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
}

/**
 * @brief RosetubeDelegate::editorEvent : 마우스 이벤트 처리
 * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
 * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
 * @param event
 * @param model
 * @param option
 * @param index
 * @return
 */
bool RosetubeDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *eventMouse = (QMouseEvent*)event;

    if(eventMouse->button() == Qt::LeftButton){

        QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
        int tmp_index = map["index"].toInt();
        int tmp_pressBtn = index.data(Qt::UserRole+1).toInt();

        QRect rectBtn_1 = option.rect.adjusted(option.rect.width()-180,31,-120,-31);
        QRect rectBtn_2 = option.rect.adjusted(option.rect.width()-120,31,-60,-31);
        QRect rectBtn_3 = option.rect.adjusted(option.rect.width()-60,31,0,-31);

        if (event->type() == QEvent::MouseButtonPress){

            if(rectBtn_1.contains(eventMouse->pos())){
                model->setData(index, BtnType::playListAdd, Qt::UserRole+1);
            }
            else if(rectBtn_2.contains(eventMouse->pos())){
                model->setData(index, BtnType::trackHeart, Qt::UserRole+1);
            }
            else if(rectBtn_3.contains(eventMouse->pos())){
                model->setData(index, BtnType::more, Qt::UserRole+1);
            }
            else{
                model->setData(index, BtnType::etc, Qt::UserRole+1);
            }
        }else if(event->type() == QEvent::MouseButtonRelease){
            if(rectBtn_1.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::playListAdd){
                emit signal_clickedBtn(tmp_index, BtnType::playListAdd);
            }
            else if(rectBtn_2.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::trackHeart){
                emit signal_clickedBtn(tmp_index, BtnType::trackHeart);
            }
            else if(rectBtn_3.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::more){
                emit signal_clickedBtn(tmp_index, BtnType::more);
            }
            else if(tmp_pressBtn==BtnType::etc){
                emit signal_clickedBtn(tmp_index, BtnType::etc);
            }
            model->setData(index, BtnType::none, Qt::UserRole+1);
        }

    }
     return true;
}
