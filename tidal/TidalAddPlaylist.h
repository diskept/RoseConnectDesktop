#ifndef TIDALADDPLAYLIST_H
#define TIDALADDPLAYLIST_H

#include "tidal/AbstractTidalSubWidget.h"

#include "widget/AbstractAddPlaylist_RHV.h"


namespace tidal {

    class TidalAddPlaylist : public AbstractTidalSubWidget
    {
        Q_OBJECT

    public:
        explicit TidalAddPlaylist(QWidget *parent = nullptr);
        ~TidalAddPlaylist();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;

    private:
        void setUIControl_basic();

    private:
        AbstractAddPlaylist_RHV *abstractAddPlaylist_RHV;

        QString clientKey = "";
        int playlistID = 0;
    };
};

#endif // TIDALADDPLAYLIST_H
