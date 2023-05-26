#include "cloudListDelegate.h"
#include <QRect>
#include <QMouseEvent>
#include <QDebug>

const int thumbnail_width = 60;
const int space = 20;
const int stretch_width = 1620;        // 1920 화면 디자인 기준
const int static_width = 260;          // 1920 화면 디자인 기준으로 여백등의 고정 값
const int title_width = 1080;            // 1920 화면 디자인 기준
const int informaition_width = 300;         // 1920 화면 디자인 기준
const int informaition_width_min = 100;

/**
 * @brief cloudListDelegate::cloudListDelegate : USB 파일정보 delegate
 * @param parent
 */
cloudListDelegate::cloudListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    this->setInit();
}


void cloudListDelegate::setInit(){
    this->pixmapMore.load(":/images/icon_menu.png");
    this->pixmapNext.load(":/images/popup_more_ico.png");
}


/**
 * @brief cloudListDelegate::paint : 페인트 Draw
 * @details 파일타입(Dir or file) 에 따라 draw 다름<br>
 * if Dir, 네트워크
 * @param painter
 * @param option
 * @param index
 */
void cloudListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    int width = rect.width();

    // TEXT Width 조정
    // 1920 디자인 시안 고려(각 항목 spacing 20 고려)

    int tmp_adjustWidth = 0;
    int tmp_informationWidth = (width-static_width) * informaition_width / (stretch_width-static_width);
    if(tmp_informationWidth < informaition_width_min){
        tmp_adjustWidth = informaition_width_min-tmp_informationWidth;
        tmp_informationWidth = informaition_width_min;
    }
    int tmp_titleWidth = (width-static_width)*title_width/(stretch_width-static_width) - tmp_adjustWidth/2;


    // Get Data
    QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
    QString tmp_fileName = map["fileName"].toString();
    QString tmp_humanReadableModTime = map["humanReadableModTime"].toString();
    QString tmp_size = map["size"].toString();
    bool tmp_flagIsFile = map["isFile"].toBool();


    // Set Hover Background color
    if(option.state & QStyle::State_MouseOver){
        painter->fillRect(rect, QColor(51,51,51));
    }


    // pen 정의
    QPen linePen(QColor::fromRgb(42,42,42), 1, Qt::SolidLine);
    QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);
    QPen timePen(QColor::fromRgb(177,134,88),1,Qt::SolidLine);


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


    if(tmp_fileName != ""){
        // 폴더명
        rect = option.rect.adjusted(thumbnail_width+space,0,0,0);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignVCenter|Qt::AlignLeft, ellipseString(tmp_fileName, tmp_titleWidth, font_16), &rect);
    }

    // 파일일때 시간 , 용량표시
    if(tmp_flagIsFile && tmp_humanReadableModTime != ""){
        rect = option.rect.adjusted(((width-60)*80)/100,0,0,0);
        painter->setPen(timePen);
        painter->drawText(rect.left(), rect.top(), tmp_informationWidth, rect.height(), Qt::AlignVCenter|Qt::AlignLeft, ellipseString(tmp_humanReadableModTime + tmp_size, tmp_informationWidth, font_16), &rect);

        // more 버튼 표시
        rect = option.rect.adjusted(width-60,0,0,0);
        painter->drawPixmap(rect.left(), rect.top(),60,70 , pixmapMore);
    }    
    // 폳더일때 시간표시
    else{
        rect = option.rect.adjusted(((width-60)*80)/100,0,0,0);
        painter->setPen(timePen);
        painter->drawText(rect.left(), rect.top(), tmp_informationWidth, rect.height(), Qt::AlignVCenter|Qt::AlignLeft, ellipseString(tmp_humanReadableModTime, tmp_informationWidth, font_16), &rect);

        // next 버튼 표시
        rect = option.rect.adjusted(width-60,0,0,0);
        painter->drawPixmap(rect.left(), rect.top(),60,70 , pixmapNext);
    }
}

/**
 * @brief cloudListDelegate::sizeHint : 싸이즈 힌트
 * @note width 값은 min 값 세팅
 * @param option
 * @param index
 * @return
 */
QSize cloudListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option);
    Q_UNUSED(index);
    //return QSize(944,70);
    return QSize(option.rect.width(), 70);
}

/**
 * @brief cloudListDelegate::ellipseString : Text ellipse
 * @details Text가 정해진 영역보다 긴 경우 ... 표기
 * @param p_str
 * @param p_width
 * @param p_font
 * @return
 */
QString cloudListDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
    QFontMetrics metrics(p_font);
    return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
}



/**
 * @brief cloudListDelegate::editorEvent : 마우스 이벤트 처리
 * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
 * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
 * @param event
 * @param model
 * @param option
 * @param index
 * @return
 */
bool cloudListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *eventMouse = (QMouseEvent*)event;

    if(eventMouse->button() == Qt::LeftButton){

        QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
        int tmp_index = map["index"].toInt();
        bool tmp_flagIsFile = map["isFile"].toBool();
        int tmp_pressBtn = index.data(Qt::UserRole+1).toInt();

        QRect rectBtn = option.rect.adjusted(option.rect.width()-60,0,0,0);

        if(event->type() == QEvent::MouseButtonPress){
            if(rectBtn.contains(eventMouse->pos()) ){
                //if(tmp_flagIsFile && flagIsMusicFileMode){
                if(tmp_flagIsFile){
                    model->setData(index, BtnType::more, Qt::UserRole+1);
                }
            }
            else{
                model->setData(index, BtnType::etc, Qt::UserRole+1);
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease){

            //if(rectBtn.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::more && flagIsMusicFileMode){
            if(rectBtn.contains(eventMouse->pos()) && tmp_pressBtn==BtnType::more){
                //emit signal_clickedBtn(tmp_index, BtnType::more);
                emit signal_clickedBtn(tmp_flagIsFile, tmp_index, BtnType::more);
            }
            else if(tmp_pressBtn==BtnType::etc){
                //emit signal_clickedBtn(tmp_index, BtnType::etc);
                emit signal_clickedBtn(tmp_flagIsFile, tmp_index, BtnType::etc);
            }
            model->setData(index, BtnType::none, Qt::UserRole+1);
        }

    }

    return true;
}

/**
 * @brief cloudListDelegate::setHiddenOptionBtn : 옵션버튼 hide
 * @note 플레이리스트담기, 하트, 더보기
 */
void cloudListDelegate::setVideoFileMode(){
    flagIsMusicFileMode = false;
}
