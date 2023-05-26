#include "rosehome/rosehomeArtistDetail.h"
#include "rosehome/ConvertData_rosehome.h"

#include "common/gscommon.h"

namespace roseHome {

    /**
     * @brief
     * @param parent
     */
    RoseHomeArtistDetail::RoseHomeArtistDetail(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::NoneScroll_filter, parent) {

        //this->setUIControl_basic();
    }

    /**
     * @brief 소멸자.
     */
    RoseHomeArtistDetail::~RoseHomeArtistDetail(){

        this->deleteLater();
    }


    /**
     * @brief QobuzAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void RoseHomeArtistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::ArtistItemData tmp_data_artist = ConvertData::convertData_artistData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data_artist.id != this->data_artist.id){
            this->flagNeedReload = true;
            this->data_artist = tmp_data_artist;

            this->artist_Obj = QJsonObject();
            if(this->data_artist.type == "MUSIC"){
                this->artist_Obj = jsonObj;
            }
            else if(this->data_artist.type == "TIDAL"){
                if(data_artist.thumbnail.isEmpty()){
                     this->artist_Obj.insert("id", data_artist.clientKey.toInt());
                }
                else{
                    this->artist_Obj.insert("id", data_artist.clientKey.toInt());
                    this->artist_Obj.insert("name", data_artist.name);
                    this->artist_Obj.insert("image", data_artist.thumbnail);
                }
            }
            else if(this->data_artist.type == "BUGS"){
                if(data_artist.thumbnail.isEmpty()){
                     this->artist_Obj.insert("artist_id", data_artist.clientKey.toInt());
                }
                else{
                    this->artist_Obj.insert("artist_id", data_artist.clientKey.toInt());
                    this->artist_Obj.insert("artist_nm", data_artist.name);
                    this->artist_Obj.insert("image", data_artist.thumbnail);
                }
            }
            else if(this->data_artist.type == "QOBUZ"){
                this->artist_Obj.insert("id", data_artist.clientKey.toInt());
            }
            else if(this->data_artist.type == "APPLE_MUSIC"){
                this->artist_Obj.insert("id", data_artist.clientKey.toInt());
            }

            this->setUIControl_basic();
        }
    }

    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseHomeArtistDetail::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractRoseHomeSubWidget::setActivePage();

        if(this->flagNeedReload){
            /*if(this->data_artist.type == "MUSIC"){
                this->artistDetail_Rose->setJsonObject_forData(this->artist_Obj);
                this->artistDetail_Rose->setActivePage();
            }
            else*/ if(this->data_artist.type == "TIDAL"){
                this->artistDetail_Tidal->setJsonObject_forData(this->artist_Obj);
                this->artistDetail_Tidal->setActivePage();
                global.user_forTidal.flag_rosehome = true;
            }
            else if(this->data_artist.type == "BUGS"){
                this->artistDetail_Bugs->setJsonObject_forData(this->artist_Obj);
                this->artistDetail_Bugs->setActivePage();
                global.user_forBugs.flag_rosehome = true;
            }
            else if(this->data_artist.type == "QOBUZ"){
                this->artistDetail_Qobuz->setJsonObject_forData(this->artist_Obj);
                this->artistDetail_Qobuz->setActivePage();
                global.user_forQobuz.flag_rosehome = true;
            }
            /*else if(this->data_artist.type == "APPLE_MUSIC"){
                this->artistDetail_Apple->setJsonObject_forData(this->artist_Obj);
                this->artistDetail_Apple->setActivePage();
            }*/
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
    void RoseHomeArtistDetail::setUIControl_basic(){

        // 앨범 상세정보
        /*if(this->data_artist.type == "MUSIC"){
            this->artistDetail_Rose = new roseHome::RoseHomeArtistDetail_Rose();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->artistDetail_Rose);
        }
        else*/ if(this->data_artist.type == "TIDAL"){
            this->artistDetail_Tidal = new tidal::TidalArtistDetail();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->artistDetail_Tidal);
        }
        if(this->data_artist.type == "BUGS"){
            this->artistDetail_Bugs = new bugs::BugsArtistDetail();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->artistDetail_Bugs);
        }
        else if(this->data_artist.type == "QOBUZ"){
            this->artistDetail_Qobuz = new qobuz::QobuzArtistDetail();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->artistDetail_Qobuz);
        }
        /*else if(this->data_artist.type == "APPLE_MUSIC"){
            this->artistDetail_Apple = new apple::AppleAlbumDetail();

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->box_contents->addWidget(this->artistDetail_Apple);
        }*/
    }
}
