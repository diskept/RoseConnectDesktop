#include "TidalAlbumTrackDelegate.h"
#include "tidal/tidal_struct.h"
#include "tidal/ConvertData.h"

#include "common/gscommon.h"
#include <QWidget>


namespace tidal {

    // 고정 사이즈
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
    TidalAlbumTrackDelegate::TidalAlbumTrackDelegate(int section, UseMode useMode, QObject *parent)
        : AbstractTidalTrackDelegate(section, useMode, parent) {

        pixmap_play.load(":images/album_play_icon.png");
        pixmap_mqa.load(":images/tidal/mqa_ico.png");
    }

    /**
     * @brief TidalAlbumTrackDelegate::paint
     * @param painter
     * @param option
     * @param index
     */
    void TidalAlbumTrackDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

        // 부모 먼저 호출
        AbstractTidalTrackDelegate::paint(painter, option, index);


        QRect rect = option.rect;
        int width = rect.width();

        // TEXT Width 조정
        int space_fixed = SPACE + ICON_PLAY_WIDTH + SPACE;      // 앞에 썸네일 영역 고정 (이 클래스에서만)
        int tmp_stretch_width = width - this->fixed_whole_width - space_fixed;

        // Duration Width 결정
        int tmp_durationWidth = (int)(tmp_stretch_width * RATE_DURATION_OF_STRETCH_WIDTH);
        if(tmp_durationWidth < DURATION_WIDTH_MIN){
            tmp_durationWidth = DURATION_WIDTH_MIN;
        }
        tmp_stretch_width -= tmp_durationWidth;

        // to debug
        tidal::TrackItemData data = ConvertData::convertData_trackItemData(index.data(Qt::UserRole).toJsonObject());//cheon210812-work
        QString txt_duration = QDateTime::fromTime_t(data.duration).toUTC().toString("m:ss");       // custom text//cheon210812-work

        // Font 설정 : 16px 고정
        int tmp_titleWidth;//cheon210812-work
        QFont font_16;//cheon210812-work
        if(data.title.size() > 90){//cheon210812-work
            font_16.setPixelSize(15);//cheon210812-work
            data.title = data.title.insert(90, '\n');//cheon210812-work
            tmp_titleWidth = (int)(tmp_stretch_width * RATE_TITLE_OF_STRETCH_WIDTH)+250;//cheon210812-work
        }else{
            font_16.setPixelSize(15);//cheon210812-work
            tmp_titleWidth = (int)(tmp_stretch_width * RATE_TITLE_OF_STRETCH_WIDTH)+150;//cheon210812-work
        }

        // Title, Album Width 결정

        int tmp_artistWidth = tmp_stretch_width - tmp_titleWidth - SPACE;
        QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);


        // 폰트 재설정
        painter->setFont(font_16);

        // 제일 앞에 Play 아이콘
        rect = option.rect.adjusted(SPACE, 20, -width+SPACE+ICON_PLAY_WIDTH, -20);
        painter->drawPixmap(rect , pixmap_play);

        // 타이틀
        rect = option.rect.adjusted(space_fixed, 26, 0, -26);          // 16 : top 위치, -5 : bottom으로부터의 위치. (마이너스로 표기하면 됨)
        painter->setPen(titlePen);

        if(data.title.size() > 90){//cheon210812-work
            painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height()+25, Qt::AlignTop|Qt::AlignLeft, ellipseString(data.title, tmp_titleWidth, font_16), &rect);//cheon210812-work
        }else{
            painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.title, tmp_titleWidth, font_16), &rect);//cheon210812-work
        }

        //painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.title, tmp_titleWidth, font_16), &rect);

        //qDebug() << tmp_titleWidth << rect.width();

        // MQA image
        if(data.audioQuality == "HI_RES"){
            int title_width = 825-40 + 10;//cheon210812-work
            //int title_width = space_fixed + rect.width() + 10;

            rect = option.rect.adjusted(title_width, 28, -width + title_width + 36, -29);
            painter->drawPixmap(rect , pixmap_mqa);
            //qDebug() << rect.width() << rect.height();
        }

        // 아티스트
        //qDebug() << "space_fixed+tmp_titleWidth+SPACE=" << space_fixed+tmp_titleWidth+SPACE;//cheon210812-work
        rect = option.rect.adjusted(865, 26, 0, -26);//cheon210812-work
        //rect = option.rect.adjusted(space_fixed+tmp_titleWidth+SPACE, 26, 0, -26);//cheon210812-work
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_artistWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.list_artist_name.join(","), tmp_artistWidth, font_16), &rect);

        // Duration
        rect = option.rect.adjusted(width-tmp_durationWidth-this->fixed_whole_width,26,-this->fixed_whole_width,-26);
        painter->setPen(titlePen);
        painter->drawText(rect, Qt::AlignCenter, txt_duration);

    }

}
