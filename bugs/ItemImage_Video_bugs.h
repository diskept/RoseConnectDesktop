#ifndef ITEMIMAGE_VIDEO_BUGS_H
#define ITEMIMAGE_VIDEO_BUGS_H

#include <QWidget>
#include "tidal/AbstractItemImage.h"

namespace bugs {
    /**
     * @brief @sa ItemVideo_bugs 에서 사용하는 벅스 Video용 Image파트 클래스
     */
    class ItemImage_Video_bugs : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Video_bugs(int imageWidth, int imageHeight, QWidget *parent = nullptr);
        void setDuration(const QString len);

    protected:
        QString getImagePath_default() override;

        void setUIControl_duration();       ///< 재생시간 UI 세팅

        QLabel *label_durationText;             ///< 재생시간
        QLabel *label_durationBG;           ///< 재생시간 영역의 배경색 (Opacity 0.6 적용)


    };
};

#endif // ITEMIMAGE_VIDEO_BUGS_H
