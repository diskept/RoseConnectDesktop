#include "apple/AppleArtistDetail.h"


namespace apple {


    /**
     * @brief
     * @param parent
     */
    AppleArtistDetail::AppleArtistDetail(QWidget *parent) : AbstractAppleSubWidget(MainUIType::VerticalScroll_filter, parent) {

        /*this->linker = Linker::getInstance();

        // data
        this->list_track = new QList<qobuz::TrackItemData>();
        this->list_album = new QList<qobuz::AlbumItemData>();
        this->list_release = new QList<qobuz::AlbumItemData>();
        this->list_appears = new QList<qobuz::TrackItemData>();
        this->list_artist = new QList<qobuz::ArtistItemData>();*/
    }


    /**
     * @brief 소멸자.
     */
    AppleArtistDetail::~AppleArtistDetail(){

        this->deleteLater();
    }


    /**
     * @brief AppleArtistDetail::setJsonObject_forData
     * @param jsonObj
     */
    void AppleArtistDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        /*qobuz::ArtistItemData tmp_data_artist = ConvertData::convertData_artistItemData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data_artist.id != this->data_artist.id){

            this->flagNeedReload = true;

            if(this->flag_page_load == true){
                this->widget_main_contents->hide();
                this->box_contents->removeWidget(this->widget_main_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->flag_page_load = false;
            }

            // clear Data
            this->list_track->clear();
            this->list_release->clear();
            this->list_album->clear();
            this->list_appears->clear();
            this->list_artist->clear();

            this->flag_tracks[0] = false;
            this->flag_lastRelease[0] = false;
            this->flag_release[0] = false;
            this->flag_appears[0] = false;
            this->flag_suggestArtists[0] = false;

            this->flag_tracks[1] = false;
            this->flag_lastRelease[1] = false;
            this->flag_release[1] = false;
            this->flag_appears[1] = false;
            this->flag_suggestArtists[1] = false;

            this->flag_topTracks = false;
            this->flag_appearsOn = false;

            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            this->track_id_fav = 0;
            this->track_idx_fav = 0;
            this->track_star_fav = 0;

            this->data_artist = tmp_data_artist;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            if(this->data_artist.biography.isEmpty()){
                // Artist 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                ProcCommon *proc = new ProcCommon(this);
                connect(proc, &ProcCommon::completeReq_artist, this, &AppleArtistDetail::slot_applyResult_artistInfo);
                proc->request_qobuz_get_artist(this->data_artist.id);
            }

            // request HTTP API
            ProcCommon *proc_release = new ProcCommon(this);
            connect(proc_release, &ProcCommon::completeReq_list_albums, this, &AppleArtistDetail::slot_applyResult_albums);
            connect(proc_release, &ProcCommon::completeReq_list_lastrelease, this, &AppleArtistDetail::slot_applyResult_lastrelease);
            connect(proc_release, &ProcCommon::completeReq_list_tracks, this, &AppleArtistDetail::slot_applyResult_tracks);
            proc_release->request_qobuz_getList_lastrelease("artist/get", "playlists,albums_with_last_release", data_artist.id, 15, 0);

            ProcCommon *proc_appears = new ProcCommon(this);
            connect(proc_appears, &ProcCommon::completeReq_list_appearson, this, &AppleArtistDetail::slot_applyResult_appearson);
            proc_appears->request_qobuz_getList_appearson("artist/get", "tracks_appears_on", data_artist.id, 100, 0);

            ProcCommon *proc_artist = new ProcCommon(this);
            connect(proc_artist, &ProcCommon::completeReq_list_artists, this, &AppleArtistDetail::slot_applyResult_artists);
            proc_artist->request_qobuz_getList_artists("artist/getSimilarArtists", data_artist.id, 15, 0);
        }
        else{
            this->flag_reload_page = true;

            this->flag_send_artist = false;
            ProcCommon *proc_fav_artist = new ProcCommon(this);
            connect(proc_fav_artist, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &AppleArtistDetail::slot_qobuz_completeReq_listAll_myFavoritesIds);
            proc_fav_artist->request_qobuz_get_favorite("artist", QString("%1").arg(this->data_artist.id));

            if(this->list_track->size() > 0){

                int max_cnt = 0;
                if(this->list_track->size() > 5){
                    max_cnt = 5;
                }
                else{
                    max_cnt = this->list_track->size();
                }

                QList<QString> tmp_clientkey;
                for(int i = 0; i < max_cnt; i++){
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
                }

                // request HTTP API - get favorite for Rose Server
                this->flag_topTracks = true;
                roseHome::ProcCommon *proc_fav_top = new roseHome::ProcCommon(this);
                connect(proc_fav_top, &roseHome::ProcCommon::completeReq_rating_track, this, &AppleArtistDetail::slot_applyResult_getRating_track);
                proc_fav_top->request_rose_getRating_Track("QOBUZ", tmp_clientkey);
            }
        }*/
    }

    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void AppleArtistDetail::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractAppleSubWidget::setActivePage();

            /*this->box_contents->removeWidget(this->widget_main_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->setUIControl_basic();*/
        }
    }
}
