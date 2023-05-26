#include "rosetube/rosetube_ADDPlaylist.h"

#include "common/gscommon.h"


namespace rosetube {

    RoseTubeAddPlaylist::RoseTubeAddPlaylist(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_filter, parent) {

    }


    RoseTubeAddPlaylist::~RoseTubeAddPlaylist(){

        this->deleteLater();
    }


    void RoseTubeAddPlaylist::setJsonObject_forData(const QJsonObject& jsonObj){

        this->flagNeedReload = false;

        QJsonObject tmpData = ProcJsonEasy::getJsonObject(jsonObj, "data");
        this->streamType = ProcJsonEasy::getString(tmpData, "type");

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        if(this->streamType == "ROSE"){
            this->flagNeedReload = true;

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->abstractAddPlaylist_RHV = new AbstractAddPlaylist_RHV(AbstractAddPlaylist_RHV::RosePlaylist, AbstractAddPlaylist_RHV::Editing);
            this->abstractAddPlaylist_RHV->setData_fromJsonObject(tmpData);
            this->abstractAddPlaylist_RHV->setActivePage();

            this->setUIControl_basic();
        }
        else{
            if(jsonObj.contains("playlistInfo") == false){
                this->flagNeedReload = true;

                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->abstractAddPlaylist_RHV = new AbstractAddPlaylist_RHV(AbstractAddPlaylist_RHV::RoseTube, AbstractAddPlaylist_RHV::Creating);
                this->abstractAddPlaylist_RHV->setData_fromJsonObject(jsonObj);
                this->abstractAddPlaylist_RHV->setActivePage();

                this->setUIControl_basic();
            }
            else{
                this->flagNeedReload = true;

                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->abstractAddPlaylist_RHV = new AbstractAddPlaylist_RHV(AbstractAddPlaylist_RHV::RoseTube, AbstractAddPlaylist_RHV::Adding);
                this->abstractAddPlaylist_RHV->setData_fromJsonObject(jsonObj);
                this->abstractAddPlaylist_RHV->setActivePage();

                this->setUIControl_basic();
            }
        }
    }


    void RoseTubeAddPlaylist::setUIControl_basic(){

        this->box_contents->addWidget(this->abstractAddPlaylist_RHV);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        ContentLoadingwaitingMsgHide();
    }
}
