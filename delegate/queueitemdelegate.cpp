#include "queueitemdelegate.h"

#include <QRect>
#include <QMouseEvent>
const int thumbnail_width = 60;
const int space = 20;
const int title_width = 330;
const int total_width_min = 500;
const int total_height = 70;

const int checkbox_width = 50;

/**
 * @brief QueueItemDelegate::QueueItemDelegate 생성자
 */
QueueItemDelegate::QueueItemDelegate(QObject *parent) : QAbstractItemDelegate(parent)
{    
    setInit();
}

void QueueItemDelegate::setInit(){
    pixmapCheckOff.load(":/images/playlist/list_check_off.png");
    pixmapCheckOn.load(":/images/playlist/list_check_on.png");
    pixmapDel.load(":/images/playlist/fol_del_ico.png");

    pixmapMore.load(":/images/icon_menu.png");
    pixmapNet.load(":/images/playBar/net_play_ico.png");
    pixmapShare.load(":/images/playBar/mus_play_ico_share.png");
    pixmapPlaying.load(":/images/playing_icon.png");
}

/**
 * @brief QueueItemDelegate::paint : 페인트 Draw
 * @details 파일타입(Dir or file) 에 따라 draw 다름<br>
 * if Dir, 네트워크
 * @param painter
 * @param option
 * @param index
 */
void QueueItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    //int width = rect.width();
    int tmp_titleWidth = title_width;

    // Get Data
    QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
    bool isEditMode         = map[QueueItemDelegate_C::KEY_ISEDITMode].toBool();
    bool isChecked          = map[QueueItemDelegate_C::KEY_ISCHECKED].toBool();
    QString tmp_title       = map[QueueItemDelegate_C::KEY_TITLE].toString();
    QString tmp_author      = map[QueueItemDelegate_C::KEY_ARTIST].toString();
    bool isDirect           = map[QueueItemDelegate_C::KEY_ISDIRECT].toBool();
    bool isShareFile        = map[QueueItemDelegate_C::KEY_ISSHAREFILE].toBool();
    bool isPlay             = map[QueueItemDelegate_C::KEY_ISPLAY].toBool();
    QueueItemTypes itemType = static_cast<QueueItemTypes>(map[QueueItemDelegate_C::KEY_TYPE].toInt());


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
    //painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    painter->drawLine(10+rect.bottomLeft().x(), rect.bottomLeft().y(), rect.bottomRight().x()-10, rect.bottomRight().y());

    int startLeft = 10;
    if(isEditMode==true){
        // 편집모드
        QPixmap tmp_pixmap_checked = pixmapCheckOff;
        if(isChecked){
            rect = option.rect.adjusted(0,0,0,-1);
            painter->fillRect(rect, QColor(33,33,33));
            tmp_pixmap_checked = pixmapCheckOn;
        }
        rect = option.rect.adjusted(0,(total_height-checkbox_width)/2,0,0);
        painter->drawPixmap(rect.left(), rect.top(),checkbox_width,checkbox_width , tmp_pixmap_checked );
        startLeft = checkbox_width;
    }

    QIcon thumbNail = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
    if(!thumbNail.isNull()){
        // 1. 썸네일 Draw
        rect = option.rect.adjusted(startLeft+5,5,-10,-5);
        thumbNail.paint(painter, rect, Qt::AlignVCenter);
        if(isDirect && isShareFile){
            // 음악의 공유 아이콘
            rect = option.rect.adjusted(startLeft+35,35,-436+startLeft,-6);
            painter->fillRect(rect, QColor(0,0,0,180));
            rect = option.rect.adjusted(startLeft+35,35,65,65);
            painter->drawPixmap(rect.left(), rect.top(),30,30 , pixmapShare );
        }else if(isDirect){
            // 음악의 네트워크 아이콘
            rect = option.rect.adjusted(startLeft+35,35,-436+startLeft,-6);
            painter->fillRect(rect, QColor(0,0,0,180));
            rect = option.rect.adjusted(startLeft+35,35,65,65);
            painter->drawPixmap(rect.left(), rect.top(),30,30 , pixmapNet );
        }
        if(isPlay==true){
            // 재생중 아이콘 배경
            if(itemType==QueueItemTypes::Music){
                rect = option.rect.adjusted(startLeft+5,5,-435+startLeft,-5);
            }else if(itemType==QueueItemTypes::Rosetube){
                rect = option.rect.adjusted(startLeft+5,18,-435+startLeft,-18);
            }
            else if(itemType==QueueItemTypes::Tidal){
                rect = option.rect.adjusted(startLeft+5,5,-435+startLeft,-5);
            }
            else if(itemType==QueueItemTypes::Qobuz){
                rect = option.rect.adjusted(startLeft+5,5,-435+startLeft,-5);
            }
            else if(itemType==QueueItemTypes::Bugs){
                rect = option.rect.adjusted(startLeft+5,5,-435+startLeft,-5);
            }
            painter->fillRect(rect, QColor(0,0,0,180));

            // 재생중 아이콘
            rect = option.rect.adjusted(startLeft+25,20,0,-20);
            painter->drawPixmap(rect.left(), rect.top(),24,26 , pixmapPlaying );
        }
    }
    if(tmp_author==""){

        // 제목
        rect = option.rect.adjusted(startLeft+thumbnail_width+space,25,0,0);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                          , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_title, tmp_titleWidth, font_16), &rect);

    }else if(tmp_author!=""){

        // 제목
        rect = option.rect.adjusted(startLeft+thumbnail_width+space,12,0,-5);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                          , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_title, tmp_titleWidth, font_16), &rect);

        // 아티스트
        rect = option.rect.adjusted(startLeft+thumbnail_width+space,35,0,0);
        painter->setPen(authorPen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()
                          , Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_author, tmp_titleWidth, font_16), &rect);
    }

    if(isEditMode==true){
        // 휴지통
        rect = option.rect.adjusted(total_width_min-50,15,0,0);
        painter->drawPixmap(rect.left(), rect.top(),40,40 , pixmapDel );
    }
    /*
    // 더보기
    rect = option.rect.adjusted(startLeft+width-60,0,0,0);
    painter->drawPixmap(rect.left(), rect.top(),60,70 , pixmapMore );
    */
}

/**
 * @brief QueueItemDelegate::sizeHint : 싸이즈 힌트
 * @note width 값은 min 값 세팅
 * @param option
 * @param index
 * @return
 */
QSize QueueItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(option.rect.width(),total_height);
}

/**
 * @brief QueueItemDelegate::ellipseString : Text ellipse
 * @details Text가 정해진 영역보다 긴 경우 ... 표기
 * @param p_str
 * @param p_width
 * @param p_font
 * @return
 */
QString QueueItemDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
    QFontMetrics metrics(p_font);
    return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
}

/**
 * @brief QueueItemDelegate::editorEvent : 마우스 이벤트 처리
 * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
 * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
 * @param event
 * @param model
 * @param option
 * @param index
 * @return
 */
bool QueueItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *eventMouse = (QMouseEvent*)event;

    if(eventMouse->button() == Qt::LeftButton){

        QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
        int tmp_index       = map[QueueItemDelegate_C::KEY_INDEX].toInt();
        bool isEditMode     = map[QueueItemDelegate_C::KEY_ISEDITMode].toBool();
        //bool isChecked          = map[QueueItemDelegate_C::KEY_ISCHECKED].toBool();
        int tmp_pressBtn = index.data(Qt::UserRole+1).toInt();

        int startLeft = 0;
        if(isEditMode==true){
            startLeft = checkbox_width;
        }

        //QRect rectBtn_checkbox = option.rect.adjusted(0,0,-total_width_min+startLeft,0);
        //QRect rectBtn_more = option.rect.adjusted(option.rect.width()-60,0,0,0);
        QRect rectBtn_del = option.rect.adjusted(option.rect.width()-50,0,0,0);
        if (event->type() == QEvent::MouseButtonPress){
            /*
            if(rectBtn_checkbox.contains(eventMouse->pos())){
                model->setData(index, BtnType::checkBox, Qt::UserRole+1);
            }
            else if(rectBtn_heart.contains(eventMouse->pos())){
                model->setData(index, BtnType::trackHeart, Qt::UserRole+1);
            }
            else */if(rectBtn_del.contains(eventMouse->pos())){
                model->setData(index, BtnType::del, Qt::UserRole+1);
            }else{
                model->setData(index, BtnType::etc, Qt::UserRole+1);
            }
        }else if(event->type() == QEvent::MouseButtonRelease){
            /*if(rectBtn_checkbox.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::checkBox){
                emit signal_clickedBtn(tmp_index, BtnType::checkBox);
            }
            else if(rectBtn_heart.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::trackHeart){
                emit signal_clickedBtn(tmp_index, BtnType::trackHeart);
            }
            else */if(rectBtn_del.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::del){
                if(isEditMode==true){
                    emit signal_clickedBtn(tmp_index, BtnType::del);
                }
            }
            else if(tmp_pressBtn==BtnType::etc){

                // 전체row클릭
                if(isEditMode==true){
                    emit signal_clickedBtn(tmp_index, BtnType::checkBox);
                }else{
                    emit signal_clickedBtn(tmp_index, BtnType::etc);
                }
            }
            model->setData(index, BtnType::none, Qt::UserRole+1);
        }

    }
     return true;
}
