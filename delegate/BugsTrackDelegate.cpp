#include "BugsTrackDelegate.h"
#include <QEvent>
#include <QJsonObject>
#include <QMouseEvent>
#include "bugs/bugs_struct.h"
#include "bugs/ConvertData_forBugs.h"

namespace bugs {

    // 고정 사이즈
//    const int SPACE = 20;                   ///< 아이템 간의 간격. 변경하려면 AbstractTidalTrackDelegate 와도 맞출것.
    const int THUMBNAIL_WIDTH = 60;         ///< 썸네일 이미지 사이즈 (정사각형)
    const int TRACK_RANK_WIDTH = 60;        ///< 트랙 Rank 텍스트 영역

//    const int BTN_ICON_WIDTH = 60;          ///< 우측에 아이콘 버튼 사이즈. (정사각형, 모두 같은 사이즈). 총 버튼 3개

    // Strech 영역의 공간 비율
    const float RATE_TITLE_OF_STRETCH_WIDTH = 0.6f;           ///< Stretch 영역에서 Duration 제외한 공간에 대하여 Title 공간 비율. 나머지 비율은 Album 차지
    const float RATE_DURATION_OF_STRETCH_WIDTH = 0.1f;        ///< Stretch 영역 중 Duration 공간 비율
    const int DURATION_WIDTH_MIN = 60;                      ///< Duration 데이터의 최소 보장 사이즈



    /**
     * @brief BugsTrackDelegate::BugsTrackDelegate
     * @param section
     * @param useBtnMode
     * @param parent
     */
    BugsTrackDelegate::BugsTrackDelegate(int section, UseMode useMode, QObject *parent)
        : AbstractTidalTrackDelegate(section, useMode, parent){

        this->flagShowRank = false;      // Track Rank 여부
        if( (useMode & BugsTrackDelegate::UseMode_Flag::TopRank) == BugsTrackDelegate::UseMode_Flag::TopRank){
            this->flagShowRank = true;
        }
    }


    /**
     * @brief TidalTrackDelegate::paint
     * @param painter
     * @param option
     * @param index
     */
    void BugsTrackDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

        // Data
        bugs::TrackItemData data = ConvertData_forBugs::convertData_trackData(index.data(Qt::UserRole).toJsonObject());

        bool isMusicVideo = data.list_mv_id.count() > 0 ? true : false;

        // 부모 먼저 호출
        //AbstractTidalTrackDelegate::paint(painter, option, index);
        AbstractTidalTrackDelegate::bugs_paint(painter, option, index, isMusicVideo);

        QRect rect = option.rect;
        int width = rect.width();
        int mime_x = 0;

        // TEXT Width 조정
        int space_fixed = SPACE + THUMBNAIL_WIDTH + SPACE;      // 앞에 썸네일 영역 고정 (이 클래스에서만)
        if(this->flagShowRank){
            space_fixed += TRACK_RANK_WIDTH;
        }
        int tmp_stretch_width = width - this->fixed_whole_width - space_fixed;


        // Duration Width 결정
        int tmp_durationWidth = (int)(tmp_stretch_width * RATE_DURATION_OF_STRETCH_WIDTH);
        if(tmp_durationWidth < DURATION_WIDTH_MIN){
            tmp_durationWidth = DURATION_WIDTH_MIN;
        }
        tmp_stretch_width -= tmp_durationWidth;

        // Title, Album Width 결정
        int tmp_titleWidth = (int)(tmp_stretch_width * RATE_TITLE_OF_STRETCH_WIDTH);
        int tmp_albumWidth = tmp_stretch_width - tmp_titleWidth - SPACE;


        QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);
        QPen authorPen(QColor::fromRgb(153,153,153), 1, Qt::SolidLine);
        QPen mimePen(QColor::fromRgb(80,80,80), 1, Qt::SolidLine);

        // Font 설정
        QFont font_18;
        font_18.setPixelSize(18);
        QFont font_16;
        font_16.setPixelSize(16);
        QFont font_14;
        font_14.setPixelSize(14);

        // Data
        //bugs::TrackItemData data = ConvertData_forBugs::convertData_trackData(index.data(Qt::UserRole).toJsonObject());


        // Fav on/off - with data
        if(this->flagShowBtn_like){
            int tmp_x1 = width - BTN_ICON_WIDTH*2 - SPACE;          // 오른쪽에 more 버튼은 항상 있으므로
            int tmp_x2 = -SPACE - BTN_ICON_WIDTH;
            rect = option.rect.adjusted(tmp_x1,0,tmp_x2,0);
            if(data.likes_yn){
                painter->drawPixmap(rect , this->pixmap_fav_on);
            }
            else{
                painter->drawPixmap(rect , this->pixmap_fav_off);
            }
        }


        // 썸네일 Draw
        QIcon thumbNail = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
        if(!thumbNail.isNull()){
            // 1. 썸네일 Draw
            rect = option.rect.adjusted(5,5,THUMBNAIL_WIDTH,-5);
            thumbNail.paint(painter, rect, Qt::AlignVCenter);
        }


        int tmp_x1 = 5 + THUMBNAIL_WIDTH;
        if(this->flagShowRank){
            // 벅스 Rank
            painter->setFont(font_18);      // 폰트 재설정
            rect = option.rect.adjusted(tmp_x1, 25, 0, -25);          // 16 : top 위치, -5 : bottom으로부터의 위치. (마이너스로 표기하면 됨)
            painter->setPen(titlePen);
            painter->drawText(rect.left(), rect.top(), TRACK_RANK_WIDTH, rect.height(), Qt::AlignCenter|Qt::AlignVCenter, ellipseString(QString("%1").arg(index.row()+1), TRACK_RANK_WIDTH, font_16), &rect);

            tmp_x1 += TRACK_RANK_WIDTH; // next
        }
        else{
            tmp_x1 += SPACE; // next
        }

        // 폰트 재설정
        painter->setFont(font_16);

        // 타이틀
        rect = option.rect.adjusted(tmp_x1, 16, 0, -5);          // 16 : top 위치, -5 : bottom으로부터의 위치. (마이너스로 표기하면 됨)
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.track_title, tmp_titleWidth, font_16), &rect);

        // 해상도
        if(data.bitrates != ""){
            if(tmp_titleWidth < rect.width()){
                mime_x = tmp_titleWidth;
            }
            else{
                mime_x = rect.width();
            }

            rect = option.rect.adjusted(tmp_x1 + mime_x + 15, 15, 0, -38);
            painter->setPen(mimePen);
            painter->drawText(rect.left()+8, rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.bitrates, tmp_titleWidth, font_14), &rect);
            painter->drawRoundedRect(rect.left()-8, rect.top()-1, rect.width()+14, rect.height()+2, 2, 2);
        }

        // 아티스트
        rect = option.rect.adjusted(tmp_x1, 39, 0, -13);
        painter->setPen(authorPen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.list_artist_nm.join(","), tmp_titleWidth, font_16), &rect);

        tmp_x1 += tmp_titleWidth + SPACE;       // next

        // Album Name
        rect = option.rect.adjusted(tmp_x1, 26, 0, -26);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_albumWidth, rect.height(), Qt::AlignLeft, ellipseString(data.album_title, tmp_albumWidth, font_16), &rect);


        // Duration
        rect = option.rect.adjusted(width-tmp_durationWidth-this->fixed_whole_width,26,-this->fixed_whole_width,-26);
        painter->setPen(titlePen);
        painter->drawText(rect, Qt::AlignCenter, data.len);

    }

}

