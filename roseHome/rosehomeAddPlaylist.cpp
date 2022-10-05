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

        this->dataSend_jsonObj = QJsonObject();
        this->dataSend_jsonObj.insert("tracks", ProcJsonEasy::getJsonArray(jsonObj, "tracks"));
        this->dataSend_jsonObj.insert("view_type", ProcJsonEasy::getString(jsonObj, "view_type"));

        ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

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
            if(this->streamType == "TIDAL"){
                this->addPlaylist_Tidal->setJsonObject_forData(this->dataSend_jsonObj);
                this->addPlaylist_Tidal->setActivePage();
            }
            else if(this->streamType == "BUGS"){

            }
            else if(this->streamType == "QOBUZ"){
                this->addPlaylist_Qobuz->setJsonObject_forData(this->dataSend_jsonObj);
                this->addPlaylist_Qobuz->setActivePage();
            }
            else{

            }

            //if(abs_ani_dialog_wait->isHidden() != true){
                ContentLoadingwaitingMsgHide();
            //}
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
        if(this->streamType == "TIDAL"){
            this->addPlaylist_Tidal = new tidal::TidalAddPlaylist();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->addPlaylist_Tidal);
        }
        else if(this->streamType == "BUGS"){

        }
        else if(this->streamType == "QOBUZ"){

            this->addPlaylist_Qobuz = new qobuz::QobuzAddPlaylist();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->addPlaylist_Qobuz);
        }
        else{

        }
    }
}
