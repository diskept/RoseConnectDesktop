#include "QobuzAlbumDelegate.h"
#include <QEvent>
#include <QDateTime>
#include <QJsonObject>
#include <QMouseEvent>
#include "qobuz/qobuz_struct.h"
#include "qobuz/ConvertData_forQobuz.h"

namespace qobuz {

    // 고정 사이즈
    const int THUMBNAIL_WIDTH = 60;         ///< 썸네일 이미지 사이즈 (정사각형)

    // Strech 영역의 공간 비율
    const float RATE_TITLE_OF_STRETCH_WIDTH = 0.6f;           ///< Stretch 영역에서 Duration 제외한 공간에 대하여 Title 공간 비율. 나머지 비율은 Album 차지
    const float RATE_DURATION_OF_STRETCH_WIDTH = 0.1f;        ///< Stretch 영역 중 Duration 공간 비율
    const int DURATION_WIDTH_MIN = 60;                      ///< Duration 데이터의 최소 보장 사이즈



    /**
     * @brief QobuzTrackDelegate::QobuzTrackDelegate
     * @param flagShowBtn_addCollection
     * @param parent
     */
    QobuzAlbumDelegate::QobuzAlbumDelegate(int section, UseMode useMode, QObject *parent)
        : AbstractTidalTrackDelegate(section, useMode, parent) {
        pixmap_hires.load(":images/qobuz/hires_ico.png");
    }

    /**
     * @brief QobuzTrackDelegate::paint
     * @param painter
     * @param option
     * @param index
     */
    void QobuzAlbumDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

        // 부모 먼저 호출
        AbstractTidalTrackDelegate::paint(painter, option, index);

        QRect rect = option.rect;
        int width = rect.width();

        // TEXT Width 조정
        int space_fixed = SPACE + THUMBNAIL_WIDTH + SPACE;      // 앞에 썸네일 영역 고정 (이 클래스에서만)
        int tmp_stretch_width = width - this->fixed_whole_width - space_fixed;


        // Duration Width 결정
        /*int tmp_durationWidth = (int)(tmp_stretch_width * RATE_DURATION_OF_STRETCH_WIDTH);
        if(tmp_durationWidth < DURATION_WIDTH_MIN){
            tmp_durationWidth = DURATION_WIDTH_MIN;
        }
        tmp_stretch_width -= tmp_durationWidth;*/

        // Title, Album Width 결정
        int tmp_titleWidth = (int)(tmp_stretch_width * RATE_TITLE_OF_STRETCH_WIDTH);
        int tmp_albumWidth = tmp_stretch_width - tmp_titleWidth - SPACE;



        QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);
        QPen authorPen(QColor::fromRgb(153,153,153), 1, Qt::SolidLine);

        // Font 설정 : 16px 고정
        QFont font_16;
        font_16.setPixelSize(16);

        // to debug
        //qobuz::TrackItemData data = ConvertData::convertData_trackItemData(index.data(Qt::UserRole).toJsonObject());
        //QString txt_duration = QDateTime::fromTime_t(data.duration).toUTC().toString("m:ss");       // custom text

        qobuz::AlbumItemData data = ConvertData::convertData_albumData(index.data(Qt::UserRole).toJsonObject());


        // 썸네일 Draw
        QIcon thumbNail = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
        if(!thumbNail.isNull()){
            // 1. 썸네일 Draw
            rect = option.rect.adjusted(5,5,THUMBNAIL_WIDTH,-5);
            thumbNail.paint(painter, rect, Qt::AlignVCenter);
        }


        // 폰트 재설정
        painter->setFont(font_16);

        // 타이틀
        rect = option.rect.adjusted(THUMBNAIL_WIDTH+SPACE, 16, 0, -5);          // 16 : top 위치, -5 : bottom으로부터의 위치. (마이너스로 표기하면 됨)
        painter->setPen(titlePen);
        //painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.title, tmp_titleWidth, font_16), &rect);
        painter->drawText(rect.left(), rect.top(), tmp_albumWidth, rect.height(), Qt::AlignLeft, ellipseString(data.title, tmp_albumWidth, font_16), &rect);

        // HIRES image
        if(data.hires == true){
            int title_width = THUMBNAIL_WIDTH + SPACE + rect.width() + 10;
            rect = option.rect.adjusted(title_width, 15, -width + title_width + 20, -35);
            painter->drawPixmap(rect , pixmap_hires);
            //qDebug() << rect.width() << rect.height();
        }

        // 아티스트
        rect = option.rect.adjusted(THUMBNAIL_WIDTH+SPACE, 39, 0, -13);
        painter->setPen(authorPen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.list_artist_name.join(","), tmp_titleWidth, font_16), &rect);

        // Album Name
        /*rect = option.rect.adjusted(THUMBNAIL_WIDTH+SPACE+tmp_titleWidth+SPACE, 26, 0, -26);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_albumWidth, rect.height(), Qt::AlignLeft, ellipseString(data.title, tmp_albumWidth, font_16), &rect);*/

        // Duration
        /*rect = option.rect.adjusted(width-tmp_durationWidth-this->fixed_whole_width,26,-this->fixed_whole_width,-26);
        painter->setPen(titlePen);
        painter->drawText(rect, Qt::AlignCenter, txt_duration);*/

    }


}
