#include "apple/ProcCommon_forApple.h"

#include "apple/ProcRosePlay_withApple.h"

#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/sqlitehelper.h"

namespace apple {

    /**
     * @brief 기본 생성자.
     */
    ProcCommon::ProcCommon(QWidget *parent) : QObject(parent) {
    }


    /**
     * @brief 소멸자. 메모리 해제 체크확인용
     */
    ProcCommon::~ProcCommon(){

        this->deleteLater();
    }


    void ProcCommon::request_apple_getListGenres(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.user_forApple.appleAPI_url + "/v1/catalog" + QString("/%1/genres").arg(global.user_forApple.get_store_front());
        QUrlQuery params;

        network->request_forApple(GetList_Genres
                         , Url
                         , NetworkHttp::HeaderOption_forApple::Request_UserToken
                         , params
                         , NetworkHttp::RequestMethod::Get);
    }


    void ProcCommon::request_apple_get_recommendations(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.music.apple.com/v1/me/recommendations
        QString Url = global.user_forApple.appleAPI_url + "/v1/me/recommendations";

        QUrlQuery params;

        network->request_forApple(GetRecommendations
                         , Url
                         , NetworkHttp::HeaderOption_forApple::Apple_UserToken
                         , params
                         , NetworkHttp::RequestMethod::Get);
    }


    void ProcCommon::request_apple_get_album(const int album_id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.user_forApple.appleAPI_url + "/v1/catalog" + QString("/%1/albums").arg(global.user_forApple.get_store_front()) + QString("/%1").arg(album_id);

        QUrlQuery params;
        params.addQueryItem("views", "appears-on");

        network->request_forApple(GetAlbumInfo
                         , Url
                         , NetworkHttp::HeaderOption_forApple::Request_UserToken
                         , params
                         , NetworkHttp::RequestMethod::Get);
    }


    void ProcCommon::request_apple_get_playlist(const QString playlist_id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.user_forApple.appleAPI_url + "/v1/catalog" + QString("/%1/playlists/").arg(global.user_forApple.get_store_front()) + playlist_id;

        QUrlQuery params;
        params.addQueryItem("views", "featured-artists");

        network->request_forApple(GetPlaylistinfo
                         , Url
                         , NetworkHttp::HeaderOption_forApple::Request_UserToken
                         , params
                         , NetworkHttp::RequestMethod::Get);
    }


    void ProcCommon::request_apple_get_chart(const QString types, const int genre, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString Url = global.user_forApple.appleAPI_url + "/v1/catalog" + QString("/%1/charts").arg(global.user_forApple.get_store_front());

        QUrlQuery params;
        params.addQueryItem("types", types);
        params.addQueryItem("genre", QString("%1").arg(genre));
        params.addQueryItem("limit", QString("%1").arg(limit));
        params.addQueryItem("offset", QString("%1").arg(offset));

        network->request_forApple(GetChartInfo
                         , Url
                         , NetworkHttp::HeaderOption_forApple::Request_UserToken
                         , params
                         , NetworkHttp::RequestMethod::Get);
    }


    // MARK : Http 요청 결과 처리 -------------------------------------------------------------------------------------------

    /**
     * @brief [슬롯] Http 요청 결과 처리. p_id에 따라 분기 처리 진행된다.
     * @details
     *      @li 로그인 처리 : @see setResult_loginTidal
     *      @li 로그아웃 처리 : @see setResult_logoutTidal
     * @param p_id 요청 아이디
     * @param jsonObject HTTP 응답 결과
     */
    void ProcCommon::slot_responseHttp(int p_id, QJsonObject jsonObj){

        //QJsonDocument doc(jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "jsonObj" << strJson;

        //NetworkHttp* sender_http = qobject_cast<NetworkHttp*>(sender());

        switch (p_id) {
            case HttpRequestType::Get_userToken:            break;

            case HttpRequestType::GetList_Genres:           this->setResult_listGenres(jsonObj);                break;

            case HttpRequestType::GetRecommendations:       this->setResult_recommendations(jsonObj);           break;

            case HttpRequestType::GetAlbumInfo:             this->setResult_album(jsonObj);                     break;
            case HttpRequestType::GetPlaylistinfo:          this->setResult_playlist(jsonObj);                  break;

            case HttpRequestType::GetChartInfo:             this->setResult_chart(jsonObj);                     break;
        }

        sender()->deleteLater();
    }


    void ProcCommon::setResult_listGenres(const QJsonObject &jsonObj){

        bool flagOk = false;
        QString errMsg = "";

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){

            flagOk = ProcJsonEasy::getBool(jsonObj, "flagOk");

            if(jsonObj.contains("data")){
                QJsonArray data = ProcJsonEasy::getJsonArray(jsonObj, "data");

                for(int i = 0; i < data.size(); i++){
                    QJsonObject genre = data.at(i).toObject();

                    QJsonObject attributes = ProcJsonEasy::getJsonObject(genre, "attributes");
                    QString name = ProcJsonEasy::getString(attributes, "name");
                    QString parentName = ProcJsonEasy::getString(attributes, "parentName");

                    int parentId = ProcJsonEasy::getInt(attributes, "parentId");

                    QString type = ProcJsonEasy::getString(genre, "type");
                    QString href = ProcJsonEasy::getString(genre, "href");

                    int id = ProcJsonEasy::getInt(genre, "id");

                    apple::GenreInfo tmpGenre;
                    tmpGenre.name = name;
                    tmpGenre.href = href;
                    tmpGenre.type = type;
                    tmpGenre.id = id;

                    tmpGenre.parentName = parentName;
                    tmpGenre.parentId = parentId;

                    list_genre.append(tmpGenre);
                }
            }
        }
        else{
            errMsg = "error";
        }

        emit signal_completeReq_getListGenre(flagOk, errMsg);
    }


    void ProcCommon::setResult_recommendations(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){

        }
    }


    void ProcCommon::setResult_album(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){

            QJsonObject albumObj;
            QJsonArray tracksArr;

            if(jsonObj.contains("data")){

                QJsonArray tmpData = ProcJsonEasy::getJsonArray(jsonObj, "data");

                if(tmpData.size() > 0){

                    QJsonObject tmpAlbum = tmpData.at(0).toObject();

                    // Album Info
                    if(tmpAlbum.contains("attributes")){

                        albumObj = ProcJsonEasy::getJsonObject(tmpAlbum, "attributes");
                        albumObj.insert("id", ProcJsonEasy::getString(tmpAlbum, "id"));

                        QJsonObject tmpObj = ProcJsonEasy::getJsonObject(tmpAlbum, "relationships");
                        albumObj.insert("artists", ProcJsonEasy::getJsonObject(tmpObj, "artists"));
                    }
                    emit this->completeReq_album(albumObj);

                    // Tracks Info
                    if(tmpAlbum.contains("relationships")){
                        QJsonObject tmpObj = ProcJsonEasy::getJsonObject(tmpAlbum, "relationships");
                        QJsonObject tracksObj = ProcJsonEasy::getJsonObject(tmpObj, "tracks");
                        tracksArr = ProcJsonEasy::getJsonArray(tracksObj, "data");
                    }
                    emit this->completeReq_tracks(tracksArr);

                    // Views Info
                    if(tmpAlbum.contains("views")){

                    }
                }
                else{
                    emit this->completeReq_album(albumObj);
                    emit this->completeReq_tracks(tracksArr);
                }
            }
        }
    }


    void ProcCommon::setResult_playlist(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){

            QJsonObject playlistObj;
            QJsonArray tracksArr;

            if(jsonObj.contains("data")){

                QJsonArray tmpData = ProcJsonEasy::getJsonArray(jsonObj, "data");

                if(tmpData.size() > 0){

                    QJsonObject tmpAlbum = tmpData.at(0).toObject();

                    // Album Info
                    if(tmpAlbum.contains("attributes")){

                        playlistObj = ProcJsonEasy::getJsonObject(tmpAlbum, "attributes");
                        playlistObj.insert("id", ProcJsonEasy::getString(tmpAlbum, "id"));

                        QJsonObject tmpObj = ProcJsonEasy::getJsonObject(tmpAlbum, "relationships");
                        playlistObj.insert("curator", ProcJsonEasy::getJsonObject(tmpObj, "curator"));
                    }
                    emit this->completeReq_playlist(playlistObj);

                    // Tracks Info
                    if(tmpAlbum.contains("relationships")){
                        QJsonObject tmpObj = ProcJsonEasy::getJsonObject(tmpAlbum, "relationships");
                        QJsonObject tracksObj = ProcJsonEasy::getJsonObject(tmpObj, "tracks");
                        tracksArr = ProcJsonEasy::getJsonArray(tracksObj, "data");
                    }
                    emit this->completeReq_tracks(tracksArr);

                    // Views Info
                    if(tmpAlbum.contains("views")){

                    }
                }
                else{
                    emit this->completeReq_playlist(playlistObj);
                    emit this->completeReq_tracks(tracksArr);
                }
            }
        }
    }


    void ProcCommon::setResult_chart(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk") && jsonObj.value("flagOk").toBool()){

            /*QJsonObject playlistObj;
            QJsonArray tracksArr;

            if(jsonObj.contains("data")){

                QJsonArray tmpData = ProcJsonEasy::getJsonArray(jsonObj, "data");

                if(tmpData.size() > 0){

                }
            }*/
        }
    }


    /**
     * @brief @see list_genre 의 length를 반환
     * @return
     */
    int ProcCommon::getCount_listGenreInfo(){

        return list_genre.length();
    }


    /**
     * @brief @see list_genre의 idx 인덱스에 해당되는 GenreInfo를 반환한다.
     * @param idx list_genre에 접근할 인덱스
     * @return idx가 유효하지 않으면, 비어있는 GenreInfo 반환.
     */
    apple::GenreInfo ProcCommon::getGenreInfo(int idx){

        if(idx < 0){
            return apple::GenreInfo{};
        }
        else if(idx >= list_genre.length()){
            return apple::GenreInfo{};
        }
        else {
            return list_genre.at(idx);
        }
    }
}
