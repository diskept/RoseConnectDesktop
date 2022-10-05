#include "QobuzEditTrackDelegate.h"

#include <QJsonObject>
#include "qobuz/qobuz_struct.h"
#include "qobuz/ConvertData_forQobuz.h"

namespace qobuz {

    // 고정 사이즈
    const int THUMBNAIL_WIDTH = 60;         ///< 썸네일 이미지 사이즈 (정사각형)

    // Strech 영역의 공간 비율
    const float RATE_TITLE_OF_STRETCH_WIDTH = 0.6f;           ///< Stretch 영역에서 Duration 제외한 공간에 대하여 Title 공간 비율. 나머지 비율은 Album 차지
//    const float RATE_DURATION_OF_STRETCH_WIDTH = 0.1f;        ///< Stretch 영역 중 Duration 공간 비율
//    const int DURATION_WIDTH_MIN = 60;                      ///< Duration 데이터의 최소 보장 사이즈



    /**
     * @brief QobuzEditTrackDelegate::QobuzEditTrackDelegate
     * @param parent
     */
    QobuzEditTrackDelegate::QobuzEditTrackDelegate(QObject *parent)
        : AbstractEditListDelegate(parent) {

    }

    /**
     * @brief QobuzTrackDelegate::paint
     * @param painter
     * @param option
     * @param index
     */
    void QobuzEditTrackDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

        // 부모 먼저 호출
        AbstractEditListDelegate::paint(painter, option, index);

        QRect rect = option.rect;
        int width = rect.width();

        // TEXT Width 조정
        int space_fixed = SIZE_CHECKBOX + THUMBNAIL_WIDTH + SPACE;      // 앞에 썸네일 영역 고정 (이 클래스에서만)
        int tmp_stretch_width = width - this->fixed_whole_width - space_fixed;


        // Title, Album Width 결정
        int tmp_titleWidth = (int)(tmp_stretch_width * RATE_TITLE_OF_STRETCH_WIDTH);
        int tmp_albumWidth = tmp_stretch_width - tmp_titleWidth - SPACE;


        QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);
        QPen authorPen(QColor::fromRgb(153,153,153), 1, Qt::SolidLine);

        // Font 설정 : 16px 고정
        QFont font_16;
        font_16.setPixelSize(16);

        // to debug
        editDelegate::ItemEditData data = AbstractEditListDelegate::convertData_itemEditData(index.data(Qt::UserRole).toJsonObject());

//        QPixmap tmp_pixmap_checked = this->pixmapCheckOff;
//        if(data.flagChecked){
//            rect = option.rect.adjusted(0,0,0,-1);
//            painter->fillRect(rect, QColor(33,33,33));
//            tmp_pixmap_checked = pixmapCheckOn;
//        }
//        rect = option.rect.adjusted(0,(ITEM_ALL_HEIGHT-SIZE_CHECKBOX)/2,0,0);
//        painter->drawPixmap(rect.left(), rect.top(),SIZE_CHECKBOX,SIZE_CHECKBOX , tmp_pixmap_checked );


        // 썸네일 Draw
        QIcon thumbNail = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
        if(!thumbNail.isNull()){
            // 1. 썸네일 Draw
            rect = option.rect.adjusted(SIZE_CHECKBOX,5,THUMBNAIL_WIDTH,-5);
            thumbNail.paint(painter, rect, Qt::AlignVCenter);
        }


        // 폰트 재설정
        painter->setFont(font_16);

        // 타이틀
        rect = option.rect.adjusted(SIZE_CHECKBOX+THUMBNAIL_WIDTH+SPACE, 16, 0, -5);          // 16 : top 위치, -5 : bottom으로부터의 위치. (마이너스로 표기하면 됨)
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.track_title, tmp_titleWidth, font_16), &rect);

        // 아티스트
        rect = option.rect.adjusted(SIZE_CHECKBOX+THUMBNAIL_WIDTH+SPACE, 39, 0, -13);
        painter->setPen(authorPen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.artist_name, tmp_titleWidth, font_16), &rect);

        // Album Name
        rect = option.rect.adjusted(SIZE_CHECKBOX+THUMBNAIL_WIDTH+SPACE+tmp_titleWidth+SPACE, 26, 0, -26);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_albumWidth, rect.height(), Qt::AlignLeft, ellipseString(data.album_title, tmp_albumWidth, font_16), &rect);


    }


}
