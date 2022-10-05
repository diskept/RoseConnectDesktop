#ifndef ITEMIMAGE_VIDEO_FORQOBUZ_H
#define ITEMIMAGE_VIDEO_FORQOBUZ_H

//#include "AbstractItemImage_forQobuz.h"
#include <QWidget>
#include "tidal/AbstractItemImage.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {
    /**
     * @brief @sa ItemVideo 에서 사용하는 Qobuz Video용 Image파트 클래스
     */
    class ItemImage_Video_qobuz : public tidal::AbstractItemImage
    {
        Q_OBJECT
    public:
        explicit ItemImage_Video_qobuz(int imageWidth, int imageHeight, QWidget *parent = nullptr);
        void setDuration(const int duration);

    protected:
        QString getImagePath_default() override;

        void setUIControl_duration();       ///< 재생시간 UI 세팅

        QLabel *label_durationText;             ///< 재생시간
        QLabel *label_durationBG;           ///< 재생시간 영역의 배경색 (Opacity 0.6 적용)


    };
};

#endif // ITEMIMAGE_VIDEO_H
