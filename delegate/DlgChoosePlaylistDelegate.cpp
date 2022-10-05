#include "DlgChoosePlaylistDelegate.h"
#include "common/global.h"

#include <QDateTime>
#include <QEvent>
#include <QJsonObject>
#include <QMouseEvent>
#include "tidal/tidal_struct.h"
#include "tidal/ConvertData.h"


namespace tidal {

    // 고정 사이즈
    const int ITEM_HEIGHT = 80;
    const int THUMBNAIL_WIDTH = 60;
    const int SPACE_IMAGE_AND_TEXT = 20;
    const int MARGIN_WIDTH = 10;


    /**
     * @brief AbstractQobuzTrackDelegate::AbstractQobuzTrackDelegate
     * @param flagShowBtn_addCollection
     * @param parent
     */
    DlgChoosePlaylistDelegate::DlgChoosePlaylistDelegate(QObject *parent)
        : QAbstractItemDelegate(parent)
    {
        setInit();
    }


    /**
     * @brief AbstractQobuzTrackDelegate::setInit
     */
    void DlgChoosePlaylistDelegate::setInit(){
        this->pixmap_new.load(":images/playlist_add_icon.png");
        this->pixmap_empty.load(":images/playlist-def.png");
    }


    /**
     * @brief AbstractQobuzTrackDelegate::paint
     * @param painter
     * @param option
     * @param index
     */
    void DlgChoosePlaylistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

        Q_UNUSED(index);

        QRect rect = option.rect;
        int tmp_textWidth = rect.width() - THUMBNAIL_WIDTH - SPACE_IMAGE_AND_TEXT;


        // Line Draw
        QPen pen_line(QColor::fromRgb(77,77,77), 1, Qt::SolidLine);          // #4d4d4d
        painter->setPen(pen_line);
        QPoint point_start = rect.bottomLeft();
        QPoint point_end = rect.bottomRight();
        point_start.setX(point_start.x() + MARGIN_WIDTH);
        point_end.setX(point_end.x() - MARGIN_WIDTH);
        painter->drawLine(point_start, point_end);



        QPen pen_title(QColor::fromRgb(204,204,204), 1, Qt::SolidLine);              // #cccccc
        QPen pen_trackCount(QColor::fromRgb(119,119,119), 1, Qt::SolidLine);         // #777777

        // Font 설정 : 16px 고정
        QFont font_16;
        font_16.setPixelSize(16);
        painter->setFont(font_16);

        // to debug
        tidal::PlaylistItemData data = tidal::ConvertData::convertData_playlistData(index.data(Qt::UserRole).toJsonObject());


        // 썸네일 Draw
        QIcon thumbNail = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
        if(!thumbNail.isNull()){
            // 1. 썸네일 Draw
            rect = option.rect.adjusted(MARGIN_WIDTH, 10, MARGIN_WIDTH+THUMBNAIL_WIDTH, -10);
            thumbNail.paint(painter, rect, Qt::AlignVCenter);
        }


        // 신규 플레이리스트 추가 인 경우
        if(data.uuid.isEmpty()){
            // Main Label
            rect = option.rect.adjusted(MARGIN_WIDTH+THUMBNAIL_WIDTH+SPACE_IMAGE_AND_TEXT, 31, -MARGIN_WIDTH, -31);
            painter->setPen(pen_title);
            painter->drawText(rect.left(), rect.top(), tmp_textWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.title, tmp_textWidth, font_16), &rect);
        }
        else{
            rect = option.rect.adjusted(MARGIN_WIDTH+THUMBNAIL_WIDTH+SPACE_IMAGE_AND_TEXT, 18, -MARGIN_WIDTH, -44);
            painter->setPen(pen_title);
            painter->drawText(rect.left(), rect.top(), tmp_textWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.title, tmp_textWidth, font_16), &rect);

            // the number of tracks
            rect = option.rect.adjusted(MARGIN_WIDTH+THUMBNAIL_WIDTH+SPACE_IMAGE_AND_TEXT, 45, -MARGIN_WIDTH, -17);
            painter->setPen(pen_trackCount);

            QString track_count = "";
            if(global.lang == 0){
                if(data.numberOfTracks <= 1){
                    track_count = QString("%1 Song").arg(data.numberOfTracks);
                }
                else{
                    track_count = QString("%1 Songs").arg(data.numberOfTracks);
                }
            }
            else if(global.lang == 1){
                track_count = QString("%1 곡").arg(data.numberOfTracks);
            }

            painter->drawText(rect.left(), rect.top(), tmp_textWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(track_count, tmp_textWidth, font_16), &rect);
        }

    }


    /**
     * @brief AbstractQobuzTrackDelegate::sizeHint
     * @param option
     * @param index
     * @return
     */
    QSize DlgChoosePlaylistDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
        Q_UNUSED(option);
        Q_UNUSED(index);
        return QSize(option.rect.width(), ITEM_HEIGHT);
    }


    /**
     * @brief Text가 정해진 영역보다 긴 경우 ... 표기
     * @param p_str
     * @param p_width
     * @param p_font
     * @return
     */
    QString DlgChoosePlaylistDelegate::ellipseString(const QString &p_str, const int &p_width, const QFont &p_font) const{
        QFontMetrics metrics(p_font);
        return metrics.elidedText(QString(p_str), Qt::ElideRight, p_width-10);
    }



    /**
     * @brief DlgChoosePlaylistDelegate::editorEvent
     * @param event
     * @param model
     * @param option
     * @param index
     * @return
     */
    bool DlgChoosePlaylistDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {

        QMouseEvent *eventMouse = (QMouseEvent*)event;
        if(eventMouse->button() == Qt::LeftButton){

            QMap<QString,QVariant> map = index.data(Qt::UserRole).toMap();
            bool tmp_pressBtn = index.data(Qt::UserRole+1).toBool();            // 이전에 클릭여부

            // 마우스 Event 별로 처리
            if(event->type() == QEvent::MouseButtonPress){
                if(option.rect.contains(eventMouse->pos())){
                    model->setData(index, true, Qt::UserRole+1);
                }
            }
            else if(event->type() == QEvent::MouseButtonRelease){
                if(tmp_pressBtn && option.rect.contains(eventMouse->pos())){
                    // emit signal
                    emit this->clicked(index.row());
                }

                model->setData(index, false, Qt::UserRole+1);       // init
            }
        }

        return true;
    }

}
