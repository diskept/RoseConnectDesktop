#ifndef ROSEHOMEADDPLAYLIST_H
#define ROSEHOMEADDPLAYLIST_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "tidal/TidalAddPlaylist.h"
#include "bugs/BugsAddPlaylist.h"
#include "qobuz/QobuzAddPlaylist.h"
#include "roseHome/rosehomeEditPlaylist.h"
#include "rosetube/rosetube_ADDPlaylist.h"

#include <QListWidget>


namespace roseHome {

    /**
     * @brief RoseHomeAddPlaylist
     */
    class RoseHomeAddPlaylist : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomeAddPlaylist(QWidget *parent = nullptr);
        ~RoseHomeAddPlaylist();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    private:
        // function about UI
        void setUIControl_basic();

    private:

        tidal::TidalAddPlaylist *addPlaylist_Tidal;
        bugs::BugsAddPlaylist *addPlaylist_Bugs;
        qobuz::QobuzAddPlaylist *addPlaylist_Qobuz;
        roseHome::RoseHomeEidtPlaylist *editPlaylist_Rose;
        rosetube::RoseTubeAddPlaylist *addPlaylist_Rosetube;

        QJsonObject dataSend_jsonObj;

        QString streamType = "";
        QString viewType = "";
    };
};
#endif // ROSEHOMEADDPLAYLIST_H
