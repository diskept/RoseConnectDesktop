#ifndef ROSETUBE_ADDPLAYLIST_H
#define ROSETUBE_ADDPLAYLIST_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "widget/AbstractAddPlaylist_RHV.h"

namespace rosetube {

    class RoseTubeAddPlaylist : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit RoseTubeAddPlaylist(QWidget *parent = nullptr);
        ~RoseTubeAddPlaylist();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;

    private:
        void setUIControl_basic();

    private:
        AbstractAddPlaylist_RHV *abstractAddPlaylist_RHV;

        QString streamType = "";
    };
};
#endif // ROSETUBE_ADDPLAYLIST_H
