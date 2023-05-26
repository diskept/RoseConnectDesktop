#include "rosehomePlaylistDetail.h"
#include "ConvertData_rosehome.h"

#include "common/gscommon.h"

namespace roseHome {

    /**
     * @brief
     * @param parent
     */
    RoseHomePlaylistDetail::RoseHomePlaylistDetail(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::NoneScroll_filter, parent) {

    }

    /**
     * @brief 소멸자.
     */
    RoseHomePlaylistDetail::~RoseHomePlaylistDetail(){

        this->deleteLater();
    }


    /**
     * @brief QobuzAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void RoseHomePlaylistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PlaylistItemData tmp_data_playlist = ConvertData::convertData_playlistData(jsonObj);

        this->flagNeedReload = false;

        if(tmp_data_playlist.id != this->data_playlist.id){

            this->flagNeedReload = true;
            this->data_playlist = tmp_data_playlist;

            this->playlist_Obj = QJsonObject();
            if(tmp_data_playlist.isRose == true){

                this->playlist_Obj = jsonObj;
            }
            else if(tmp_data_playlist.isRose == false){

                if(this->data_playlist.type == "TIDAL"){
                    this->playlist_Obj.insert("uuid", data_playlist.clientKey);
                    this->tidal_mix_chk = data_playlist.title;
                    if(this->tidal_mix_chk.contains("Mix")){
                        this->playlist_Obj.insert("image", data_playlist.thumbnail);
                        this->playlist_Obj.insert("title", data_playlist.title);
                        this->playlist_Obj.insert("creatorName", data_playlist.ownerName);
                        this->playlist_Obj.insert("description", data_playlist.comment);
                        this->playlist_Obj.insert("numberOfTracks", data_playlist.trackCount);
                    }
                }
                else if(this->data_playlist.type == "BUGS"){
                    this->playlist_Obj.insert("es_album_id", data_playlist.clientKey.toInt());
                }
                else if(this->data_playlist.type == "QOBUZ"){
                    this->playlist_Obj.insert("id", data_playlist.clientKey);
                }
                else if(this->data_playlist.type == "APPLE_MUSIC"){
                    this->playlist_Obj.insert("id", data_playlist.clientKey);
                }
            }

            this->setUIControl_basic();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }

    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseHomePlaylistDetail::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractRoseHomeSubWidget::setActivePage();

        if(this->flagNeedReload){

            if(this->data_playlist.isRose == true){

                this->playlistDetail_Rose->setJsonObject_forData(this->playlist_Obj);
                this->playlistDetail_Rose->setActivePage();

                this->box_contents->addWidget(this->playlistDetail_Rose);
            }
            else if(this->data_playlist.isRose == false){

                if(this->data_playlist.type == "TIDAL"){
                    if(this->tidal_mix_chk.contains("Mix")){
                        this->mixlistDetail_Tidal->setJsonObject_forData(this->playlist_Obj);
                        this->mixlistDetail_Tidal->setActivePage();

                        this->box_contents->addWidget(this->mixlistDetail_Tidal);
                    }
                    else{
                        this->playlistDetail_Tidal->setJsonObject_forData(this->playlist_Obj);
                        this->playlistDetail_Tidal->setActivePage();

                        this->box_contents->addWidget(this->playlistDetail_Tidal);
                    }

                    global.user_forTidal.flag_rosehome = true;
                }
                else if(this->data_playlist.type == "BUGS"){
                    this->pdDlbumDetail_Bugs->setJsonObject_forData(this->playlist_Obj);
                    this->pdDlbumDetail_Bugs->setActivePage();

                    this->box_contents->addWidget(this->pdDlbumDetail_Bugs);

                    global.user_forBugs.flag_rosehome = true;
                }
                else if(this->data_playlist.type == "QOBUZ"){
                    this->playlistDetail_Qobuz->setJsonObject_forData(this->playlist_Obj);
                    this->playlistDetail_Qobuz->setActivePage();

                    this->box_contents->addWidget(this->playlistDetail_Qobuz);

                    global.user_forQobuz.flag_rosehome = true;
                }
                else if(this->data_playlist.type == "APPLE_MUSIC"){
                    this->playlistDetail_Apple->setJsonObject_forData(this->playlist_Obj);
                    this->playlistDetail_Apple->setActivePage();

                    this->box_contents->addWidget(this->playlistDetail_Apple);
                }
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
    void RoseHomePlaylistDetail::setUIControl_basic(){

        if(this->data_playlist.isRose == true){

            this->playlistDetail_Rose = new RoseHomePlaylistDetail_Rose();
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
        }
        else if(this->data_playlist.isRose == false){
            if(this->data_playlist.type == "TIDAL"){
                if(this->tidal_mix_chk.contains("Mix")){

                    this->mixlistDetail_Tidal = new tidal::TidalMixesDetail();
                    GSCommon::clearLayout(this->box_contents);
                    this->box_contents->setAlignment(Qt::AlignTop);
                }
                else{

                    this->playlistDetail_Tidal = new tidal::TidalPlaylistDetail();
                    GSCommon::clearLayout(this->box_contents);
                    this->box_contents->setAlignment(Qt::AlignTop);
                }
            }
            else if(this->data_playlist.type == "BUGS"){

                this->pdDlbumDetail_Bugs = new bugs::BugsPDAlbumDetail();
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);
            }
            else if(this->data_playlist.type == "QOBUZ"){

                this->playlistDetail_Qobuz = new qobuz::QobuzPlaylistDetail();
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);
            }
            else if(this->data_playlist.type == "APPLE_MUSIC"){

                this->playlistDetail_Apple = new apple::ApplePlaylistDetail();
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);
            }
        }
    }
}
