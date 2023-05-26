#ifndef ITEMWIDGET_VIDEO_H
#define ITEMWIDGET_VIDEO_H

#include "tidal/AbstractItem.h"

#include <QWidget>
#include <QJsonObject>


namespace video {

    /**
     * @brief Qobuz 전용으로 사용하는 Album 아이템 UI.
     */
    class ItemWidget_video : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemWidget_video(int index, int section, ImageSizeMode imageSizeMode, bool flagPlay, QWidget *parent = nullptr);

        void setData(const QJsonObject&);

        QJsonObject get_videoInfo();
        int get_fixedHeight() override;
        int get_fixedWidth();
        int get_rightMargin();

    protected slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

    private:
        // data
        QJsonObject data_video;
        QJsonObject video_Obj;

        // ui
        QLabel *label_title;

        QString img_path = "";

        bool flag_btnPlay = false;
    };
}
#endif // ITEMWIDGET_VIDEO_H
