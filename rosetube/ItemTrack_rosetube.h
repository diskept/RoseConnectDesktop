#ifndef ITEMTRACK_ROSETUBE_H
#define ITEMTRACK_ROSETUBE_H

#include "tidal/AbstractItem.h"

#include <QWidget>
#include <QJsonObject>


namespace rosetube {

    /**
     * @brief Qobuz 전용으로 사용하는 Album 아이템 UI.
     */
    class ItemTrack_rosetube : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemTrack_rosetube(int index, int section, ImageSizeMode imageSizeMode, bool flagPlay, QWidget *parent = nullptr);
        ~ItemTrack_rosetube();

        void setData(const QJsonObject &jsonObj);
        void setCacheImgShow(bool flag);

        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // ui
        QLabel *label_title;
        QLabel *label_artist;
        QLabel *label_number;

        QJsonObject data_track;

        bool flag_btnPlay = false;
    };

};

#endif // ITEMTRACK_ROSETUBE_H
