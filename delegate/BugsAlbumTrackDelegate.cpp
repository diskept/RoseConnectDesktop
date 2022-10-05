#include "BugsAlbumTrackDelegate.h"
#include "bugs/bugs_struct.h"
#include "bugs/ConvertData_forBugs.h"


namespace bugs {

    // 고정 사이즈
//    const int SPACE = 20;                   ///< 아이템 간의 간격. 변경하려면 AbstractTidalTrackDelegate 와도 맞출것.
    const int ICON_PLAY_WIDTH = 30;         ///< 좌측 앞에 play 아이콘

    // Strech 영역의 공간 비율
    const float RATE_TITLE_OF_STRETCH_WIDTH = 0.6f;           ///< Stretch 영역에서 Duration 제외한 공간에 대하여 Title 공간 비율. 나머지 비율은 Artist 차지
    const float RATE_DURATION_OF_STRETCH_WIDTH = 0.1f;        ///< Stretch 영역 중 Duration 공간 비율
    const int DURATION_WIDTH_MIN = 60;                      ///< Duration 데이터의 최소 보장 사이즈


    /**
     * @brief TidalAlbumTrackDelegate::TidalAlbumTrackDelegate
     * @param section
     * @param flagShowBtn_addCollection
     * @param parent
     */
    BugsAlbumTrackDelegate::BugsAlbumTrackDelegate(int section, UseMode useMode, QObject *parent)
        : AbstractTidalTrackDelegate(section, useMode, parent) {

        pixmap_play.load(":images/album_play_icon.png");

    }

    /**
     * @brief TidalAlbumTrackDelegate::paint
     * @param painter
     * @param option
     * @param index
     */
    void BugsAlbumTrackDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

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
        int space_fixed = SPACE + ICON_PLAY_WIDTH + SPACE;      // 앞에 썸네일 영역 고정 (이 클래스에서만)
        int tmp_stretch_width = width - this->fixed_whole_width - space_fixed;

        // Duration Width 결정
        int tmp_durationWidth = (int)(tmp_stretch_width * RATE_DURATION_OF_STRETCH_WIDTH);
        if(tmp_durationWidth < DURATION_WIDTH_MIN){
            tmp_durationWidth = DURATION_WIDTH_MIN;
        }
        tmp_stretch_width -= tmp_durationWidth;

        // Title, Album Width 결정
        int tmp_titleWidth = (int)(tmp_stretch_width * RATE_TITLE_OF_STRETCH_WIDTH);
        int tmp_artistWidth = tmp_stretch_width - tmp_titleWidth - SPACE;



        QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);
        QPen mimePen(QColor::fromRgb(80,80,80), 1, Qt::SolidLine);

        // Font 설정 : 16px 고정
        QFont font_16;
        font_16.setPixelSize(16);
        QFont font_14;
        font_14.setPixelSize(14);

        // Data
        //bugs::TrackItemData data = ConvertData_forBugs::convertData_trackData(index.data(Qt::UserRole).toJsonObject());

        // 폰트 재설정
        painter->setFont(font_16);


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


        // 제일 앞에 Play 아이콘
        rect = option.rect.adjusted(SPACE, 20, -width+SPACE + ICON_PLAY_WIDTH, -20);
        painter->drawPixmap(rect , pixmap_play);

        // 타이틀
        rect = option.rect.adjusted(space_fixed, 26, 0, -26);          // 16 : top 위치, -5 : bottom으로부터의 위치. (마이너스로 표기하면 됨)
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

            rect = option.rect.adjusted(space_fixed + mime_x + 15, 26, 0, -26);
            painter->setPen(mimePen);
            painter->drawText(rect.left()+8, rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.bitrates, tmp_titleWidth, font_14), &rect);
            painter->drawRoundedRect(rect.left()-8, rect.top()-1, rect.width()+14, rect.height()+2, 2, 2);
        }

        // 아티스트
        rect = option.rect.adjusted(space_fixed+tmp_titleWidth+SPACE, 26, 0, -26);
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_artistWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.list_artist_nm.join(","), tmp_artistWidth, font_16), &rect);

        // Duration
        rect = option.rect.adjusted(width-tmp_durationWidth-this->fixed_whole_width,26,-this->fixed_whole_width,-26);
        painter->setPen(titlePen);
        painter->drawText(rect, Qt::AlignCenter, data.len);

    }

}
