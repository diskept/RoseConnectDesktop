#include "roseHome/rosehomeEditPlaylist.h"

#include "common/gscommon.h"


namespace roseHome {

    /**
     * @brief
     * @param parent
     */
    RoseHomeEidtPlaylist::RoseHomeEidtPlaylist(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_filter, parent) {

    }


    /**
     * @brief 소멸자.
     */
    RoseHomeEidtPlaylist::~RoseHomeEidtPlaylist(){

        this->deleteLater();
    }


    /**
     * @brief RoseHomeEidtPlaylist::setJsonObject_forData
     * @param jsonObj
     */
    void RoseHomeEidtPlaylist::setJsonObject_forData(const QJsonObject &jsonObj){

        this->flagNeedReload = true;

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        QString viewType = ProcJsonEasy::getString(jsonObj, "view_type");

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);

        if(viewType == "create"){
            this->abstractAddPlaylist_RHV = new AbstractAddPlaylist_RHV(AbstractAddPlaylist_RHV::RosePlaylist, AbstractAddPlaylist_RHV::Creating);
        }
        else if(viewType == "add"){
            this->abstractAddPlaylist_RHV = new AbstractAddPlaylist_RHV(AbstractAddPlaylist_RHV::RosePlaylist, AbstractAddPlaylist_RHV::Adding);
        }
        else if(viewType == "edit"){
            this->abstractAddPlaylist_RHV = new AbstractAddPlaylist_RHV(AbstractAddPlaylist_RHV::RosePlaylist, AbstractAddPlaylist_RHV::Editing);
        }
        this->abstractAddPlaylist_RHV->setData_fromJsonObject(jsonObj);
        this->abstractAddPlaylist_RHV->setActivePage();

        this->setUIControl_basic();
    }


    void RoseHomeEidtPlaylist::setUIControl_basic(){

        this->box_contents->addWidget(this->abstractAddPlaylist_RHV);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        ContentLoadingwaitingMsgHide();
    }
}
