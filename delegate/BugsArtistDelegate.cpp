#include "BugsArtistDelegate.h"
#include <QEvent>
#include <QJsonObject>
#include <QMouseEvent>
#include "bugs/bugs_struct.h"
#include "bugs/ConvertData_forBugs.h"

namespace bugs {

    // 고정 사이즈
    const int THUMBNAIL_WIDTH = 60;         ///< 썸네일 이미지 사이즈 (정사각형)


    /**
     * @brief BugsArtistDelegate::BugsArtistDelegate
     * @param section
     * @param useBtnMode
     * @param parent
     */
    BugsArtistDelegate::BugsArtistDelegate(int section, UseMode useMode, QObject *parent)
        : AbstractTidalTrackDelegate(section, useMode, parent){

        this->flagShowRank = false;      // Track Rank 여부
        if( (useMode & BugsArtistDelegate::UseMode_Flag::TopRank) == BugsArtistDelegate::UseMode_Flag::TopRank){
            this->flagShowRank = true;
        }
    }



    /**
     * @brief TidalTrackDelegate::paint
     * @param painter
     * @param option
     * @param index
     */
    void BugsArtistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

        // 부모 먼저 호출
        AbstractTidalTrackDelegate::paint(painter, option, index);

        QRect rect = option.rect;
        int width = rect.width();


        // TEXT Width 조정
        int space_fixed = SPACE + THUMBNAIL_WIDTH + SPACE;      // 앞에 썸네일 영역 고정 (이 클래스에서만)
        int tmp_stretch_width = width - this->fixed_whole_width - space_fixed;


        // Title, Album Width 결정
        int tmp_titleWidth = tmp_stretch_width;

        QPen titlePen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);
        QPen authorPen(QColor::fromRgb(153,153,153), 1, Qt::SolidLine);

        // Font 설정
        QFont font_16;
        font_16.setPixelSize(16);

        // Data
        bugs::ArtistItemData data = ConvertData_forBugs::convertData_artistData(index.data(Qt::UserRole).toJsonObject());

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
            rect = option.rect.adjusted(5,5,THUMBNAIL_WIDTH,-5);
            thumbNail.paint(painter, rect, Qt::AlignVCenter);
        }


        int tmp_x1 = 5 + THUMBNAIL_WIDTH + SPACE;

        // 폰트 재설정
        painter->setFont(font_16);

        // 타이틀(아티스트 이름) & 서브타이틀(아티스트의 타입, 속성)
        rect = option.rect.adjusted(tmp_x1, 16, 0, -5);          // 16 : top 위치, -5 : bottom으로부터의 위치. (마이너스로 표기하면 됨)
        painter->setPen(titlePen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(data.artist_nm, tmp_titleWidth, font_16), &rect);

        QString tmp_text = data.group_cd_nm + "(" + data.gender_cd_nm + ")   " + data.list_genre_nm.join(",");
        rect = option.rect.adjusted(tmp_x1, 39, 0, -13);
        painter->setPen(authorPen);
        painter->drawText(rect.left(), rect.top(), tmp_titleWidth, rect.height(), Qt::AlignTop|Qt::AlignLeft, ellipseString(tmp_text, tmp_titleWidth, font_16), &rect);

        tmp_x1 += tmp_titleWidth + SPACE;       // next


    }

}
