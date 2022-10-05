#ifndef ROSEHOMEADDPLAYLIST_H
#define ROSEHOMEADDPLAYLIST_H

#include "roseHome/AbstractRoseHomeSubWidget.h"
//#include "roseHome/.h"

#include "tidal/TidalAddPlaylist.h"
#include "qobuz/QobuzAddPlaylist.h"

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
        qobuz::QobuzAddPlaylist *addPlaylist_Qobuz;

        QJsonObject dataSend_jsonObj;

        QString streamType = "";

    };
};
#endif // ROSEHOMEADDPLAYLIST_H
