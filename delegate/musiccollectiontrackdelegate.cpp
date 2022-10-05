#include "musiccollectiontrackdelegate.h"

#include <QRect>
#include <QMouseEvent>

//const int MAX_STAR_CNT = 5;
const int thumbnail_width = 60;
const int space = 20;
const int stretch_width = 1620;        // 1920 화면 디자인 기준
const int static_width = 260;          // 1920 화면 디자인 기준으로 여백등의 고정 값
const int title_width = 752;            // 1920 화면 디자인 기준
const int albumTitle_width = 410;       // 1920 화면 디자인 기준
const int duration_width = 132;         // 1920 화면 디자인 기준
const int duration_width_min = 70;

MusicCollectionTrackDelegate::MusicCollectionTrackDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    setInit();
}

void MusicCollectionTrackDelegate::setInit(){
    pixmapPlaylist.load(":/images/play_list_add_icon.png");
    pixmapStarOff.load(":/images/star_icon_off.png");
    pixmapStarOn.load(":/images/star_icon_on.png");
    pixmapMore.load(":/images/icon_menu.png");
    pixmapPlay.load(":/images/album_play_icon.png");
}

/**
 * @brief MusicCollectionTrackDelegate::paint : 페인트 Draw
 * @param painter
 * @param option
 * @param index
 */
void MusicCollectionTrackDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    int width = rect.width();

    // TEXT Width 조정
    // 1920 디자인 시안 고려(각 항목 spacing 20 고려)
    // duration part만 min 적용
    // duration part width 계산 후 track명, album명 width 계산
    int tmp_adjustWidth = 0;
    int tmp_durationWidth = (width-static_width)*duration_width/(stretch_width-static_width);
    if(tmp_durationWidth < duration_width_min){
        tmp_adjustWidth = duration_width_min-tmp_durationWidth;
        tmp_durationWidth = duration_width_min;
    }
    int tmp_titleWidth = (width-static_width)*title_width/(stretch_width-static_width) - tmp_adjustWidth/2;
    int tmp_albumWidth = (width-static_width)*albumTitle_width/(stretch_width-static_width) - tmp_adjustWidth/2;


    // Get Data
    QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
    QString tmp_title = map["title"].toString();
    QString tmp_author = map["artist"].toString();
    QString tmp_album = map["album"].toString();
    QString tmp_duration = map["duration"].toString();
//    int tmp_star = map["star"].toInt();


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
    painter->setFont(font_16);



    // Line Draw
    painter->setPen(linePen);
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());

    // 썸네일 Draw :: if dir인 경우 폴더 이미지
    QIcon thumbNail = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
    if(!thumbNail.isNull()){
        // 1. 썸네일 Draw
        rect = option.rect.adjusted(5,5,-10,-5);
        thumbNail.paint(painter, rect, Qt::AlignVCenter);

        // 3. hover 상태 Draw
        if(option.state & QStyle::State_MouseOver){
            rect = option.rect.adjusted(5,5,-10,-5);        // 정사각형 회색 배경
            painter->fillRect(rect, QColor(51,51,51,100));
            rect = option.rect.adjusted(20,20,0,-20);       // play 아이콘 위치
            painter->drawPixmap(rect.left(), rect.top(),30,30 , pixmapPlay );
        }
    }



    // 제목
    rect = option.rect.adjusted(thumbnail_width+space,12,0,-5);
    painter->setPen(titlePen);
    painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                      , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_title, tmp_titleWidth, font_16), &rect);

    // 아티스트
    rect = option.rect.adjusted(thumbnail_width+space,35,0,0);
    painter->setPen(authorPen);
    painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                      , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_author, tmp_titleWidth, font_16), &rect);


    // 앨범명
    rect = option.rect.adjusted(thumbnail_width+space+tmp_titleWidth+space,0,0,0);
    painter->setPen(titlePen);
    painter->drawText(rect.left(), rect.top(), tmp_albumWidth, rect.height()
                      , Qt::AlignVCenter|Qt::AlignLeft, ellipseString(tmp_album, tmp_albumWidth, font_16), &rect);

    // star 그리기
    // API star alaways 0
    /*
    for(int i = 0 ; i < MAX_STAR_CNT; i++){
        rect = option.rect.adjusted(thumbnail_width+space+tmp_titleWidth+space+tmp_albumWidth+space+(i*34),0,0,0);

        if(tmp_star >= i){
            painter->drawPixmap(rect.left(), rect.center().y()-20,34,40, pixmapStarOn);
        }else{
            painter->drawPixmap(rect.left(), rect.center().y()-20,34,40, pixmapStarOff);
        }
    }
    */

    // Duration
    rect = option.rect.adjusted(thumbnail_width+space+tmp_titleWidth+space+tmp_albumWidth+space,0,0,0);
    painter->drawText(rect.left(), rect.top(), tmp_durationWidth, rect.height(), Qt::AlignVCenter|Qt::AlignLeft, tmp_duration, &rect);

    // 플레이리스트 담기
    rect = option.rect.adjusted(width-120,5,-60,-5);
    painter->drawPixmap(rect.left(), rect.top(),rect.width(), rect.height() , pixmapPlaylist );


    // 더보기
    rect = option.rect.adjusted(width-60,0,0,0);
    painter->drawPixmap(rect.left(), rect.top(),60,70 , pixmapMore );
}

/**
 * @brief MusicDelegate::sizeHint : 싸이즈 힌트
 * @note width 값은 min 값 세팅
 * @param option
 * @param index
 * @return
 */
QSize MusicCollectionTrackDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option);
    Q_UNUSED(index);
    //return QSize(944,70);
    return QSize(option.rect.width(), 70);
}

/**
 * @brief MusicDelegate::ellipseString : Text ellipse
 * @details Text가 정해진 영역보다 긴 경우 ... 표기
 * @param p_str
 * @param p_width
 * @param p_font
 * @return
 */
QString MusicCollectionTrackDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
    QFontMetrics metrics(p_font);
    return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
}

/**
 * @brief MusicDelegate::editorEvent : 마우스 이벤트 처리
 * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
 * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
 * @param event
 * @param model
 * @param option
 * @param index
 * @return
 */
bool MusicCollectionTrackDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *eventMouse = (QMouseEvent*)event;

    if(eventMouse->button() == Qt::LeftButton){

        QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
        int tmp_index = map["index"].toInt();
        int tmp_pressBtn = index.data(Qt::UserRole+1).toInt();

        QRect rectBtn_1 = option.rect.adjusted(option.rect.width()-120,0,-60,0);
        QRect rectBtn_2 = option.rect.adjusted(option.rect.width()-60,0,0,0);

        if (event->type() == QEvent::MouseButtonPress){
            if(rectBtn_1.contains(eventMouse->pos())){
                model->setData(index, BtnType::playListAdd, Qt::UserRole+1);
            }
            else if(rectBtn_2.contains(eventMouse->pos())){
                model->setData(index, BtnType::more, Qt::UserRole+1);
            }else{
                model->setData(index, BtnType::etc, Qt::UserRole+1);
            }
        }else if(event->type() == QEvent::MouseButtonRelease){

            if(rectBtn_1.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::playListAdd){
                 emit signal_clickedBtn(tmp_index, BtnType::playListAdd);
            }
            else if(rectBtn_2.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::more){
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
