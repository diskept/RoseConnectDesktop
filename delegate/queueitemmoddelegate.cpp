#include "queueitemmoddelegate.h"

#include <QMouseEvent>

const int checkbox_width = 50;
const int thumbnail_width = 60;
const int space = 20;
const int title_width = 280;
//const int total_width_min = 500;
const int total_height = 70;

/**
 * @brief QueueItemModDelegate::QueueItemModDelegate : 큐 재생 목록 편집 Delegate
 * @param parent
 */
QueueItemModDelegate::QueueItemModDelegate(QObject *parent) : QAbstractItemDelegate(parent)
{
    setInit();
}

/**
 * @brief QueueItemModDelegate::setInit : 초기 세팅
 */
void QueueItemModDelegate::setInit(){
    pixmapCheckOn.load(":images/list_check_on.png");    // 50 by 50
    pixmapCheckOff.load(":images/list_check_off.png");
    pixmapDelete.load(":images/fol_del_ico.png");       // 40 by 40
    pixmapNet.load(":images/playBar/net_play_ico.png"); // 30x30
    pixmapPlaying.load(":images/playing_icon.png");
}

/**
 * @brief QueueItemModDelegate::paint : 페인트 Draw
 * @param painter
 * @param option
 * @param index
 */
void QueueItemModDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    int width = rect.width();
    int tmp_titleWidth = title_width;

    // Get Data
    QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
    QString tmp_title       = map[QueueItemModDelegate_C::KEY_TITLE].toString();
    QString tmp_author      = map[QueueItemModDelegate_C::KEY_ARTIST].toString();
    bool isDirect           = map[QueueItemModDelegate_C::KEY_ISDIRECT].toBool();
    bool isPlay             = map[QueueItemModDelegate_C::KEY_ISPLAY].toBool();
    bool isCheck            = map[QueueItemModDelegate_C::KEY_ISCHECK].toBool();
    QueueItemTypes itemType = static_cast<QueueItemTypes>(map[QueueItemModDelegate_C::KEY_TYPE].toInt());

    if(isCheck){
        painter->fillRect(rect, QColor(21,21,21));
    }

    // Set Hover Background color
    if(option.state & QStyle::State_MouseOver){
        painter->fillRect(rect, QColor(77,77,77));
    }

    // pen 정의
    QPen linePen(QColor::fromRgb(73,73,73), 1, Qt::SolidLine);
    QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);
    QPen authorPen(QColor::fromRgb(153,153,153), 1, Qt::SolidLine);
    if(isPlay==true){
        // 재생중인경우 스타일 변경
        titlePen.setColor(QColor::fromRgb(177,134,88));
        authorPen.setColor(QColor::fromRgb(177,134,88));
    }

    // Font 설정 : 16px 고정
    QFont font_16;
    font_16.setPixelSize(16);
    painter->setFont(font_16);

    // Line Draw
    painter->setPen(linePen);
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());

    // 체크 박스 Draw
    rect = option.rect.adjusted(0,10,0,-10);
    painter->drawPixmap(rect.left(), rect.top(),checkbox_width,checkbox_width , isCheck ? pixmapCheckOn : pixmapCheckOff );

    QIcon thumbNail = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
    if(!thumbNail.isNull()){
        // 1. 썸네일 Draw
        rect = option.rect.adjusted(checkbox_width,5,-10,-5);
        thumbNail.paint(painter, rect, Qt::AlignVCenter);
        if(isDirect){
            // 음악의 네트워크 아이콘
            rect = option.rect.adjusted(85,35,115,65);
            painter->drawPixmap(rect.left(), rect.top(),30,30 , pixmapNet );
        }
        if(isPlay==true){
            // 재생중 아이콘
            if(itemType==QueueItemTypes::Music){
                rect = option.rect.adjusted(checkbox_width,5,-385,-5);
            }else if(itemType==QueueItemTypes::Rosetube){
                rect = option.rect.adjusted(checkbox_width,18,-385,-18);
            }
            painter->fillRect(rect, QColor(0,0,0,180));
            rect = option.rect.adjusted(70,20,0,-20);       // play 아이콘 위치
            painter->drawPixmap(rect.left(), rect.top(),24,26 , pixmapPlaying );
        }
    }


    if(tmp_author==""){

        // 제목
        rect = option.rect.adjusted(checkbox_width+thumbnail_width+space,25,0,0);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                          , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_title, tmp_titleWidth, font_16), &rect);

    }else if(tmp_author!=""){

        // 제목
        rect = option.rect.adjusted(checkbox_width+thumbnail_width+space,12,0,-5);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                          , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_title, tmp_titleWidth, font_16), &rect);

        // 아티스트
        rect = option.rect.adjusted(checkbox_width+thumbnail_width+space,35,0,0);
        painter->setPen(authorPen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                          , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_author, tmp_titleWidth, font_16), &rect);
    }


    // 휴지통
    rect = option.rect.adjusted(width-55,15,0,-15);
    painter->drawPixmap(rect.left(), rect.top(),40,40 , pixmapDelete );

}

/**
 * @brief QueueItemModDelegate::sizeHint : 싸이즈 힌트
 * @note width 값은 min 값 세팅
 * @param option
 * @param index
 * @return
 */
QSize QueueItemModDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option);
    Q_UNUSED(index);
    //return QSize(total_width_min,total_height);
    return QSize(option.rect.width(),total_height);
}

/**
 * @brief QueueItemModDelegate::ellipseString : Text ellipse
 * @details Text가 정해진 영역보다 긴 경우 ... 표기
 * @param p_str
 * @param p_width
 * @param p_font
 * @return
 */
QString QueueItemModDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
    QFontMetrics metrics(p_font);
    return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
}

/**
 * @brief QueueItemModDelegate::editorEvent : 마우스 이벤트 처리
 * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
 * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
 * @param event
 * @param model
 * @param option
 * @param index
 * @return
 */
bool QueueItemModDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *eventMouse = (QMouseEvent*)event;

    if(eventMouse->button() == Qt::LeftButton){

        QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
        int tmp_index = map[QueueItemModDelegate_C::KEY_INDEX].toInt();
        int tmp_pressBtn = index.data(Qt::UserRole+1).toInt();

        QRect rectBtn_1 = option.rect.adjusted(option.rect.width()-55,15,0,-15);

        if (event->type() == QEvent::MouseButtonPress){
            if(rectBtn_1.contains(eventMouse->pos())){
                model->setData(index, BtnType::remove, Qt::UserRole+1);
            }else{
                model->setData(index, BtnType::etc, Qt::UserRole+1);
            }
        }else if(event->type() == QEvent::MouseButtonRelease){
            if(rectBtn_1.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::remove){
                emit signal_clickedBtn(tmp_index, BtnType::remove);
            }
            else if(tmp_pressBtn==BtnType::etc){
                emit signal_clickedBtn(tmp_index, BtnType::etc);
            }
            model->setData(index, BtnType::none, Qt::UserRole+1);
        }

    }
     return true;
}
