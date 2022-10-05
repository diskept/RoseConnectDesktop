#ifndef ITEMPLAYLIST_ROSETUBE_H
#define ITEMPLAYLIST_ROSETUBE_H

#include "tidal/AbstractItem.h"

#include <QWidget>
#include <QJsonObject>


namespace rosetube {

    /**
     * @brief Qobuz 전용으로 사용하는 Album 아이템 UI.
     */
    class ItemPlaylist_rosetube : public tidal::AbstractItem
    {
        Q_OBJECT

    public:
        explicit ItemPlaylist_rosetube(int index, int section, ImageSizeMode imageSizeMode, bool flagPlay, QWidget *parent = nullptr);
        ~ItemPlaylist_rosetube();

        void setData(const QJsonObject &jsonObj);
        int get_fixedHeight() override;

    private:
        void setUIControl();
        void paintEvent(QPaintEvent *event) override;

        // ui
        QLabel *label_title;
        QLabel *label_owner;
        QLabel *image_thumb;
        QLabel *label_thumb;

        QJsonObject data_playlist;

        bool flag_btnPlay = false;
    };

};

#endif // ITEMPLAYLIST_ROSETUBE_H
