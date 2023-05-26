#include "roseHome/rosehomeAddPlaylist.h"

#include "common/gscommon.h"


namespace roseHome {

    /**
     * @brief
     * @param parent
     */
    RoseHomeAddPlaylist::RoseHomeAddPlaylist(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::NoneScroll_filter, parent) {

        //this->setUIControl_basic();
    }


    /**
     * @brief 소멸자.
     */
    RoseHomeAddPlaylist::~RoseHomeAddPlaylist(){

        this->deleteLater();
    }


    /**
     * @brief QobuzAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void RoseHomeAddPlaylist::setJsonObject_forData(const QJsonObject &jsonObj){

        this->flagNeedReload = true;

        this->streamType = ProcJsonEasy::getString(jsonObj, "type");
        this->viewType = ProcJsonEasy::getString(jsonObj, "view_type");

        this->dataSend_jsonObj = QJsonObject();
        this->dataSend_jsonObj.insert("tracks", ProcJsonEasy::getJsonArray(jsonObj, "tracks"));
        this->dataSend_jsonObj.insert("view_type", this->viewType);

        if(jsonObj.contains("playlistInfo")){
             this->dataSend_jsonObj.insert("playlistInfo", ProcJsonEasy::getJsonObject(jsonObj, "playlistInfo"));
        }

        if(jsonObj.contains("playlist_id")){
            this->dataSend_jsonObj.insert("playlist_id", ProcJsonEasy::getInt(jsonObj, "playlist_id"));
        }

        this->setUIControl_basic();
    }


    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseHomeAddPlaylist::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractRoseHomeSubWidget::setActivePage();

        if(this->flagNeedReload){
            if(this->streamType == "ROSETUBE"){
                this->addPlaylist_Rosetube->setJsonObject_forData(this->dataSend_jsonObj);
                this->addPlaylist_Rosetube->setActivePage();
            }
            else if(this->streamType == "TIDAL"){
                this->addPlaylist_Tidal->setJsonObject_forData(this->dataSend_jsonObj);
                this->addPlaylist_Tidal->setActivePage();
            }
            else if(this->streamType == "BUGS"){
                this->addPlaylist_Bugs->setJsonObject_forData(this->dataSend_jsonObj);
                this->addPlaylist_Bugs->setActivePage();
            }
            else if(this->streamType == "QOBUZ"){
                this->addPlaylist_Qobuz->setJsonObject_forData(this->dataSend_jsonObj);
                this->addPlaylist_Qobuz->setActivePage();
            }
            else if(this->streamType == "ROSE"){
                this->editPlaylist_Rose->setJsonObject_forData(this->dataSend_jsonObj);
                this->editPlaylist_Rose->setActivePage();
            }
            else{

            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 화면 UI 세팅
     */
    void RoseHomeAddPlaylist::setUIControl_basic(){

        // playlist add page
        if(this->streamType == "ROSETUBE"){
            this->addPlaylist_Rosetube = new rosetube::RoseTubeAddPlaylist();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->addPlaylist_Rosetube);
        }
        else if(this->streamType == "TIDAL"){
            this->addPlaylist_Tidal = new tidal::TidalAddPlaylist();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->addPlaylist_Tidal);
        }
        else if(this->streamType == "BUGS"){
            this->addPlaylist_Bugs = new bugs::BugsAddPlaylist();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->addPlaylist_Bugs);
        }
        else if(this->streamType == "QOBUZ"){

            this->addPlaylist_Qobuz = new qobuz::QobuzAddPlaylist();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->addPlaylist_Qobuz);
        }
        else if(this->streamType == "ROSE"){

            this->editPlaylist_Rose = new roseHome::RoseHomeEidtPlaylist();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->editPlaylist_Rose);
        }
        else{

        }
    }
}
