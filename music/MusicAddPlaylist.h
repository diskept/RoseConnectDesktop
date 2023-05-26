#ifndef MUSICADDPLAYLIST_H
#define MUSICADDPLAYLIST_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/AbstractAddPlaylist_RHV.h"


namespace music {

    class MusicAddPlaylist : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit MusicAddPlaylist(QWidget *parent = nullptr);
        ~MusicAddPlaylist();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;

    private:
        void setUIControl_basic();

    private:
        AbstractAddPlaylist_RHV *abstractAddPlaylist_RHV;

        QString streamType = "";
        QString clientKey = "";
        int playlistID = 0;

    };
};
#endif // MUSICADDPLAYLIST_H
