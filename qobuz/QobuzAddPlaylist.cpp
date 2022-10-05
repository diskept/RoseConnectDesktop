#include "qobuz/QobuzAddPlaylist.h"

#include "common/gscommon.h"


namespace qobuz {

    QobuzAddPlaylist::QobuzAddPlaylist(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_filter, parent)
    {

    }


    QobuzAddPlaylist::~QobuzAddPlaylist(){

        this->deleteLater();
    }


    void QobuzAddPlaylist::setJsonObject_forData(const QJsonObject& jsonObj){

        this->flagNeedReload = false;

        //ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        if(jsonObj.contains("playlistInfo") == false){

            this->flagNeedReload = true;

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->abstractAddPlaylist_RHV = new AbstractAddPlaylist_RHV(AbstractAddPlaylist_RHV::Qobuz, AbstractAddPlaylist_RHV::Creating);
            this->abstractAddPlaylist_RHV->setData_fromJsonObject(jsonObj);
            this->abstractAddPlaylist_RHV->setActivePage();

            this->setUIControl_basic();
        }
        else{
            QJsonObject playlistInfo = ProcJsonEasy::getJsonObject(jsonObj, "playlistInfo");

            QString tmpclientKey = ProcJsonEasy::getString(playlistInfo, "clientKey");
            int tmpId = ProcJsonEasy::getInt(playlistInfo, "id");

            if(!tmpclientKey.isEmpty() && (tmpclientKey != this->clientKey)){

                this->flagNeedReload = true;

                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->clientKey = tmpclientKey;
                this->playlistID = tmpId;

                this->abstractAddPlaylist_RHV = new AbstractAddPlaylist_RHV(AbstractAddPlaylist_RHV::Qobuz, AbstractAddPlaylist_RHV::Adding);
                this->abstractAddPlaylist_RHV->setData_fromJsonObject(jsonObj);
                this->abstractAddPlaylist_RHV->setActivePage();

                this->setUIControl_basic();
            }
            else if((tmpId > 0) && (tmpId != this->playlistID)){

                this->flagNeedReload = true;

                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->clientKey = tmpclientKey;
                this->playlistID = tmpId;

                this->abstractAddPlaylist_RHV = new AbstractAddPlaylist_RHV(AbstractAddPlaylist_RHV::Qobuz, AbstractAddPlaylist_RHV::Adding);
                this->abstractAddPlaylist_RHV->setData_fromJsonObject(jsonObj);
                this->abstractAddPlaylist_RHV->setActivePage();

                this->setUIControl_basic();
            }
        }
    }


    void QobuzAddPlaylist::setUIControl_basic(){

        this->box_contents->addWidget(this->abstractAddPlaylist_RHV);
        //ContentLoadingwaitingMsgHide();
    }
}
