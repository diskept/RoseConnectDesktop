#include "ProcStarData.h"
#include "networkhttp.h"
#include "ProcJsonEasy.h"
#include "common/global.h"

namespace common {

    // Get 관련
    const int HTTP_ROSE___GET_STAR___Music_Album = 8000;
    const int HTTP_ROSE___GET_STAR___Music_Playlist = 8001;
    const int HTTP_ROSE___GET_STAR___Music_Track = 8002;
    const int HTTP_ROSE___GET_STAR___Rosetube_Video = 8003;

    // Set 관련
    const int HTTP_ROSE___SET_STAR___Music_Album = 8100;
    const int HTTP_ROSE___SET_STAR___Music_Playlist = 8101;
    const int HTTP_ROSE___SET_STAR___Music_Track = 8102;
    const int HTTP_ROSE___SET_STAR___Rosetube_Video = 8103;



    /**
     * @brief ProcStarData::ProcStarData
     */
    ProcStarData::ProcStarData(QWidget *parent) : QObject(parent){
        // do nothing
    }


    ProcStarData::~ProcStarData(){
        // do nothing
    }


    /**
     * @brief 별점 정보를 요청함 (Get)
     * @param popupMenuMode - 요청하는 데이터의 모드값
     * @param data_pk 요청하고자 하는 데이터의 ID라고 보면 됨. OptMorePopup::PopupMenuMode 에 종속되어 있다.
     */
    void ProcStarData::request_getStarPoint(const OptMorePopup::PopupMenuMode popupMenuMode, const QString data_pk){
        // OptMorePopup::PopupMenuMode 에 따라서 처리 함수 호출함
        if(popupMenuMode == OptMorePopup::PopupMenuMode::Music_Album){
            this->proc_reqRose___music_album___get(data_pk);
        }
        else if(popupMenuMode == OptMorePopup::PopupMenuMode::Music_Playlist){
            this->proc_reqRose___music_playlist___get(data_pk);
        }
        else if(popupMenuMode == OptMorePopup::PopupMenuMode::Music_Track){
            this->proc_reqRose___music_track___get(data_pk);
        }
        else if(popupMenuMode == OptMorePopup::PopupMenuMode::Rosetube_Video){
            this->proc_reqRose___rosetube_video___get(data_pk);
        }
        else {
            emit this->signal_completed_getStarRate(0);
        }
    }


    /**
     * @brief 별점 정보 업데이트를 요청함 (Set)
     * @param popupMenuMode 요청하는 데이터의 모드값
     * @param data_pk
     * @param starRate
     */
    void ProcStarData::request_setStarPoint(const OptMorePopup::PopupMenuMode popupMenuMode, const QString data_pk, const int starRate){
        this->curr_starRate = starRate;
        // OptMorePopup::PopupMenuMode 에 따라서 처리 함수 호출함
        if(popupMenuMode == OptMorePopup::PopupMenuMode::Music_Album){
            this->proc_reqRose___music_playlist___set(data_pk, starRate);
        }
        else if(popupMenuMode == OptMorePopup::PopupMenuMode::Music_Playlist){
            this->proc_reqRose___music_playlist___set(data_pk, starRate);
        }
        else if(popupMenuMode == OptMorePopup::PopupMenuMode::Music_Track){
            this->proc_reqRose___music_track___set(data_pk, starRate);
        }
        else if(popupMenuMode == OptMorePopup::PopupMenuMode::Rosetube_Video){
            this->proc_reqRose___rosetube_video___set(data_pk, starRate);
        }
        else {
            emit this->signal_completed_setStarRate(starRate);
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : private functions [SLOT about HTTP]
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief OptionPopup::responseHttp : 로즈 장치 정보요청 결과 처리
     * @param p_jsonObject QJsonObject&
     */
    void ProcStarData::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        if(p_id == HTTP_ROSE___GET_STAR___Music_Album){
            int starRate = ProcJsonEasy::getInt(p_jsonObject, "starRate");
            emit this->signal_completed_getStarRate(starRate);
        }
        else if(p_id == HTTP_ROSE___SET_STAR___Music_Album){
            emit this->signal_completed_setStarRate(this->curr_starRate);
        }

        else if(p_id == HTTP_ROSE___GET_STAR___Music_Playlist){
            /// @todo 구현 필요 (음악 > Playlist) - get starRate
            int starRate = 5;   // 데이터 json 으로부터 세팅 필
            emit this->signal_completed_getStarRate(starRate);
        }
        else if(p_id == HTTP_ROSE___SET_STAR___Music_Playlist){
            /// @todo 구현 필요 (음악 > Playlist) - set starRate
            emit this->signal_completed_setStarRate(this->curr_starRate);
        }

        else if(p_id == HTTP_ROSE___GET_STAR___Music_Track){
            /// @todo 구현 필요 (음악 > Track) - get starRate
            int starRate = 5;   // 데이터 json 으로부터 세팅 필
            emit this->signal_completed_getStarRate(starRate);
        }
        else if(p_id == HTTP_ROSE___SET_STAR___Music_Track){
            /// @todo 구현 필요 (음악 > Playlist) - set starRate
            emit this->signal_completed_setStarRate(this->curr_starRate);
        }

        else if(p_id == HTTP_ROSE___GET_STAR___Rosetube_Video){
            /// @todo 구현 필요 (음악 > Track) - get starRate
            int starRate = 5;   // 데이터 json 으로부터 세팅 필
            emit this->signal_completed_getStarRate(starRate);
        }
        else if(p_id == HTTP_ROSE___SET_STAR___Rosetube_Video){
            /// @todo 구현 필요 (음악 > Playlist) - set starRate
            emit this->signal_completed_setStarRate(this->curr_starRate);
        }

        sender()->deleteLater();

    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : private functions - Rose HTTP API
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 음악 > Album 에 대해서 별점 Get 실제요청 처리
     * @param id_album
     */
    void ProcStarData::proc_reqRose___music_album___get(const QString& id_album){
        QJsonObject jsonObj_send;
        jsonObj_send.insert("id", id_album);
        jsonObj_send.insert("isAlbum", false);                      // 오디오냐 앨범이냐

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_ROSE___SET_STAR___Music_Album, QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port).arg(global.api_music_rate_get), jsonObj_send, true, true);
    }

    /**
     * @brief 음악 > Album 에 대해서 별점 Set 실제요청 처리
     * @param id - 앨범 아이디
     * @param starRate - 별점
     */
    void ProcStarData::proc_reqRose___music_album___set(const QString& id_album, const int starRate){
        QJsonObject jsonObj_send;
        jsonObj_send.insert("starRate", starRate);
        jsonObj_send.insert("id", id_album);
        jsonObj_send.insert("isAlbum", true);


        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_ROSE___GET_STAR___Music_Album, QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port).arg(global.api_music_rate_set), jsonObj_send, true, true);
    }



    /**
     * @brief 음악 > Playlust 에 대해서 별점 Get 실제요청 처리
     * @param id_playlist
     */
    void ProcStarData::proc_reqRose___music_playlist___get(const QString& id_playlist){
        Q_UNUSED(id_playlist);
    }

    /**
     * @brief 음악 > Playlust 에 대해서 별점 Set 실제요청 처리
     * @param id_playlist
     * @param starRate
     */
    void ProcStarData::proc_reqRose___music_playlist___set(const QString& id_playlist, const int starRate){
        Q_UNUSED(id_playlist);
        Q_UNUSED(starRate);
    }




    /**
     * @brief 음악 > Track 에 대해서 별점 Get 실제요청 처리
     * @param id_track
     */
    void ProcStarData::proc_reqRose___music_track___get(const QString& id_track){
        Q_UNUSED(id_track);
    }

    /**
     * @brief 음악 > Track 에 대해서 별점 Set 실제요청 처리
     * @param id_track
     * @param starRate
     */
    void ProcStarData::proc_reqRose___music_track___set(const QString& id_track, const int starRate){
        Q_UNUSED(id_track);
        Q_UNUSED(starRate);
    }




    /**
     * @brief Rosetube > Video 에 대해서 별점 Get 실제요청 처리
     * @param id_channelId
     */
    void ProcStarData::proc_reqRose___rosetube_video___get(const QString& id_channelId){
        Q_UNUSED(id_channelId);
    }

    /**
     * @brief Rosetube > Video 에 대해서 별점 Set 실제요청 처리
     * @param id_channelId
     * @param starRate
     */
    void ProcStarData::proc_reqRose___rosetube_video___set(const QString& id_channelId, const int starRate){
        Q_UNUSED(id_channelId);
        Q_UNUSED(starRate);
    }











}
