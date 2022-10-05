#include "AbstractEditListDelegate.h"

#include <QDateTime>
#include <QDebug>
#include <QEvent>
#include <QJsonObject>
#include <QMouseEvent>
#include "common/ProcJsonEasy.h"


// 고정 사이즈
//    const int ITEM_ALL_WIDTH_MIN = 914;     ///< MIN WIDTH
/*const int ITEM_ALL_HEIGHT = 70;*/         ///< Track 아이템 Height
const int SIZE_TRASH_ICON = 40;


/**
 * @brief AbstractEditListDelegate::AbstractEditListDelegate
 * @param parent
 */
AbstractEditListDelegate::AbstractEditListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    setInit();
}


/**
 * @brief AbstractTidalTrackDelegate::setInit
 */
void AbstractEditListDelegate::setInit(){
    this->pixmapCheckOff.load(":images/playlist/list_check_off.png");
    this->pixmapCheckOn.load(":images/playlist/list_check_on.png");
    this->pixmapDel.load(":images/playlist/fol_del_ico.png");

    // 기본값
    this->fixed_whole_width = SPACE + BTN_ICON_WIDTH + SPACE;

}



/**
 * @brief AbstractTidalTrackDelegate::paint
 * @param painter
 * @param option
 * @param index
 */
void AbstractEditListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    Q_UNUSED(index);

    QRect rect = option.rect;
    int width = rect.width();


    // Line Draw
    QPen linePen(QColor::fromRgb(77,77,77), 1, Qt::SolidLine);          // #4d4d4d
    painter->setPen(linePen);
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());

    // to debug
    editDelegate::ItemEditData data = AbstractEditListDelegate::convertData_itemEditData(index.data(Qt::UserRole).toJsonObject());

    QPixmap tmp_pixmap_checked = this->pixmapCheckOff;
    if(data.flagChecked){
        rect = option.rect.adjusted(0,0,0,-1);
        painter->fillRect(rect, QColor(51,51,51));      // #333333
        tmp_pixmap_checked = pixmapCheckOn;
    }
    rect = option.rect.adjusted(0,(ITEM_ALL_HEIGHT-SIZE_CHECKBOX)/2,0,0);
    painter->drawPixmap(rect.left(), rect.top(),SIZE_CHECKBOX,SIZE_CHECKBOX , tmp_pixmap_checked );


    // 쓰레기 - the most right side
    int tmp_x1 = width-SIZE_TRASH_ICON-SPACE;
    int tmp_x2 = -SPACE;
    rect = option.rect.adjusted(tmp_x1,16,tmp_x2,-14);
    painter->drawPixmap(rect , this->pixmapDel);

}


/**
 * @brief AbstractTidalTrackDelegate::sizeHint
 * @param option
 * @param index
 * @return
 */
QSize AbstractEditListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(option.rect.width(), ITEM_ALL_HEIGHT);
}


/**
 * @brief AbstractTidalTrackDelegate::ellipseString : Text ellipse
 * @details Text가 정해진 영역보다 긴 경우 ... 표기
 * @param p_str
 * @param p_width
 * @param p_font
 * @return
 */
QString AbstractEditListDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
    QFontMetrics metrics(p_font);
    return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
}


/**
 * @brief AbstractTidalTrackDelegate::editorEvent : 마우스 이벤트 처리
 * @details 마우스 왼쪽 버튼 presse, release, pos 확인<br>
 * 파일 타입 확인 후 클릭한 버튼 관련 시그널 발생
 * @param event
 * @param model
 * @param option
 * @param index
 * @return
 */
bool AbstractEditListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {

    QMouseEvent *eventMouse = (QMouseEvent*)event;
    if(eventMouse->button() == Qt::LeftButton){

        QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
        int tmp_pressBtn = index.data(Qt::UserRole+1).toInt();              // 이전에 클릭했던 Btn.

        // init
        int tmp_x1 = option.rect.width();
        int tmp_x2 = 0;
        int tmp_width_allBtn = 0;

        QRect rectBtn_trash;
        QRect rectBtn_remain;

        // 쓰레기통 아이콘 클릭
        tmp_x1 -= BTN_ICON_WIDTH;
        rectBtn_trash = option.rect.adjusted(tmp_x1, 0, tmp_x2, 0);
        tmp_width_allBtn += BTN_ICON_WIDTH;


        // 나머지 영역
        rectBtn_remain = option.rect.adjusted(0, 0, -tmp_width_allBtn, 0);


        // 마우스 Event 별로 처리
        if(event->type() == QEvent::MouseButtonPress){
            if(rectBtn_trash.contains(eventMouse->pos())){
                model->setData(index, ClickMode::TrashBtn, Qt::UserRole+1);
            }
            else if(rectBtn_remain.contains(eventMouse->pos())){
                model->setData(index, ClickMode::CheckBox, Qt::UserRole+1);
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease){
            if(tmp_pressBtn==ClickMode::TrashBtn && rectBtn_trash.contains(eventMouse->pos())){
                emit this->clicked(index.row(), ClickMode::TrashBtn);
            }
            else if(tmp_pressBtn==ClickMode::CheckBox && rectBtn_remain.contains(eventMouse->pos())){
                emit this->clicked(index.row(), ClickMode::CheckBox);
            }

            model->setData(index, 0, Qt::UserRole+1);       // init
        }

    }

    return true;
}



// Convert about editDelegate::ItemEditData -----------------------------------------------------------------------------

/**
 * @brief ItemEditData struct 타입을 JsonObject로 반환
 * @param data
 * @return
 */
QJsonObject AbstractEditListDelegate::getObjectJson_itemEditData(const editDelegate::ItemEditData &data){
    QJsonObject jsonObj;
    jsonObj.insert("album_image", data.album_image);

    jsonObj.insert("track_title", data.track_title);
    jsonObj.insert("artist_name", data.artist_name);
    jsonObj.insert("album_title", data.album_title);

    jsonObj.insert("flagChecked", data.flagChecked);

    return jsonObj;
}

/**
 * @brief JsonObject을 ItemEditData struct 타입으로 반환
 * @param jsonObj
 * @return
 */
editDelegate::ItemEditData AbstractEditListDelegate::convertData_itemEditData(const QJsonObject &jsonObj){
    editDelegate::ItemEditData data_output;
    data_output.album_image = ProcJsonEasy::getString(jsonObj, "album_image");
    data_output.track_title = ProcJsonEasy::getString(jsonObj, "track_title");
    data_output.artist_name = ProcJsonEasy::getString(jsonObj, "artist_name");
    data_output.album_title = ProcJsonEasy::getString(jsonObj, "album_title");
    data_output.flagChecked = ProcJsonEasy::getBool(jsonObj, "flagChecked");

    return data_output;
}



