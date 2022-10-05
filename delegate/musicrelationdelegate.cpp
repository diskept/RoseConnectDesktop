#include "musicrelationdelegate.h"
#include <QEvent>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QDebug>

const int thumbnail_width = 60;
const int space = 20;
const int stretch_width = 700;        // 1920 화면 디자인 기준
const int static_width = 160;          // 1920 화면 디자인 기준으로 여백등의 고정 값
const int title_width = 460;            // 1920 화면 디자인 기준

/**
 * @brief MusicRelationDelegate::MusicRelationDelegate : 음악 연관컨텐츠 목록 delegate
 * @param parent
 */
MusicRelationDelegate::MusicRelationDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    setInit();
}

/**
 * @brief MusicRelationDelegate::setInit : 초기 세팅
 */
void MusicRelationDelegate::setInit(){

    pixmapMore.load(":/images/icon_menu.png");
    pixmapHeartOn.load(":/images/list_fav_icon_on.png");
    pixmapHeartOff.load(":/images/list_fav_icon.png");
    pixmapPlay.load(":/images/icon_list_play.png");

}

/**
 * @brief MusicRelationDelegate::paint : 페인트 Draw
 * @param painter
 * @param option
 * @param index
 */
void MusicRelationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    int width = rect.width();

    // TEXT Width 조정
    int tmp_titleWidth = (width-static_width)*title_width/(stretch_width-static_width);

    // Get Data
    QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
    QString tmp_fileName = map["fileName"].toString();
    QString tmp_filePath = map["filePath"].toString();
    QString tmp_title = map["title"].toString();
    QString tmp_author = map["author"].toString();
    QString tmp_album = map["albumTitle"].toString();
    QString tmp_duration = map["duration"].toString();
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

        if(option.state & QStyle::State_MouseOver){
            rect = option.rect.adjusted(0,5,0,-5);
            painter->fillRect(rect, QColor(51,51,51,100));
            rect = option.rect.adjusted(15,20,0,-20);
            painter->drawPixmap(rect.left(), rect.top(),30,30 , pixmapPlay );
        }
    }

    // Font 설정 : 16px 고정
    QFont font_16;
    font_16.setPixelSize(16);
    painter->setFont(font_16);




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


    // 하트
    rect = option.rect.adjusted(width-120,0,-60,0);
    painter->drawPixmap(rect.left(), rect.top(),rect.width(),rect.height() , tmp_favorites ? pixmapHeartOn : pixmapHeartOff );


    // 더보기
    rect = option.rect.adjusted(width-60,0,0,0);
    painter->drawPixmap(rect.left(), rect.top(),60,70 , pixmapMore );
}

/**
 * @brief MusicRelationDelegate::sizeHint : 싸이즈 힌트
 * @note width 값은 min 값 세팅
 * @param option
 * @param index
 * @return
 */
QSize MusicRelationDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option);
    Q_UNUSED(index);
    //return QSize(944,70);
    return QSize(option.rect.width(), 70);
}

/**
 * @brief MusicRelationDelegate::ellipseString : Text ellipse
 * @details Text가 정해진 영역보다 긴 경우 ... 표기
 * @param p_str
 * @param p_width
 * @param p_font
 * @return
 */
QString MusicRelationDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
    QFontMetrics metrics(p_font);
    return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
}

/**
 * @brief MusicRelationDelegate::editorEvent : 마우스 이벤트 처리
 * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
 * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
 * @param event
 * @param model
 * @param option
 * @param index
 * @return
 */
bool MusicRelationDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *eventMouse = (QMouseEvent*)event;

    if(eventMouse->button() == Qt::LeftButton){

        QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
        int tmp_index = map["index"].toInt();
        int tmp_pressBtn = index.data(Qt::UserRole+1).toInt();

        QRect rectBtn_2 = option.rect.adjusted(option.rect.width()-120,0,-60,0);
        QRect rectBtn_3 = option.rect.adjusted(option.rect.width()-60,0,0,0);

        if (event->type() == QEvent::MouseButtonPress){
            if(rectBtn_2.contains(eventMouse->pos())){
                model->setData(index, BtnType::trackHeart, Qt::UserRole+1);
            }
            else if(rectBtn_3.contains(eventMouse->pos())){
                model->setData(index, BtnType::more, Qt::UserRole+1);

            }else{
                model->setData(index, BtnType::etc, Qt::UserRole+1);
            }
        }else if(event->type() == QEvent::MouseButtonRelease){
            if(rectBtn_2.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::trackHeart){
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
