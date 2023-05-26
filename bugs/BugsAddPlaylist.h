#ifndef BUGSADDPLAYLIST_H
#define BUGSADDPLAYLIST_H

#include "bugs/AbstractBugsSubWidget.h"

#include "widget/AbstractAddPlaylist_RHV.h"


namespace bugs {

    class BugsAddPlaylist : public AbstractBugsSubWidget
    {
        Q_OBJECT

    public:
        explicit BugsAddPlaylist(QWidget *parent = nullptr);
        ~BugsAddPlaylist();

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
#endif // BUGSADDPLAYLIST_H
