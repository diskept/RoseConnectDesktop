#include "rosehome/rosehomeAlbumDetail.h"
#include "rosehome/ConvertData_rosehome.h"

#include "common/gscommon.h"

namespace roseHome {

    /**
     * @brief
     * @param parent
     */
    RoseHomeAlbumDetail::RoseHomeAlbumDetail(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::NoneScroll_filter, parent) {

        //this->setUIControl_basic();
    }

    /**
     * @brief 소멸자.
     */
    RoseHomeAlbumDetail::~RoseHomeAlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief QobuzAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void RoseHomeAlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::AlbumItemData tmp_data_album = ConvertData::convertData_albumData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data_album.id != this->data_album.id){
            this->flagNeedReload = true;
            this->data_album = tmp_data_album;

            this->album_Obj = QJsonObject();
            if(this->data_album.type == "MUSIC"){
                this->album_Obj = jsonObj;
            }
            else if(this->data_album.type == "TIDAL"){
                this->album_Obj.insert("id", data_album.clientKey.toInt());
            }
            else if(this->data_album.type == "BUGS"){
                this->album_Obj.insert("album_id", data_album.clientKey.toInt());
            }
            else if(this->data_album.type == "QOBUZ"){
                this->album_Obj.insert("id", data_album.clientKey);
            }
            else if(this->data_album.type == "APPLE_MUSIC"){
                this->album_Obj.insert("id", data_album.clientKey.toInt());
            }

            this->setUIControl_basic();
        }
    }

    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseHomeAlbumDetail::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractRoseHomeSubWidget::setActivePage();

        if(this->flagNeedReload){
            if(this->data_album.type == "MUSIC"){
                this->albumDetail_Rose->setJsonObject_forData(this->album_Obj);
                this->albumDetail_Rose->setActivePage();
            }
            else if(this->data_album.type == "TIDAL"){
                this->albumDetail_Tidal->setJsonObject_forData(this->album_Obj);
                this->albumDetail_Tidal->setActivePage();
                global.user_forTidal.flag_rosehome = true;
            }
            else if(this->data_album.type == "BUGS"){
                this->albumDetail_Bugs->setJsonObject_forData(this->album_Obj);
                this->albumDetail_Bugs->setActivePage();
                global.user_forBugs.flag_rosehome = true;
            }
            else if(this->data_album.type == "QOBUZ"){
                this->albumDetail_Qobuz->setJsonObject_forData(this->album_Obj);
                this->albumDetail_Qobuz->setActivePage();
                global.user_forQobuz.flag_rosehome = true;
            }
            else if(this->data_album.type == "APPLE_MUSIC"){
                this->albumDetail_Apple->setJsonObject_forData(this->album_Obj);
                this->albumDetail_Apple->setActivePage();
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
    void RoseHomeAlbumDetail::setUIControl_basic(){

        // 앨범 상세정보
        if(this->data_album.type == "MUSIC"){
            this->albumDetail_Rose = new roseHome::RoseHomeAlbumDetail_Rose();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->albumDetail_Rose);
        }
        else if(this->data_album.type == "TIDAL"){
            this->albumDetail_Tidal = new tidal::TidalAlbumDetail();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->albumDetail_Tidal);
        }
        if(this->data_album.type == "BUGS"){
            this->albumDetail_Bugs = new bugs::BugsAlbumDetail();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->albumDetail_Bugs);
        }
        else if(this->data_album.type == "QOBUZ"){
            this->albumDetail_Qobuz = new qobuz::QobuzAlbumDetail();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->albumDetail_Qobuz);
        }
        else if(this->data_album.type == "APPLE_MUSIC"){
            this->albumDetail_Apple = new apple::AppleAlbumDetail();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->albumDetail_Apple);
        }
    }
}
