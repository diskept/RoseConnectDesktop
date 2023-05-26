#ifndef QOBUZADDPLAYLIST_H
#define QOBUZADDPLAYLIST_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "widget/AbstractAddPlaylist_RHV.h"


namespace qobuz {

    class QobuzAddPlaylist : public AbstractQobuzSubWidget
    {
        Q_OBJECT

    public:
        explicit QobuzAddPlaylist(QWidget *parent = nullptr);
        ~QobuzAddPlaylist();

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
#endif // QOBUZADDPLAYLIST_H
