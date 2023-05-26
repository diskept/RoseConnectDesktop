#ifndef ROSEHOMEEDITPLAYLIST_H
#define ROSEHOMEEDITPLAYLIST_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/AbstractAddPlaylist_RHV.h"


namespace roseHome {

    class RoseHomeEidtPlaylist : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit RoseHomeEidtPlaylist(QWidget *parent = nullptr);
        ~RoseHomeEidtPlaylist();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;

    private:
        void setUIControl_basic();

    private:
        AbstractAddPlaylist_RHV *abstractAddPlaylist_RHV;

        QString clientKey = "";
        int playlistID = 0;
    };
};
#endif // ROSEHOMEEDITPLAYLIST_H
