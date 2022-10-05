#include "usbfiledelegate.h"
#include <QRect>
#include <QMouseEvent>

const int thumbnail_width = 60;
const int space = 20;
const int stretch_width = 1620;        // 1920 화면 디자인 기준
const int static_width = 260;          // 1920 화면 디자인 기준으로 여백등의 고정 값
const int title_width = 752;            // 1920 화면 디자인 기준
const int albumTitle_width = 410;       // 1920 화면 디자인 기준
const int duration_width = 132;         // 1920 화면 디자인 기준
const int duration_width_min = 70;

/**
 * @brief USBFileDelegate::USBFileDelegate : USB 파일정보 delegate
 * @param parent
 */
USBFileDelegate::USBFileDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    setInit();
}

void USBFileDelegate::setInit(){
    pixmapMore.load(":/images/icon_menu.png");
    pixmapFavor.load(":/images/play_list_add_icon.png");
    pixmapHeartOn.load(":/images/list_fav_icon_on.png");
    pixmapHeartOff.load(":/images/list_fav_icon.png");
    pixmapBookMark.load(":/images/fol_bookmark.png");
}

/**
 * @brief USBFileDelegate::paint : 페인트 Draw
 * @details 파일타입(Dir or file) 에 따라 draw 다름<br>
 * if Dir, 네트워크
 * @param painter
 * @param option
 * @param index
 */
void USBFileDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
    QString tmp_fileName = map["fileName"].toString();
    QString tmp_filePath = map["filePath"].toString();
    QString tmp_title = map["title"].toString();
    QString tmp_author = map["author"].toString();
    QString tmp_album = map["albumTitle"].toString();
    QString tmp_duration = map["duration"].toString();
    bool tmp_flagIsFile = map["isFile"].toBool();
    bool tmp_favorites = map["favorites"].toBool();


    // Set Hover Background color
    if(option.state & QStyle::State_MouseOver){
        painter->fillRect(rect, QColor(51,51,51));
    }


    // pen 정의
    QPen linePen(QColor::fromRgb(42,42,42), 1, Qt::SolidLine);
    QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);
    QPen authorPen(QColor::fromRgb(153,153,153), 1, Qt::SolidLine);


    // Line Draw
    painter->setPen(linePen);
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());

    // 썸네일 Draw :: if dir인 경우 폴더 이미지
    QIcon thumbNail = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
    if(!thumbNail.isNull()){
        rect = option.rect.adjusted(0,5,-10,-5);
        thumbNail.paint(painter, rect, Qt::AlignVCenter);
    }

    // Font 설정 : 16px 고정
    QFont font_16;
    font_16.setPixelSize(16);
    painter->setFont(font_16);


    if(!tmp_flagIsFile){
        // 폴더명
        rect = option.rect.adjusted(thumbnail_width+space,0,0,0);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignVCenter|Qt::AlignLeft, tmp_title, &rect);


        if(flagIsMusicFileMode){
            // 즐겨찾기
            rect = option.rect.adjusted(width-60,0,0,0);
            painter->drawPixmap(rect.left(), rect.top(),60,70 , pixmapBookMark );
        }
        /*
        // 네트워크 폴더 추가
        rect = option.rect.adjusted(width-60,0,0,0);
        painter->drawPixmap(rect.left(), rect.top(),60,70 , pixmapAddFolder );
        */

    }else{

        if(tmp_author==""){
            // -----------------------------------------------------------------------
            // 아티스트명이 없는경우... 파일명이 VerticalCenter 로 보이도록 위치 재조정
            // -----------------------------------------------------------------------
            // 파일명
            rect = option.rect.adjusted(thumbnail_width+space,25,0,0);
            painter->setPen(titlePen);
            painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                              , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_fileName, tmp_titleWidth, font_16), &rect);

        }else if(tmp_author!=""){
            // 파일명
            rect = option.rect.adjusted(thumbnail_width+space,12,0,-5);
            painter->setPen(titlePen);
            painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                              , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_fileName, tmp_titleWidth, font_16), &rect);

            // 아티스트
            rect = option.rect.adjusted(thumbnail_width+20,35,0,0);
            painter->setPen(authorPen);
            painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                              , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_author, tmp_titleWidth, font_16), &rect);

        }
        // 앨범명
        rect = option.rect.adjusted(thumbnail_width+space+tmp_titleWidth+space,0,0,0);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_albumWidth, rect.height()
                          , Qt::AlignVCenter|Qt::AlignLeft, ellipseString(tmp_album, tmp_albumWidth, font_16), &rect);

        if(flagIsMusicFileMode==false){
            // -----------------------------------------------------------------------
            // Duration을 맨오른쪽정렬하기위해 위치 재조정
            // -----------------------------------------------------------------------
            // Duration
            //rect = option.rect.adjusted(width-100,0,0,0);
            //painter->drawText(rect.left(), rect.top(), tmp_durationWidth, rect.height(), Qt::AlignVCenter|Qt::AlignLeft, tmp_duration, &rect);

        }else{

            // Duration
            if(tmp_duration != "00:00"){
                rect = option.rect.adjusted(thumbnail_width+space+tmp_titleWidth+space+tmp_albumWidth+space,0,0,0);
                painter->drawText(rect.left(), rect.top(), tmp_durationWidth, rect.height(), Qt::AlignVCenter|Qt::AlignLeft, tmp_duration, &rect);
            }

            // 플레이리스트 담기
            //rect = option.rect.adjusted(width-180,5,-120,-5);
            //painter->drawPixmap(rect.left(), rect.top(),rect.width(), rect.height() , pixmapFavor );

            // 하트
            //rect = option.rect.adjusted(width-120,0,-60,0);
            //painter->drawPixmap(rect.left(), rect.top(),rect.width(),rect.height() , tmp_favorites ? pixmapHeartOn : pixmapHeartOff );

            // 더보기
            rect = option.rect.adjusted(width-60,0,0,0);
            painter->drawPixmap(rect.left(), rect.top(),60,70 , pixmapMore );
        }

    }
}

/**
 * @brief USBFileDelegate::sizeHint : 싸이즈 힌트
 * @note width 값은 min 값 세팅
 * @param option
 * @param index
 * @return
 */
QSize USBFileDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option);
    Q_UNUSED(index);
    //return QSize(944,70);
    return QSize(option.rect.width(), 70);
}

/**
 * @brief USBFileDelegate::ellipseString : Text ellipse
 * @details Text가 정해진 영역보다 긴 경우 ... 표기
 * @param p_str
 * @param p_width
 * @param p_font
 * @return
 */
QString USBFileDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
    QFontMetrics metrics(p_font);
    return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
}

/**
 * @brief USBFileDelegate::editorEvent : 마우스 이벤트 처리
 * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
 * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
 * @param event
 * @param model
 * @param option
 * @param index
 * @return
 */
bool USBFileDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *eventMouse = (QMouseEvent*)event;

    if(eventMouse->button() == Qt::LeftButton){

        QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
        int tmp_index = map["index"].toInt();
        bool tmp_flagIsFile = map["isFile"].toBool();
        int tmp_pressBtn = index.data(Qt::UserRole+1).toInt();

        QRect rectBtn_1 = option.rect.adjusted(option.rect.width()-180,5,-120,-5);
        QRect rectBtn_2 = option.rect.adjusted(option.rect.width()-120,0,-60,0);
        QRect rectBtn_3 = option.rect.adjusted(option.rect.width()-60,0,0,0);

        if (event->type() == QEvent::MouseButtonPress){

            if(rectBtn_1.contains(eventMouse->pos()) && flagIsMusicFileMode){
                model->setData(index, BtnType::playListAdd, Qt::UserRole+1);
            }
            else if(rectBtn_2.contains(eventMouse->pos()) ){
                if(tmp_flagIsFile ){
                    model->setData(index, BtnType::trackHeart, Qt::UserRole+1);
                }
            }
            else if(rectBtn_3.contains(eventMouse->pos()) ){
                if(tmp_flagIsFile && flagIsMusicFileMode){
                    model->setData(index, BtnType::more, Qt::UserRole+1);
                }else{
                    if(flagIsMusicFileMode){
                        model->setData(index, BtnType::folderFav, Qt::UserRole+1);
                    }
                }
            }else{
                model->setData(index, BtnType::etc, Qt::UserRole+1);
            }
        }else if(event->type() == QEvent::MouseButtonRelease){

            if(rectBtn_1.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::playListAdd && flagIsMusicFileMode){
                emit signal_clickedBtn(tmp_index, BtnType::playListAdd);
            }
            else if(rectBtn_2.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::trackHeart && flagIsMusicFileMode){
                emit signal_clickedBtn(tmp_index, BtnType::trackHeart);
            }
            else if(rectBtn_3.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::more && flagIsMusicFileMode){
                emit signal_clickedBtn(tmp_index, BtnType::more);
            }
            else if(rectBtn_3.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::folderFav && flagIsMusicFileMode){
                emit signal_clickedBtn(tmp_index, BtnType::folderFav);
            }
            else if(tmp_pressBtn==BtnType::etc){
                emit signal_clickedBtn(tmp_index, BtnType::etc);
            }
            model->setData(index, BtnType::none, Qt::UserRole+1);
        }

    }
     return true;
}

/**
 * @brief USBFileDelegate::setHiddenOptionBtn : 옵션버튼 hide
 * @note 플레이리스트담기, 하트, 더보기
 */
void USBFileDelegate::setVideoFileMode(){
    flagIsMusicFileMode = false;
}
