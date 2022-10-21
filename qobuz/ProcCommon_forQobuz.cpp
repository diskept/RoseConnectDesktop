#include "qobuz/ConvertData_forQobuz.h"
#include "ProcCommon_forQobuz.h"
#include "ProcRosePlay_withQobuz.h"
#include "common/global.h"
#include <QDebug>
#include <QException>
#include <common/networkhttp.h>
#include "common/ProcJsonEasy.h"
#include <common/sqlitehelper.h>

namespace qobuz {

    QString QUERY_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS User_Qobuz (email VARCHAR(50), password TEXT, token VARCHAR(1023), id VARCHAR(50))";
    QString QUERY_DROP_TABLE = "DROP TABLE User_Qobuz";
    QString QUERY_DEL = "DELETE FROM User_Qobuz";
    QString QUERY_SEL = "SELECT * FROM User_Qobuz";


    /**
     * @brief 기본 생성자.
     */
    ProcCommon::ProcCommon(QWidget *parent) : QObject(parent) {
    }


    /**
     * @brief 소멸자. 메모리 해제 체크확인용
     */
    ProcCommon::~ProcCommon(){
    }




    // MARK : public functions  --------------------------------------------------------------------------------------------------------

    /**
     * @brief 타이달 API에서 반환하는 이미지 key값에 대해서, 타이달 Resource URL로 변환하여 반환
     * @details  베어앱 노트 참고 - [[Rose Qobuz - 이미지 URL화 방법]]
     * @param qobuzImageKey 타이달API에서 반환하는 이미지 key값
     * @param imageSize 이미지 리소스에서 제공하는 가로세로 동일값 사이즈. 타이달API에 따라 차이가 있음.
     * @return 타이달 Resource Url 반환
     */
    QString ProcCommon::convertImagePath(const QString qobuzImageKey, const int imageSize){
        if(qobuzImageKey.isEmpty()){
            return "";
        }
        else if(imageSize <= 0){
            return "";
        }
        else{
            QString output = qobuzImageKey;
            output.replace(QString("-"), QString("/"), Qt::CaseSensitive);
//            output.insert(0, "https://resources.qobuz.com/images/");          // https
            output.insert(0, "http://resources.QOBUZ.com/images/");             // http
            output.append( QString("/%1x%1.jpg").arg(imageSize) );
            return output;
        }

    }




    /**
     * @brief 타이달 로그인 정보를 sqllite3 정보로 저장함
     * @param username : 타이달 사용자 email 정보
     * @param password : 타이달 사용자 비밀번호 정보
     */
    void ProcCommon::saveLoginInfo_qobuzDB(UserLoginInfo &userLoginInfo){
        QString strQuery_insert = " INSERT INTO User_Qobuz (email, password, token, id) VALUES ";
        strQuery_insert.append( QString("('%1', '%2', '%3', '%4')").arg(userLoginInfo.username).arg(userLoginInfo.password).arg(userLoginInfo.auth_token).arg(userLoginInfo.user_id));

        // 항상 테이블 생성을 체크하고, 삭제처리 한다. for 초기화 (항상 레코드 최대 1개 유지)
        this->procQueryList(QList<QString>{ QUERY_CREATE_TABLE, QUERY_DEL, strQuery_insert });
    }

    void ProcCommon::clearTablesInfo_qobuzDB(){
        // 항상 테이블 생성을 체크하고, 삭제처리 한다.
        this->procQueryList(QList<QString>{ QUERY_CREATE_TABLE, QUERY_DEL });
    }

    /**
     * @brief 타이달 로그인 정보를 clear 처리함
     */
    void ProcCommon::clearLoginInfo_qobuzDB(){
        // 항상 테이블 생성을 체크하고, 삭제처리 한다.
        this->procQueryList(QList<QString>{ QUERY_CREATE_TABLE, QUERY_DEL });
    }


    /**
     * @brief 로그인 정보를 SELECT 해서 userLoginInfo에 세팅한다.
     * @param userLoginInfo
     */
    UserLoginInfo ProcCommon::getLoginInfo_qobuzDB(){

        UserLoginInfo output_userLoginInfo;

        SqliteHelper *sqliteHelper = new SqliteHelper(this);
        QSqlError err = sqliteHelper->addConnectionLocal();

        sqliteHelper->exec(QUERY_CREATE_TABLE);

        // SELECT 처리
        QVariantList *list = new QVariantList();
        sqliteHelper->exec(QUERY_SEL, *list);
        if(list->size() > 0){
            foreach(QVariant val, *list){
                QMap<QString, QVariant> map = val.toMap();
                output_userLoginInfo.username = map["email"].toString();
                output_userLoginInfo.password = map["password"].toString();
                output_userLoginInfo.auth_token = map["token"].toString();
                output_userLoginInfo.user_id =  map["id"].toString();
                output_userLoginInfo.flagSavedLoginInfo = true;
                //qDebug() << "ProcCommon::getLoginInfo_qobuzDB()------------------------------";
                //qDebug() << output_userLoginInfo.username;
                //qDebug() << output_userLoginInfo.password;
                break;
            }
        }

        if(output_userLoginInfo.auth_token.isEmpty())
        {
            sqliteHelper->exec(QUERY_DROP_TABLE);
        }

        sqliteHelper->close();
        delete sqliteHelper;

        return output_userLoginInfo;
    }





    // MARK : Request API - 일반 데이터 요청 (List or 단일 형태) -------------------------------------------------------------------

    /**
     * @brief HTTP 요청 - Qobuz 로그인 API
     * @param userLoginInfo
     * @see
     */
    void ProcCommon::request_qobuzLogin(UserLoginInfo userLoginInfo){

        // slot 에서 추가처리를 위해 멤버변수로 저장해둠
        this->userLoginInfo = userLoginInfo;

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("username", userLoginInfo.username);

        // Convert password format to md5 format    Added code 21/12/2020 by cheon
        QString qstrReadpw = userLoginInfo.password;
        QByteArray byteArray = qstrReadpw.toUtf8();
        const char *chrTest = byteArray.constData();

        QString str_pw_md5 = QString(QCryptographicHash::hash((chrTest),QCryptographicHash::Md5).toHex());        
        params.addQueryItem("password", str_pw_md5);

        //params.addQueryItem("clientUniqueKey", "");
        QString header1 = "RS201";
        params.addQueryItem("device_manufacturer_id", header1.toUtf8());        

        //qDebug() << "ProcCommon::request_qobuzLogin()--------------";       //cheon_debug01
        //qDebug() << userLoginInfo.username;
        //qDebug() << userLoginInfo.password;
        //qDebug() << "password" << str_pw_md5;

        network->request_forQobuz(Login,
                         QString("%1/user/login").arg(global.qobuzAPI_url),
                         NetworkHttp::HeaderOption_forQobuz::X_App_Id,
                         params,
                         NetworkHttp::RequestMethod::Post);

    }



    /**
     * @brief HTTP 요청 - Qobuz 로그아웃 API
     * @see
     */
    void ProcCommon::request_qobuzLogout(){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        network->request_forQobuz(Logout,
                         QString("%1/logout").arg(global.qobuzAPI_url),
                         NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session,
                         QUrlQuery{},
                         NetworkHttp::RequestMethod::Post);
    }


    /**
     * @brief HTTP 요청 - 타이달 Genres 종류 전체반환 API
     * @details - @see list_genre 변수에 데이터가 없는 경우에만 내부 동작한다. (최초 1번만 세팅하기 위함)
     */
    void ProcCommon::request_qobuz_getListGenres(){
        if(list_genre.length() == 0){
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QUrlQuery params;
            //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
            //params.addQueryItem("limit", "25");
            //params.addQueryItem("offset", "0");
            //params.addQueryItem("parent_id", "64");

            network->request_forQobuz(GetList_Genres,
                             QString("%1/genre/list").arg(global.qobuzAPI_url),
                             NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session,
                             params,
                             NetworkHttp::RequestMethod::Get);
        }
    }


    /**
     * @brief HTTP 요청 - 타이달 Moods & Activities 종류 전체반환 API
     * @details - @see list_mood 변수에 데이터가 없는 경우에만 내부 동작한다. (최초 1번만 세팅하기 위함)
     */
    void ProcCommon::request_qobuz_getListMoods(){
        if(list_mood.length() == 0){
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QUrlQuery params;
            //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
            network->request_forQobuz(GetList_Moods,
                             QString("%1/moods").arg(global.qobuzAPI_url),
                             NetworkHttp::HeaderOption_forQobuz::X_App_Id,
                             params,
                             NetworkHttp::RequestMethod::Get);
        }
    }


    /**
     * @brief Qobuz API - Playlist 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_playlists(const QString api_subPath, const QString type, const QString genre_ids, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("type", "editor-picks");
        //params.addQueryItem("genre_id", "0");

        params.addQueryItem("type", type);
        params.addQueryItem("genre_id", genre_ids);

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        //print_qobuz_func();
        //qDebug() << "QOBUZ request url : " << global.qobuzAPI_url << "/" << api_subPath << "\n";

        network->request_forQobuz(HttpRequestType::GetList_Playlists
                                  , QString("%1/%2").arg(global.qobuzAPI_url).arg(api_subPath)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Qobuz API - Album 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_albums(const QString api_subPath, const QString type, const QString genre_ids, const int limit, const int offset){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("type", "new-releases");
        //params.addQueryItem("genre_id", "0");

        params.addQueryItem("type", type);
        params.addQueryItem("genre_id", genre_ids);

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forQobuz(HttpRequestType::GetList_Albums,
                         QString("%1/%2").arg(global.qobuzAPI_url).arg(api_subPath),
                         NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session,
                         params,
                         NetworkHttp::RequestMethod::Get);
    }


    void ProcCommon::request_qobuz_getList_lastrelease(const QString path, const QString extra, const int artist_id, const int limit, const int offset){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/artist/get?artist_id=53389&extra=playlists%2Calbums_with_last_release%2Ctracks_appears_on&limit=20&offset=0
        QString extra_opt = extra;
        QUrlQuery params;
        params.addQueryItem("artist_id", QString("%1").arg(artist_id));
        params.addQueryItem("extra", extra_opt);

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forQobuz(HttpRequestType::GetList_LastRelease,
                         QString("%1/%2").arg(global.qobuzAPI_url).arg(path),
                         NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session,
                         params,
                         NetworkHttp::RequestMethod::Get);
    }


    /**
     * @brief Qobuz API - Artist 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_artists(const QString api_subPath, const int artist_id, const int limit, const int offset){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        params.addQueryItem("artist_id", QString("%1").arg(artist_id));


        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset > 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forQobuz(HttpRequestType::GetList_Artists
                                  , QString("%1/%2").arg(global.qobuzAPI_url).arg(api_subPath)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }



    /**
     * @brief Qobuz API - Track 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_tracks(const QString api_subPath, const int limit, const int offset){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }
        //qDebug() << "###ProcCommon::request_qobuz_getList_albums()---------global.qobuzAPI_url : " << global.qobuzAPI_url;
        //qDebug() << "###ProcCommon::request_qobuz_getList_albums()---------api_subPath : " << api_subPath;

        network->request_forQobuz(HttpRequestType::GetList_Tracks
                                  , QString("%1/%2").arg(global.qobuzAPI_url).arg(api_subPath)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Qobuz API - Track 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_track(const QString api_subPath, const int track_id, const int limit, const int offset){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("track_id", QString("%1").arg(track_id));

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forQobuz(HttpRequestType::GetList_Tracks
                                  , QString("%1/%2").arg(global.qobuzAPI_url).arg(api_subPath)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    void ProcCommon::request_qobuz_getList_appearson(const QString path, const QString extra, const int artist_id, const int limit, const int offset){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());

        params.addQueryItem("artist_id", QString("%1").arg(artist_id));
        params.addQueryItem("extra", extra);

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        /*network->request_forQobuz(HttpRequestType::GetList_Tracks
                                  , QString("%1/%2").arg(global.qobuzAPI_url).arg(api_subPath)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);*/

        network->request_forQobuz(HttpRequestType::GetList_Appearson,
                         QString("%1/%2").arg(global.qobuzAPI_url).arg(path),
                         NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session,
                         params,
                         NetworkHttp::RequestMethod::Get);
    }


    /**
     * @brief Qobuz API - Video 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_videos(const QString api_subPath, const int limit, const int offset){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        if(limit > 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset > 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forQobuz(HttpRequestType::GetList_Videos
                                  , QString("%1/%2").arg(global.qobuzAPI_url).arg(api_subPath)
                                  , NetworkHttp::HeaderOption_forQobuz::X_App_Id
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }





    /**
     * @brief Qobuz API - Album 1개에 대한 상세정보 요청
     * @param album_id
     */
    //void ProcCommon::request_qobuz_get_album(const int album_id){
    void ProcCommon::request_qobuz_get_album(const QString album_id){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("album_id", album_id);
        //params.addQueryItem("extra", "albumsFromSameArtist");
        //params.addQueryItem("limit", "250");
        //params.addQueryItem("offset", "0");

        network->request_forQobuz(HttpRequestType::Get_AlbumInfo
                                  //, QString("%1/albums/%2").arg(global.qobuzAPI_url).arg(album_id)
                                  , QString("%1/album/get").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }

    /**
     * @brief Qobuz API - Album 1개에 대한 상세정보 요청
     * @param album_id
     */
    void ProcCommon::request_qobuz_get_albuminfo(const QString album_id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("album_id", album_id);
        params.addQueryItem("extra", "albumsFromSameArtist");
        params.addQueryItem("limit", "0");
        params.addQueryItem("offset", "0");

        network->request_forQobuz(HttpRequestType::Get_AlbumInfo
                                  , QString("%1/album/get").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }

    /**
     * @brief Qobuz API - Artist 1개에 대한 상세정보 요청
     * @param artist_id
     */
    void ProcCommon::request_qobuz_get_artist(const int artist_id){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        params.addQueryItem("artist_id", QString("%1").arg(artist_id));

        /*network->request_forQobuz(HttpRequestType::Get_ArtistInfo
                                  , QString("%1/artists/%2").arg(global.qobuzAPI_url).arg(artist_id)
                                  , NetworkHttp::HeaderOption_forQobuz::X_App_Id
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);*/

        network->request_forQobuz(HttpRequestType::Get_ArtistInfo
                                  , QString("%1/artist/get").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }



    /**
     * @brief Qobuz API - Artist 1개에 대하여 bio 정보 텍스트(description) 요청
     * @param artist_id
     */
    void ProcCommon::request_qobuz_get_artist_bioText(const int artist_id){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/artist/get?artist_id=388363

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        params.addQueryItem("artist_id", QString("%1").arg(artist_id));

        /*network->request_forQobuz(HttpRequestType::Get_ArtistBioText
                                  , QString("%1/artists/%2/bio").arg(global.qobuzAPI_url).arg(artist_id)
                                  , NetworkHttp::HeaderOption_forQobuz::X_App_Id
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);*/

        network->request_forQobuz(HttpRequestType::Get_ArtistBioText
                                  , QString("%1/artist/get").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Qobuz API - 나의 Created Playlist 목록을 요청
     * @param orderOpt 정렬옵션
     * @param orderDirection 정렬방향 (내림, 올림)
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_myCreatedPlaylists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset, const QJsonObject& jsonObj_opt){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/playlist/getUserPlaylists?limit=20&offset=0&sort=updated_at

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        //params.addQueryItem("order", QString("%1").arg( QVariant::fromValue(orderOpt).toString() ));
        //params.addQueryItem("orderDirection", QString("%1").arg( QVariant::fromValue(orderDirection).toString() ));

        if(limit > 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }        
        /*if(QString("%1").arg(QVariant::fromValue(orderOpt).toString()) == "DATE")
        {
            params.addQueryItem("sort", "updated_at");
        }
        if(QString("%1").arg(QVariant::fromValue(orderDirection).toString()) == "ASC")
        {
            params.addQueryItem("order", "asc");
        }*/
        params.addQueryItem("sort", "updated_at");
        params.addQueryItem("order", "desc");

        /*network->request_forQobuz(HttpRequestType::GetList_My_Created_Playlists
                                  , QString("%1/users/%2/playlists").arg(global.qobuzAPI_url).arg(global.user_forQobuz.getUserId())
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false
                                  , jsonObj_opt);*/

        network->request_forQobuz(HttpRequestType::GetList_My_Created_Playlists
                                  , QString("%1/playlist/getUserPlaylists").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false
                                  , jsonObj_opt);

    }



    /**
     * @brief Qobuz API - 나의 Favorite Playlist 목록을 요청
     * @param orderOpt
     * @param orderDirection
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_myFavoritePlaylists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        params.addQueryItem("order", QString("%1").arg( QVariant::fromValue(orderOpt).toString() ));
        params.addQueryItem("orderDirection", QString("%1").arg( QVariant::fromValue(orderDirection).toString() ));
        if(limit > 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset > 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forQobuz(HttpRequestType::GetList_My_Favorite_Playlists
                                  , QString("%1/users/%2/favorites/playlists").arg(global.qobuzAPI_url).arg(global.user_forQobuz.getUserId())
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);

    }


    /**
     * @brief Qobuz API - 나의 Favorite Album 목록을 요청
     * @param orderOpt
     * @param orderDirection
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_myFavoriteAlbums(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/favorite/getUserFavorites?type=albums&limit=20&offset=0

        QUrlQuery params;
        params.addQueryItem("type", "albums");

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }
        if(QString("%1").arg(QVariant::fromValue(orderOpt).toString()) == "DATE")
        {
            params.addQueryItem("sort", "updated_at");
        }
        if(QString("%1").arg(QVariant::fromValue(orderDirection).toString()) == "ASC")
        {
            params.addQueryItem("order", "asc");
        }


        network->request_forQobuz(HttpRequestType::GetList_My_Favorite_Albums
                                  , QString("%1/favorite/getUserFavorites").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Qobuz API - 나의 Favorite Track 목록을 요청
     * @param orderOpt
     * @param orderDirection
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_myFavoriteTracks(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/favorite/getUserFavorites?type=tracks&limit=20&offset=0

        QUrlQuery params;
        params.addQueryItem("type", "tracks");

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        if(QString("%1").arg(QVariant::fromValue(orderOpt).toString()) == "DATE")
        {
            params.addQueryItem("sort", "updated_at");
        }
        if(QString("%1").arg(QVariant::fromValue(orderDirection).toString()) == "ASC")
        {
            params.addQueryItem("order", "asc");
        }

        network->request_forQobuz(HttpRequestType::GetList_My_Favorite_Tracks
                                  , QString("%1/favorite/getUserFavorites").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Qobuz API - 나의 Favorite Artist 목록을 요청
     * @param orderOpt
     * @param orderDirection
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_myFavoriteArtists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset) {
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/favorite/getUserFavorites?type=artists&limit=20&offset=0

        QUrlQuery params;
        params.addQueryItem("type", "artists");

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }
        if(QString("%1").arg(QVariant::fromValue(orderOpt).toString()) == "DATE")
        {
            params.addQueryItem("sort", "updated_at");
        }
        if(QString("%1").arg(QVariant::fromValue(orderDirection).toString()) == "ASC")
        {
            params.addQueryItem("order", "asc");
        }


        network->request_forQobuz(HttpRequestType::GetList_My_Favorite_Artists
                                  , QString("%1/favorite/getUserFavorites").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Qobuz API - 나의 Favorite Video 목록을 요청
     * @param orderOpt
     * @param orderDirection
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_myFavoriteVideos(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset) {
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        params.addQueryItem("order", QString("%1").arg( QVariant::fromValue(orderOpt).toString() ));
        params.addQueryItem("orderDirection", QString("%1").arg( QVariant::fromValue(orderDirection).toString() ));
        if(limit > 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset > 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forQobuz(HttpRequestType::GetList_My_Favorite_Videos
                                  , QString("%1/users/%2/favorites/videos").arg(global.qobuzAPI_url).arg(global.user_forQobuz.getUserId())
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }




    void ProcCommon::request_qobuz_getList_myPurchasesAlbums(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/purchase/getUserPurchases?type=albums&limit=20&offset=0

        QUrlQuery params;
        params.addQueryItem("type", "albums");

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }
        if(QString("%1").arg(QVariant::fromValue(orderOpt).toString()) == "DATE")
        {
            params.addQueryItem("sort", "updated_at");
        }
        if(QString("%1").arg(QVariant::fromValue(orderDirection).toString()) == "ASC")
        {
            params.addQueryItem("order", "asc");
        }


        network->request_forQobuz(HttpRequestType::GetList_My_Purchases_Albums
                                  , QString("%1/purchase/getUserPurchases").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    void ProcCommon::request_qobuz_getList_myPurchasesTracks(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/purchase/getUserPurchases?type=tracks&limit=20&offset=0

        QUrlQuery params;
        params.addQueryItem("type", "tracks");

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        if(QString("%1").arg(QVariant::fromValue(orderOpt).toString()) == "DATE")
        {
            params.addQueryItem("sort", "updated_at");
        }
        if(QString("%1").arg(QVariant::fromValue(orderDirection).toString()) == "ASC")
        {
            params.addQueryItem("order", "asc");
        }

        network->request_forQobuz(HttpRequestType::GetList_My_Purchases_Tracks
                                  , QString("%1/purchase/getUserPurchases").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }




    /**
     * @brief Qobuz API - Playlist 에 있는 track 목록을 요청
     * @param playlist_uuid
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_getList_items_of_playlist(const QString playlist_id, const int limit, const int offset){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/playlist/get?playlist_id=5551640&limit=1000&offset=0&extra=tracks%2CgetSimilarPlaylists

        QUrlQuery params;
        params.addQueryItem("playlist_id", playlist_id);       // Added Jeon 29/12/2020
        params.addQueryItem("extra", "tracks%2CgetSimilarPlaylists");
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());

        if(limit > 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset > 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        /*network->request_forQobuz(GetList_Items_Playlist
                                  , QString("%1/playlists/%2/items").arg(global.qobuzAPI_url).arg(playlist_id)
                                  , NetworkHttp::HeaderOption_forQobuz::X_App_Id
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);*/

        network->request_forQobuz(GetList_Items_Playlist
                                  , QString("%1/playlist/get").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Qobuz API - Album 에 있는 track 목록을 요청.
     * @details limit, offset 없음. 보통 alubm 내에 트랙 개수는 적으니까,,, Qobuz Api에 옵션이 없음.
     * @param album_id
     */
    //void ProcCommon::request_qobuz_getList_items_of_album(const int album_id){
    void ProcCommon::request_qobuz_getList_items_of_album(const QString album_id){

        //print_qobuz_func();
        //qDebug() << "5. [QOBUZ][SEQ][FUNC] : ProcCommon_forQobuz.cpp -> ProcCommon::request_qobuz_getList_items_of_album(const QString album_id)";
        //qDebug() << "[QOBUZ][SEQ][MSG] send Request JSON to QOBUZ -> goto Nest step\n";

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        /*QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        network->request_forQobuz(GetList_Items_Album
                                  , QString("%1/albums/%2/tracks").arg(global.qobuzAPI_url).arg(album_id)
                                  , NetworkHttp::HeaderOption_forQobuz::X_App_Id
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);*/
        // Added Jeon 29/12/2020
        QUrlQuery params;
        params.addQueryItem("album_id", album_id);
        //params.addQueryItem("extra", "albumsFromSameArtist");

        network->request_forQobuz(HttpRequestType::GetList_Items_Album
                                  , QString("%1/album/get").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }







    // MARK : Favorites 처리 ( get & set ) -----------------------------------------------------------------------------------

    /**
     * @brief QOBUZ API 요청 - 좋아요 체크 여부를 확인하기 위해 필요한 정보임
     */
    void ProcCommon::request_qobuz_getAll_favorites(){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        network->request_forQobuz(HttpRequestType::GetAll_UserFavorites
                                  , QString("%1/users/%2/favorites/ids").arg(global.qobuzAPI_url).arg(global.user_forQobuz.getUserId())
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }

    void ProcCommon::request_qobuz_get_favorite(const QString type, const QString id)
    {
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/favorite/status?item_id=qf0o8z4sglqjb&type=album;
        //https://www.qobuz.com/api.json/0.2/playlist/subscribe?playlist_id=2230221
        QString url;
        QUrlQuery params;
        if(type != "playlist")
        {
            url = QString("%1/favorite/status").arg(global.qobuzAPI_url);
            params.addQueryItem("item_id", id);
            params.addQueryItem("type", type);
        }
        else
        {
            url = QString("%1/playlist/subscribe").arg(global.qobuzAPI_url);
            params.addQueryItem("playlist_id", id);
        }

        network->request_forQobuz(HttpRequestType::GetAll_UserFavorites
                                  , url
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    void ProcCommon::request_qobuz_set_favorite(const QString type, const QString id, const bool flag)
    {
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/favorite/toggle?item_id=qf0o8z4sglqjb&type=album
        //https://www.qobuz.com/api.json/0.2/playlist/subscribe?playlist_id=2230221
        QString url;
        QUrlQuery params;
        if(type != "playlist")
        {
            url = QString("%1/favorite/toggle").arg(global.qobuzAPI_url);
            params.addQueryItem("item_id", id);
            params.addQueryItem("type", type);
        }
        else
        {
            if(flag == true){
                url = QString("%1/playlist/subscribe").arg(global.qobuzAPI_url);
                params.addQueryItem("playlist_id", id);
            }
            else if(flag == false){
                url = QString("%1/playlist/unsubscribe").arg(global.qobuzAPI_url);
                params.addQueryItem("playlist_id", id);
            }
        }

        network->request_forQobuz(HttpRequestType::GetAll_UserFavorites
                                  , url
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Post
                                  , false);
    }



    // 즐겨찾기 추가 ---------------------------------------------------------------------------------------------

    /**
     * @brief QOBUZ API 요청 - 즐겨찾기 추가 (Album)
     * @param album_id
     */
    //void ProcCommon::request_qobuz_addFavorite_album(const int album_id){         // Added Jeon 27/12/2020
    void ProcCommon::request_qobuz_addFavorite_album(const QString album_id){
        this->request_qobuz_addFavorie(Favorite_Add_Album, "albumIds", album_id, "albums");
    }

    /**
     * @brief QOBUZ API 요청 - 즐겨찾기 추가 (Aritst)
     * @param artist_id
     */
    void ProcCommon::request_qobuz_addFavorite_artist(const int artist_id){
        this->request_qobuz_addFavorie(Favorite_Add_Artist, "artistIds", artist_id, "artists");
    }

    /**
     * @brief QOBUZ API 요청 - 즐겨찾기 추가 (Track)
     * @param artist_id
     */
    void ProcCommon::request_qobuz_addFavorite_track(const int track_id){
        this->request_qobuz_addFavorie(Favorite_Add_Track, "trackIds", track_id, "tracks");
    }

    /**
     * @brief QOBUZ API 요청 - 즐겨찾기 추가 (Playlist)
     * @param artist_id
     */
    void ProcCommon::request_qobuz_addFavorite_playlist(const QString playlist_id){
    //void ProcCommon::request_qobuz_addFavorite_playlist(const int playlist_id){
        this->request_qobuz_addFavorie(Favorite_Add_Playlist, "uuids", playlist_id, "playlists");
        //this->request_qobuz_addFavorie(Favorite_Add_Playlist, "uuids", playlist_id, "playlists");
    }


    /**
     * @brief QOBUZ API 요청 - 즐겨찾기 추가 (Video)
     * @param artist_id
     */
    void ProcCommon::request_qobuz_addFavorite_video(const int video_id){
        this->request_qobuz_addFavorie(Favorite_Add_Video, "videoIds", video_id, "videos");
    }



    /**
     * @brief QOBUZ API 요청 - 각 타입별로 즐겨찾기 추가에 대해 요청
     * @param requestType
     * @param id_key
     * @param id_val
     * @param subPath
     */
    void ProcCommon::request_qobuz_addFavorie(const HttpRequestType requestType, const QString id_key, const QVariant id_val, const QString subPath){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        QUrlQuery params_post;
        params_post.addQueryItem(id_key, id_val.toString());

        QUrlQuery params_get;
        //params_get.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());

        network->request_forQobuz(requestType,
                                  QString("%1/users/%2/favorites/%3").arg(global.qobuzAPI_url).arg(global.user_forQobuz.getUserId()).arg(subPath),
                                  NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session,
                                  params_post,
                                  params_get,
                                  false);
    }



    // 즐겨찾기 삭제 ---------------------------------------------------------------------------------------------

    /**
     * @brief QOBUZ API 요청 - 즐겨찾기 삭제 (Album)
     * @param album_id
     */
    //void ProcCommon::request_qobuz_deleteFavorite_album(const int album_id){      // Added Jeon 27/12/2020
    void ProcCommon::request_qobuz_deleteFavorite_album(const QString album_id){
        this->request_qobuz_deleteFavorie(Favorite_Delete_Album, album_id, "albums");
    }

    /**
     * @brief QOBUZ API 요청 - 즐겨찾기 삭제 (Artist)
     * @param artist_id
     */
    void ProcCommon::request_qobuz_deleteFavorite_artist(const int artist_id){
        this->request_qobuz_deleteFavorie(Favorite_Delete_Artist, artist_id, "artists");
    }

    /**
     * @brief QOBUZ API 요청 - 즐겨찾기 삭제 (Track)
     * @param track_id
     */
    void ProcCommon::request_qobuz_deleteFavorite_track(const int track_id){
        this->request_qobuz_deleteFavorie(Favorite_Delete_Track, track_id, "tracks");
    }

    /**
     * @brief QOBUZ API 요청 - 즐겨찾기 삭제 (Playlist)
     * @param playlist_uuid
     */
    void ProcCommon::request_qobuz_deleteFavorite_playlist(const QString playlist_id){
    //void ProcCommon::request_qobuz_deleteFavorite_playlist(const int playlist_id){
        this->request_qobuz_deleteFavorie(Favorite_Delete_Playlist, playlist_id, "playlists");
        //this->request_qobuz_deleteFavorie(Favorite_Delete_Playlist, playlist_id, "playlists");
    }

    /**
     * @brief QOBUZ API 요청 - 즐겨찾기 삭제 (Video)
     * @param video_id
     */
    void ProcCommon::request_qobuz_deleteFavorite_video(const int video_id){
        this->request_qobuz_deleteFavorie(Favorite_Delete_Video, video_id, "videos");
    }

    /**
     * @brief QOBUZ API 요청 - 각 타입별로 즐겨찾기 삭제에 대해 요청
     * @param requestType
     * @param id_val
     * @param subPath
     */
    void ProcCommon::request_qobuz_deleteFavorie(const HttpRequestType requestType, const QVariant id_val, const QString subPath){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        network->request_forQobuz(requestType,
                                  QString("%1/users/%2/favorites/%3/%4").arg(global.qobuzAPI_url).arg(global.user_forQobuz.getUserId()).arg(subPath).arg(id_val.toString()),
                                  NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session,
                                  QUrlQuery(),
                                  NetworkHttp::RequestMethod::Delete,
                                  false);
    }






    // User's Playlist 관련 -------------------------------------------------------------------------------------------

    /**
     * @brief User created playlist 를 삭제 요청
     * @param playlist_uuid
     */
    void ProcCommon::request_qobuz_delete_myCreatedPlaylist(const QString playlist_id, const QJsonObject& jsonObj_opt){
    //void ProcCommon::request_qobuz_delete_myCreatedPlaylist(const int playlist_id, const QJsonObject& jsonObj_opt){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        network->request_forQobuz(ProcCommon::HttpRequestType::UserCreatePlaylist_Delete
                                  , QString("%1/playlists/%2").arg(global.qobuzAPI_url).arg(playlist_id)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , QUrlQuery()
                                  , NetworkHttp::RequestMethod::Delete
                                  , false
                                  , jsonObj_opt);

    }


    /**
     * @brief request_qobuz_addPlaylist_track
     * @param playlist_uuid
     * @param track_id
     */
    void ProcCommon::request_qobuz_add_track_in_myPlaylist(const QString playlist_uuid_target, const int track_id){

        this->request_qobuz_addPlaylist_items(playlist_uuid_target, QString("%1").arg(track_id));
    }

    /**
     * @brief ProcCommon::request_qobuz_add_album_in_myPlaylist
     * @param playlist_uuid_target
     * @param album_id
     */
    void ProcCommon::request_qobuz_add_album_in_myPlaylist(const QString playlist_uuid_target, const int album_id){
        // album_id 로 내부 트랙 목록 요청 필요
        QJsonObject jsonObj_opt;
        jsonObj_opt.insert("playlist_uuid_target", playlist_uuid_target);
        connect(this, &ProcCommon::completeReq_list_trackIds_toAddPlaylist, this, &ProcCommon::slot_getDone_trackIds_toAddPlaylist);
        this->request_qobuz_getList_trackIds_of_album(album_id, jsonObj_opt);

    }

    /**
     * @brief ProcCommon::request_qobuz_add_playlist_in_myPlaylist
     * @param playlist_uuid_target
     * @param playlist_uuid
     */
    void ProcCommon::request_qobuz_add_playlist_in_myPlaylist(const QString playlist_uuid_target, const QString playlist_uuid){
        // playlist_uuid 로 내부 트랙 목록 요청 필요
        QJsonObject jsonObj_opt;
        jsonObj_opt.insert("playlist_uuid_target", playlist_uuid_target);
        connect(this, &ProcCommon::completeReq_list_trackIds_toAddPlaylist, this, &ProcCommon::slot_getDone_trackIds_toAddPlaylist);
        this->request_qobuz_getList_trackIds_of_playlist(playlist_uuid, jsonObj_opt);
    }


    /**
     * @brief 플레이리스트 담기 요청
     * @param uuid
     * @param itemIds
     */
    void ProcCommon::request_qobuz_addPlaylist_items(QString playlist_uuid, QString itemIds){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        //https://www.qobuz.com/api.json/0.2/playlist/addTracks?playlist_id=5377183&track_ids=20278300
        QUrlQuery params;
        params.addQueryItem("playlist_id", playlist_uuid);
        params.addQueryItem("track_ids", itemIds);

        network->request_forQobuz(ProcCommon::HttpRequestType::UserCreatePlaylist_Add_Tracks
                                  , QString("%1/playlist/addTracks").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Post
                                  , false);

    }


    /**
     * @brief ProcCommon::request_qobuz_getList_trackIds_of_playlist
     * @param playlist_uuid
     */
    void ProcCommon::request_qobuz_getList_trackIds_of_playlist(const QString playlist_uuid, const QJsonObject& jsonObj_opt){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        params.addQueryItem("offset", "0");
        params.addQueryItem("limit", "99");         // 99 몇개를 넣어야할까.
        network->request_forQobuz(GetList_TrackIds_Playlist
                                  , QString("%1/playlists/%2/items").arg(global.qobuzAPI_url).arg(playlist_uuid)
                                  , NetworkHttp::HeaderOption_forQobuz::X_App_Id
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false
                                  , jsonObj_opt);
    }

    /**
     * @brief Album 안에 있는 모든 트랙에 대한 id 만을 따로 추출하기 위해 요청함.
     * @details 원래 @see request_qobuz_getList_items_of_album 함수와 바디는 같다.
     * 그러나 결과물 처리 및 연결 signal을 구분하기 위해서 함수를 따로 분리하였음.
     * @param album_id
     */
    void ProcCommon::request_qobuz_getList_trackIds_of_album(const int album_id, const QJsonObject& jsonObj_opt){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        network->request_forQobuz(GetList_TrackIds_Album
                                  , QString("%1/albums/%2/tracks").arg(global.qobuzAPI_url).arg(album_id)
                                  , NetworkHttp::HeaderOption_forQobuz::X_App_Id
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false
                                  , jsonObj_opt);
    }


    /**
     * @brief completeReq_list_trackIds_toAddPlaylist 시그널에 대한 처리를 진행
     */
    void ProcCommon::slot_getDone_trackIds_toAddPlaylist(const QString playlist_uuid_target, const QString track_ids){
        this->request_qobuz_addPlaylist_items(playlist_uuid_target, track_ids);
    }




    /**
     * @brief ProcCommon::request_qobuz_delete_items_in_myPlaylist
     * @param playlist_uuid_target
     * @param list_itemIds
     */
    void ProcCommon::request_qobuz_delete_items_in_myPlaylist(const QString playlist_id, const QList<QString>& list_itemIds){
    //void ProcCommon::request_qobuz_delete_items_in_myPlaylist(const int playlist_id, const QList<QString>& list_itemIds){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        network->request_forQobuz(DeleteItems_in_MyCreatedPlaylist
                                  , QString("%1/playlists/%2/items/%3").arg(global.qobuzAPI_url).arg(playlist_id).arg(list_itemIds.join(","))
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session_with_IF_None_Match
                                  , params
                                  , NetworkHttp::RequestMethod::Delete
                                  , false);

    }


    void ProcCommon::request_qobuz_create_myPlaylist(const QString playlislt_name, const QString description){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/playlist/create?name=PC%20DEV%20TEST&description=

        QUrlQuery params;
        params.addQueryItem("name", playlislt_name);
        params.addQueryItem("description", description);

        network->request_forQobuz(HttpRequestType::Create_userPlaylist
                                  , QString("%1/playlist/create").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Post
                                  , false);
    }


    void ProcCommon::request_qobuz_addTracks_myPlaylist(const QString playlist_id, const QString track_ids){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/playlist/addTracks?playlist_id=7776259&track_ids=134787662%2C

        QUrlQuery params;
        params.addQueryItem("playlist_id", playlist_id);
        params.addQueryItem("track_ids", track_ids);

        network->request_forQobuz(HttpRequestType::AddTracks_userPlaylist
                                  , QString("%1/playlist/addTracks").arg(global.qobuzAPI_url)
                                  , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                  , params
                                  , NetworkHttp::RequestMethod::Post
                                  , false);
    }


    /**
     * @brief ProcCommon::request_qobuz_searchAllItems
     * @param search_word
     * @param limit
     * @param offset
     */
    void ProcCommon::request_qobuz_searchAllItems(const QString search_word, const int limit, const int offset, const QobuzSearch_ContentType contentType){

        /*HttpRequestType httpRequestType = HttpRequestType::Search_GetList_Track;
        QString makeURL;

        if(contentType == QobuzSearch_ContentType::MostPopular){
            httpRequestType = HttpRequestType::Search_GetList_MostPopular;
            makeURL = QString("%1/most-popular/get").arg(global.qobuzAPI_url);
        }
        else if(contentType == QobuzSearch_ContentType::Artists){
            httpRequestType = HttpRequestType::Search_GetList_Artist;
            makeURL = QString("%1/artist/search").arg(global.qobuzAPI_url);
        }
        else if(contentType == QobuzSearch_ContentType::Albums){
            httpRequestType = HttpRequestType::Search_GetList_Album;
            makeURL = QString("%1/album/search").arg(global.qobuzAPI_url);
        }
        else if(contentType == QobuzSearch_ContentType::Tracks){
            httpRequestType = HttpRequestType::Search_GetList_Track;
            makeURL = QString("%1/track/search").arg(global.qobuzAPI_url);
        }
        else if(contentType == QobuzSearch_ContentType::Playlists){
            httpRequestType = HttpRequestType::Search_GetList_Playlist;
            makeURL = QString("%1/playlist/search").arg(global.qobuzAPI_url);
        }

        //QString str_contentType = QVariant::fromValue(contentType).toString().toUpper();

        // request
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("countryCode", global.user_forQobuz.getCountryCode());
        //params.addQueryItem("types", str_contentType);
        params.addQueryItem("query", search_word);
        params.addQueryItem("limit", QString("%1").arg(limit));
        params.addQueryItem("offset", QString("%1").arg(offset));

        network->request_forQobuz(httpRequestType
                                          , QString("%1").arg(makeURL)
                                          , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                          , params
                                          , NetworkHttp::RequestMethod::Get
                                          , false);*/

        // request
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://www.qobuz.com/api.json/0.2/catalog/search?query=IU&limit=30&offset=0
        QString url = global.qobuzAPI_url + "/catalog/search";

        QUrlQuery params;
        params.addQueryItem("query", search_word);
        params.addQueryItem("limit", QString("%1").arg(limit));
        params.addQueryItem("offset", QString("%1").arg(offset));

        network->request_forQobuz(HttpRequestType::Search_GetList_MostPopular
                                          , url
                                          , NetworkHttp::HeaderOption_forQobuz::Qobuz_X_Session
                                          , params
                                          , NetworkHttp::RequestMethod::Get
                                          , false);
    }


    // MARK : Http 요청 결과 처리 -------------------------------------------------------------------------------------------

    /**
     * @brief [슬롯] Http 요청 결과 처리. p_id에 따라 분기 처리 진행된다.
     * @details
     *      @li 로그인 처리 : @see setResult_loginQobuz
     *      @li 로그아웃 처리 : @see setResult_logoutQobuz
     * @param p_id 요청 아이디
     * @param p_jsonObject HTTP 응답 결과
     */
    void ProcCommon::slot_responseHttp(int p_id, QJsonObject p_jsonObj){

        //qDebug() << "[QOBUZ][MSG][FUNC] : ProcCommon_forQobuz.cpp -> ProcCommon::slot_responseHttp(int p_id, QJsonObject p_jsonObj)";
        //print_qobuz_func();

        //if(p_id == Get_AlbumInfo){
            //QJsonDocument doc(p_jsonObj);
            //QString strJson(doc.toJson(QJsonDocument::Compact));
           // qDebug() << "p_id : " << p_id << "\nProcCommon::slot_responseHttp----p_jsonObj : " << strJson << "\n";                             //cheon210717-hiresjson
        //}

        NetworkHttp* sender_http = qobject_cast<NetworkHttp*>(sender());

        switch (p_id) {
            case Login:  setResult_loginQobuz(p_jsonObj);    break;

            case Logout: setResult_logoutQobuz(p_jsonObj);   break;

            case GetList_Genres:   setResult_listGenres(p_jsonObj);    break;

            case GetList_Moods:    setResult_listMoods(p_jsonObj);     break;

            case HttpRequestType::GetList_Playlists :   setResult_list_playlists(p_jsonObj);        break;
            case HttpRequestType::GetList_Albums :      setResult_list_albums(p_jsonObj);           break;
            case HttpRequestType::GetList_Artists :     setResult_list_artists(p_jsonObj);          break;
            case HttpRequestType::GetList_Tracks :      setResult_list_tracks(p_jsonObj);           break;
            case HttpRequestType::GetList_Appearson :   setResult_list_appearson(p_jsonObj);        break;
            case HttpRequestType::GetList_LastRelease:  setResult_list_lastrelease(p_jsonObj);      break;
            case HttpRequestType::GetList_Videos :      setResult_list_videos(p_jsonObj);           break;

            case HttpRequestType::Get_AlbumInfo :       setResult_album(p_jsonObj);                 break;
            case HttpRequestType::Get_ArtistInfo :      setResult_artist(p_jsonObj);                break;

            case HttpRequestType::Get_ArtistBioText :   setResult_artist_bioText(p_jsonObj);        break;


            case HttpRequestType::GetList_My_Favorite_Playlists :       setResult_list_myFavoritePlaylists(p_jsonObj);      break;
            case HttpRequestType::GetList_My_Favorite_Albums :          setResult_list_myFavoriteAlbums(p_jsonObj);         break;
            case HttpRequestType::GetList_My_Favorite_Tracks :          setResult_list_myFavoriteTracks(p_jsonObj);         break;
            case HttpRequestType::GetList_My_Favorite_Artists :         setResult_list_myFavoriteArtists(p_jsonObj);        break;
            case HttpRequestType::GetList_My_Favorite_Videos :          setResult_list_myFavoriteVideos(p_jsonObj);         break;

            case HttpRequestType::GetList_Items_Playlist :              setResult_list_itmes_of_playlist(p_jsonObj);        break;
            case HttpRequestType::GetList_Items_Album :                 setResult_list_itmes_of_album(p_jsonObj);           break;

            case HttpRequestType::GetAll_UserFavorites :                setResult_listAll_myFavoritesIds(p_jsonObj);        break;


            case HttpRequestType::GetList_My_Purchases_Albums :         setResult_list_myPurchasesAlbums(p_jsonObj);        break;
            case HttpRequestType::GetList_My_Purchases_Tracks :         setResult_list_myPurchasesTracks(p_jsonObj);        break;

            case HttpRequestType::Favorite_Add_Album :
            case HttpRequestType::Favorite_Add_Artist :
            case HttpRequestType::Favorite_Add_Track :
            case HttpRequestType::Favorite_Add_Playlist :
            case HttpRequestType::Favorite_Add_Video :

            case HttpRequestType::Favorite_Delete_Album :
            case HttpRequestType::Favorite_Delete_Artist :
            case HttpRequestType::Favorite_Delete_Track :
            case HttpRequestType::Favorite_Delete_Playlist :
            case HttpRequestType::Favorite_Delete_Video :
                break;

            case HttpRequestType::GetList_My_Created_Playlists :
                this->setResult_list_myCreatedPlaylists(p_jsonObj, sender_http->get_userOptJson());
                break;

            case HttpRequestType::UserCreatePlaylist_Delete :
                this->setResult_delete_myCreatedplaylist(p_jsonObj, sender_http->get_userOptJson());
                break;

            case HttpRequestType::UserCreatePlaylist_Add_Tracks :
                this->setResult_add_items_in_myPlaylist(p_jsonObj);
                break;

            case HttpRequestType::GetList_TrackIds_Album :
                this->setResult_list_trackId_of_album(p_jsonObj, sender_http->get_userOptJson());
                break;

            case HttpRequestType::GetList_TrackIds_Playlist :
                this->setResult_list_trackId_of_playlist(p_jsonObj, sender_http->get_userOptJson());
                break;

            case HttpRequestType::DeleteItems_in_MyCreatedPlaylist :
                this->setResult_delete_items_in_myPlaylist(p_jsonObj);
                break;

            case HttpRequestType::Create_userPlaylist :
                this->setResult_create_myPlaylist(p_jsonObj);
                break;

            case HttpRequestType::AddTracks_userPlaylist :
                this->setResult_addTracks_myPlaylsit(p_jsonObj);
                break;


            case HttpRequestType::Search_GetList_MostPopular :  this->setResult_search_getList_mostPopular(p_jsonObj);  break;
            case HttpRequestType::Search_GetList_Track :        this->setResult_search_getList_track(p_jsonObj);        break;
            case HttpRequestType::Search_GetList_Artist :       this->setResult_search_getList_artist(p_jsonObj);       break;
            case HttpRequestType::Search_GetList_Album :        this->setResult_search_getList_album(p_jsonObj);        break;
            case HttpRequestType::Search_GetList_Playlist :     this->setResult_search_getList_playlist(p_jsonObj);     break;
            case HttpRequestType::Search_GetList_Video :        this->setResult_search_getList_video(p_jsonObj);        break;


        }

        sender()->deleteLater();


    }


    /**
     * @brief Qobuz 로그인 요청결과 처리 함수
     * @param p_jsonData QJsonObject API response
     * @see https://documenter.getpostman.com/view/448610/HXVi?version=latest#11d1026f-3789-480c-aa99-57dae2a357b1
     */
    void ProcCommon::setResult_loginQobuz(const QJsonObject &p_jsonObj){

        //qDebug() << "[QOBUZ][MSG][FUNC] : ProcCommon_forQobuz.cpp -> ProcCommon::setResult_loginQobuz(const QJsonObject &p_jsonObj)";
        //print_qobuz_func();
        //QJsonDocument doc(p_jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "p_jsonObj = " << strJson << "\n";

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            qobuz::RoseSessionInfo_forQobuz data_output;
            data_output.user_auth_token = ProcJsonEasy::getString(p_jsonObj, "user_auth_token");

            QJsonObject user_Obj = ProcJsonEasy::getJsonObject(p_jsonObj, "user");
            data_output.email = ProcJsonEasy::getString(user_Obj, "email");
            data_output.user_id = QString("%1").arg(ProcJsonEasy::getInt(user_Obj, "id"));
            QJsonObject device_Obj = ProcJsonEasy::getJsonObject(user_Obj, "device");
            data_output.device_man_id = ProcJsonEasy::getString(device_Obj, "device_manufacturer_id");
            data_output.device_id = QString("%1").arg(ProcJsonEasy::getInt(device_Obj, "id"));
            QJsonObject credential_Obj = ProcJsonEasy::getJsonObject(user_Obj, "credential");
            data_output.description = ProcJsonEasy::getString(credential_Obj, "description");
            data_output.c_id = QString("%1").arg(ProcJsonEasy::getInt(credential_Obj, "id"));
            data_output.label = ProcJsonEasy::getString(credential_Obj, "label");
            QJsonObject parameters_Obj = ProcJsonEasy::getJsonObject(credential_Obj, "parameters");
            data_output.hfp_purchases = ProcJsonEasy::getBool(parameters_Obj, "hfp_purchase");
            data_output.hires_purchases = ProcJsonEasy::getBool(parameters_Obj, "hires_purchases_streaming");
            data_output.hires_streaming = ProcJsonEasy::getBool(parameters_Obj, "hires_streaming");
            data_output.lossless_streaming = ProcJsonEasy::getBool(parameters_Obj, "lossless_streaming");
            data_output.lossy_streaming = ProcJsonEasy::getBool(parameters_Obj, "lossy_streaming");
            data_output.mobile_streaming = ProcJsonEasy::getBool(parameters_Obj, "mobile_streaming");
            data_output.offline_streaming = ProcJsonEasy::getBool(parameters_Obj, "offline_streaming");

            userLoginInfo.auth_token = data_output.user_auth_token;
            userLoginInfo.user_id = data_output.user_id;

            if(data_output.email.isEmpty()){
                emit this->failedLogin(tr("Unable to login to QOBUZ service. (2)"));
                //emit this->failedLogin(tr("QOBUZ 서비스에 로그인할 수 없습니다. (2)"));
            }
            else{
                // 정보 저장 (어플 사용하는 동안 필요한 정보임)
                global.user_forQobuz.setLogin(data_output.user_auth_token, data_output.email, data_output.user_id, userLoginInfo.flagSavedLoginInfo);
                //global.user_forQobuz.setUsername(userLoginInfo.username);

                // DB 정보 처리
                if(userLoginInfo.flagSavedLoginInfo){
                    this->saveLoginInfo_qobuzDB(userLoginInfo);
                }
                else{
                    this->clearLoginInfo_qobuzDB();
                }

                //login 정보 rose로 전달
                ProcRosePlay_withQobuz *procRose = new ProcRosePlay_withQobuz();
                procRose->request_set_session_info(data_output);

                // 시그널 발생
                emit this->successLogin();
            }

        }
        else{
            emit this->failedLogin(tr("Unable to login to QOBUZ service. (1)"));
            //emit this->failedLogin(tr("QOBUZ 서비스에 로그인할 수 없습니다. (1)"));
        }
    }


    /**
     * @brief 타이달 로그아웃 요청결과 처리 함수
     * @param p_jsonObj
     * @see https://documenter.getpostman.com/view/448610/HXVi?version=latest#98e2e430-ce2e-455e-a171-e4552195c822
     */
    void ProcCommon::setResult_logoutQobuz(const QJsonObject &p_jsonObj){
        Q_UNUSED(p_jsonObj);

        global.user_forQobuz.setLogout();
        this->userLoginInfo = UserLoginInfo{};
        this->clearLoginInfo_qobuzDB();

        emit this->successLogout();
    }


    /**
     * @brief 장르 종류 결과처리
     * @param p_jsonObject
     */
    void ProcCommon::setResult_listGenres(const QJsonObject &p_jsonObj){
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && list_genre.isEmpty()){
            QJsonObject jsonPar_genre = ProcJsonEasy::getJsonObject(p_jsonObj, "genres");
            QJsonArray jsonArr_list = ProcJsonEasy::getJsonArray(jsonPar_genre, "items");

            qobuz::GenreInfo tmp_data;
            tmp_data.id = 0;
            tmp_data.name = "All genres";
            tmp_data.path = "path";
            list_genre.append(tmp_data);

            for(int i = 0; i < jsonArr_list.count(); i++){
                QJsonObject tmp_jsonObj = jsonArr_list.at(i).toObject();
                int tmp_id = ProcJsonEasy::getInt(tmp_jsonObj, "id");
                QString tmp_name = ProcJsonEasy::getString(tmp_jsonObj, "name");
                QString tmp_path = ProcJsonEasy::getString(tmp_jsonObj, "slug");

                if(!tmp_name.isEmpty() && tmp_id != 0){
                    //qobuz::GenreInfo tmp_data;
                    tmp_data.id = tmp_id;
                    tmp_data.name = tmp_name;
                    tmp_data.path = tmp_path;
                    list_genre.append(tmp_data);
                }
            }

            emit successLogin();
        }
    }



    /**
     * @brief Mood & Activity 종류 결과처리
     * @param p_jsonObject
     */
    void ProcCommon::setResult_listMoods(const QJsonObject &p_jsonObj){
        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray jsonArr_list = ProcJsonEasy::getJsonArray(p_jsonObj, "array");

            for(int i=0 ; i<jsonArr_list.count() ; i++){
                QJsonObject tmp_jsonObj = jsonArr_list.at(i).toObject();
                QString tmp_name = ProcJsonEasy::getString(tmp_jsonObj, "name");
                QString tmp_path = ProcJsonEasy::getString(tmp_jsonObj, "path");

                if(!tmp_name.isEmpty() && !tmp_path.isEmpty()){
                    qobuz::MoodInfo tmp_data;
                    tmp_data.name = tmp_name;
                    tmp_data.path = tmp_path;
                    list_mood.append(tmp_data);
                }
            }
        }
    }



    //----------------------------------------------------------------------


    /**
     * @brief more data에 대한 last page 체크 반환.
     * offset, limit, totalNumberOfItems 으로 flag_lastPage 계산함
     * @param p_jsonObj
     * @return
     */
    bool ProcCommon::get_flag_lastPage(const QJsonObject& p_jsonObj){

        //qDebug() << "[QOBUZ][MSG][FUNC] : ProcCommon_forQobuz.cpp -> ProcCommon::get_flag_lastPage(const QJsonObject& p_jsonObj)";
        //print_qobuz_func();

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << strJson << "\n";*/

        int offset = ProcJsonEasy::getInt(p_jsonObj, "offset");
        int limit = ProcJsonEasy::getInt(p_jsonObj, "limit");
        int totalNumberOfItems = ProcJsonEasy::getInt(p_jsonObj, "total");

        return (totalNumberOfItems > offset + limit) ? false : true;
    }



    /**
     * @brief ProcCommon::setResult_list_playlists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_playlists(const QJsonObject &p_jsonObj){

        QList<qobuz::PlaylistItemData> list_output;
        QJsonObject jsonPar_playlist = ProcJsonEasy::getJsonObject(p_jsonObj, "playlists");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_playlist, "items");

        //qDebug() << "[QOBUZ][MSG][FUNC] : ProcCommon_forQobuz.cpp -> ProcCommon::setResult_list_playlists(const QJsonObject &p_jsonObj)";
        //print_qobuz_func();
        //qDebug() << "ProcJsonEasy::get_flagOk(p_jsonObj) : " << ProcJsonEasy::get_flagOk(p_jsonObj) << ", jsonArr_item.count : " << jsonArr_item.count();

        bool flag_lastPage = true;      // default

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                PlaylistItemData tmp_data = ConvertData::make_playlistData_fromQobuzJsonObj(tmp_json);
                tmp_data.album_total_cnt = ProcJsonEasy::getInt(jsonPar_playlist, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonPar_playlist, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonPar_playlist, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonPar_playlist, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_playlists(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_albums
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_albums(const QJsonObject &p_jsonObj){

        QList<qobuz::AlbumItemData> list_output;
        QJsonObject jsonPar_album = ProcJsonEasy::getJsonObject(p_jsonObj, "albums");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_album, "items");

        bool flag_lastPage = true;

        //qDebug() << "[QOBUZ][MSG][FUNC] : ProcCommon_forQobuz.cpp -> ProcCommon::setResult_list_albums(const QJsonObject &p_jsonObj)";
        //print_qobuz_func();
        //qDebug() << "ProcJsonEasy::get_flagOk(p_jsonObj) : " << ProcJsonEasy::get_flagOk(p_jsonObj) << ", jsonArr_item.count : " << jsonArr_item.count();

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i = 0 ; i < jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                AlbumItemData tmp_data = ConvertData::make_albumData_fromQobuzJsonObj(tmp_json);
                tmp_data.album_total_cnt = ProcJsonEasy::getInt(jsonPar_album, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonPar_album, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonPar_album, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonPar_album, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_albums(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_artists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_artists(const QJsonObject &p_jsonObj){

        QList<qobuz::ArtistItemData> list_output;
        QJsonObject tmp_jonPar = ProcJsonEasy::getJsonObject(p_jsonObj, "artists");
        QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(tmp_jonPar, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = tmp_jsonArr.at(i).toObject();

                // 정보 담을 struct
                ArtistItemData tmp_data = ConvertData::make_artistData_fromQobuzJsonObj(tmp_json);
                tmp_data.artist_total_cnt = ProcJsonEasy::getInt(tmp_jonPar, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(tmp_jonPar, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(tmp_jonPar, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(tmp_jonPar, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_artists(list_output, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_tracks
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_tracks(const QJsonObject &p_jsonObj){

        QList<qobuz::TrackItemData> list_output;
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            if(jsonArr_item.count() > 0){
                for(int i = 0; i < jsonArr_item.count(); i++){
                    QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                    // 정보 담을 struct
                    qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);
                    list_output.append(tmp_data);
                }

                QJsonObject tmpObj = QJsonObject();
                tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
                tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
                tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "total"));

                flag_lastPage = this->get_flag_lastPage(tmpObj);
            }
            else{
                QJsonObject tmp_json = p_jsonObj;
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);
                list_output.append(tmp_data);
            }
        }

        emit this->completeReq_list_tracks(list_output, jsonArr_item, flag_lastPage);
    }



    /**
     * @brief ProcCommon::setResult_list_tracks
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_appearson(const QJsonObject &p_jsonObj){

        QList<qobuz::TrackItemData> list_output;
        QJsonObject jsonPar_appearson = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks_appears_on");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_appearson, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);
                tmp_data.appearson_count = ProcJsonEasy::getInt(jsonPar_appearson, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonPar_appearson, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonPar_appearson, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonPar_appearson, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        //emit this->completeReq_list_tracks(list_output, jsonArr_item, flag_lastPage);
        emit this->completeReq_list_appearson(list_output, jsonArr_item, flag_lastPage);

    }



    void ProcCommon::setResult_list_lastrelease(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            bool flag_lastPage = true;

            if(p_jsonObj.contains("album_last_release")){
                QList<qobuz::AlbumItemData> list_output;
                QJsonObject jsonPar_lastrelease = ProcJsonEasy::getJsonObject(p_jsonObj, "album_last_release");

                // 정보 담을 struct
                AlbumItemData tmp_data = ConvertData::make_albumData_fromQobuzJsonObj(jsonPar_lastrelease);
                list_output.append(tmp_data);

                QJsonArray jsonArr_add;
                jsonArr_add.append(jsonPar_lastrelease);

                emit this->completeReq_list_lastrelease(list_output, jsonArr_add, flag_lastPage);
            }
            else{
                QList<qobuz::AlbumItemData> list_output;
                QJsonArray jsonArr_add;

                emit this->completeReq_list_lastrelease(list_output, jsonArr_add, flag_lastPage);
            }

            if(p_jsonObj.contains("albums_without_last_release")){
                QList<qobuz::AlbumItemData> list_output;
                QJsonObject jsonPar_album = ProcJsonEasy::getJsonObject(p_jsonObj, "albums_without_last_release");
                QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_album, "items");

                for(int i = 0; i < jsonArr_item.count(); i++){
                    QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                    // 정보 담을 struct
                    AlbumItemData tmp_data = ConvertData::make_albumData_fromQobuzJsonObj(tmp_json);
                    tmp_data.album_total_cnt = ProcJsonEasy::getInt(jsonPar_album, "total");
                    list_output.append(tmp_data);
                }

                QJsonObject tmpObj = QJsonObject();
                tmpObj.insert("limit", ProcJsonEasy::getInt(jsonPar_album, "limit"));
                tmpObj.insert("offset", ProcJsonEasy::getInt(jsonPar_album, "offset"));
                tmpObj.insert("total", ProcJsonEasy::getInt(jsonPar_album, "total"));

                flag_lastPage = this->get_flag_lastPage(tmpObj);

                emit this->completeReq_list_albums(list_output, jsonArr_item, flag_lastPage);
            }
            else{
                QList<qobuz::AlbumItemData> list_output;
                QJsonArray jsonArr_item;

                emit this->completeReq_list_albums(list_output, jsonArr_item, flag_lastPage);
            }

            if(p_jsonObj.contains("playlists")){
                QList<qobuz::TrackItemData> list_output;
                QJsonArray jsonArr_playlist = ProcJsonEasy::getJsonArray(p_jsonObj, "playlists");
                QJsonObject tmp_playlist = jsonArr_playlist.at(0).toObject();
                QJsonObject tmp_tracks = ProcJsonEasy::getJsonObject(tmp_playlist, "tracks");
                QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(tmp_tracks, "items");

                for(int i = 0; i < jsonArr_item.count(); i++){
                    QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                    // 정보 담을 struct
                    TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);
                    list_output.append(tmp_data);
                }

                emit this->completeReq_list_tracks(list_output, jsonArr_item, flag_lastPage);
            }
            else{
                QList<qobuz::TrackItemData> list_output;
                QJsonArray jsonArr_item;

                emit this->completeReq_list_tracks(list_output, jsonArr_item, flag_lastPage);
            }
        }
    }



    /**
     * @brief ProcCommon::setResult_list_videos
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_videos(const QJsonObject &p_jsonObj){

        QList<qobuz::VideoItemData> list_output;
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::VideoItemData tmp_data = ConvertData::make_videoData_fromQobuzJsonObj(tmp_json);
                list_output.append(tmp_data);
            }

            flag_lastPage = this->get_flag_lastPage(p_jsonObj);
        }

        emit this->completeReq_list_videos(list_output, flag_lastPage);
    }



    /**
     * @brief ProcCommon::setResult_album
     * @param p_jsonObj
     */
    void ProcCommon::setResult_album(const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << strJson << "\n";*/

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            AlbumItemData data_album = ConvertData::make_albumData_fromQobuzJsonObj(p_jsonObj);

            emit this->completeReq_album(data_album);

            if(p_jsonObj.contains("tracks")){
                QList<qobuz::TrackItemData> list_output;
                QJsonArray send_tracks;

                QJsonObject jsonPar_album = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
                QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_album, "items");
                bool flag_lastPage = true;

                // Added Jeon 30/12/2020 jsonArr_item 재구성
                for(int i = 0; i < jsonArr_item.count(); i++){
                    QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                    // 정보 담을 struct
                    qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);

                    // for album
                    tmp_data.album_duration = ProcJsonEasy::getInt(p_jsonObj, "duration");
                    tmp_data.album_hires = ProcJsonEasy::getBool(p_jsonObj, "hires");
                    tmp_data.album_hires_streamable = ProcJsonEasy::getBool(p_jsonObj, "hires_streamable");
                    tmp_data.album_id = ProcJsonEasy::getString(p_jsonObj, "id");

                    QJsonObject jsonPar_image = ProcJsonEasy::getJsonObject(p_jsonObj, "image");
                    tmp_data.album_image_large = ProcJsonEasy::getString(jsonPar_image, "large");
                    tmp_data.album_image_small = ProcJsonEasy::getString(jsonPar_image, "small");
                    tmp_data.album_image_thumbnail = ProcJsonEasy::getString(jsonPar_image, "thumbnail");
                    tmp_data.album_streamable = ProcJsonEasy::getBool(p_jsonObj, "streamable");
                    tmp_data.album_title = ProcJsonEasy::getString(p_jsonObj, "title");
                    tmp_data.album_version = ProcJsonEasy::getString(p_jsonObj, "version");
                    tmp_data.album_tracks_count = ProcJsonEasy::getInt(p_jsonObj, "tracks_count");

                    /*QJsonObject jsonPar_comporser = ProcJsonEasy::getJsonObject(p_jsonObj, "composer");
                    tmp_data.composer_id = ProcJsonEasy::getInt(jsonPar_comporser, "id");
                    tmp_data.composer_name = ProcJsonEasy::getString(jsonPar_comporser, "name");*/

                    tmp_data.qobuz_id = ProcJsonEasy::getInt(p_jsonObj, "qobuz_id");

                    // for artist
                    if(tmp_data.list_artist_albums_count.empty())
                    {
                        QJsonObject jsonPar_artist = ProcJsonEasy::getJsonObject(p_jsonObj, "artist");
                        tmp_data.list_artist_albums_count.append(ProcJsonEasy::getInt(jsonPar_artist, "albums_count"));
                        tmp_data.list_artist_id.append(ProcJsonEasy::getInt(jsonPar_artist, "id"));
                        tmp_data.list_artist_name.append(ProcJsonEasy::getString(jsonPar_artist, "name"));
                    }

                    list_output.append(tmp_data);

                    QJsonObject track;
                    QJsonObject album;
                    album.insert("duration", tmp_data.album_duration);
                    album.insert("hires", tmp_data.album_hires);
                    album.insert("hires_streamable", tmp_data.album_hires_streamable);
                    album.insert("id", tmp_data.album_id);
                    QJsonObject image;
                    image.insert("large", tmp_data.album_image_large);
                    image.insert("small", tmp_data.album_image_small);
                    image.insert("thumbnail", tmp_data.album_image_thumbnail);
                    album.insert("image", image);
                    album.insert("streamable", tmp_data.album_streamable);
                    album.insert("title", tmp_data.album_title);
                    album.insert("tracks_count", tmp_data.album_tracks_count);

                    QJsonObject artist;
                    QJsonArray jsonArr_artist_albums_count;
                    foreach(int tmp_id, tmp_data.list_artist_albums_count){
                        jsonArr_artist_albums_count.append(tmp_id);
                    }
                    artist.insert("list_artist_albums_count", jsonArr_artist_albums_count);

                    QJsonArray jsonArr_id;
                    foreach(int tmp_id, tmp_data.list_artist_id){
                        jsonArr_id.append(tmp_id);
                    }
                    artist.insert("list_artist_id", jsonArr_id);

                    QJsonArray jsonArr_name;
                    foreach(QString tmp_name, tmp_data.list_artist_name){
                        jsonArr_name.append(tmp_name);
                    }
                    artist.insert("list_artist_name", jsonArr_name);

                    QJsonObject composer;
                    composer.insert("id", tmp_data.composer_id);
                    composer.insert("name", tmp_data.composer_name);

                    QJsonObject performer;
                    performer.insert("id", tmp_data.performer_id);
                    performer.insert("name", tmp_data.performer_name);

                    track.insert("album", album);
                    track.insert("artist", artist);
                    track.insert("composer", composer);
                    track.insert("copyright", tmp_data.copyright);
                    track.insert("duration", tmp_data.duration);
                    track.insert("hires", tmp_data.hires);
                    track.insert("hires_streamable", tmp_data.hires_streamable);
                    track.insert("id", tmp_data.id);
                    track.insert("maximum_bit_depth", tmp_data.maximum_bit_depth);
                    track.insert("maximum_sampling_rate", tmp_data.maximum_sampling_rate);
                    track.insert("media_number", tmp_data.media_number);
                    track.insert("performer", performer);
                    track.insert("performers", tmp_data.performers);
                    track.insert("previewable", tmp_data.previewable);
                    track.insert("purchasable", tmp_data.purchasable);
                    track.insert("qobuz_id", tmp_data.qobuz_id);
                    track.insert("sampleable", tmp_data.sampleable);
                    track.insert("streamable", tmp_data.streamable);
                    track.insert("title", tmp_data.title);
                    track.insert("work", tmp_data.work);
                    track.insert("track_number", tmp_data.track_number);

                    send_tracks.append(track);
                }

                QJsonObject tmpObj = QJsonObject();
                tmpObj.insert("limit", ProcJsonEasy::getInt(jsonPar_album, "limit"));
                tmpObj.insert("offset", ProcJsonEasy::getInt(jsonPar_album, "offset"));
                tmpObj.insert("total", ProcJsonEasy::getInt(jsonPar_album, "total"));

                flag_lastPage = this->get_flag_lastPage(tmpObj);

                emit this->completeReq_list_items_of_album(list_output, send_tracks, flag_lastPage);
            }
            else{
                QList<qobuz::TrackItemData> list_output;
                QJsonArray send_tracks;
                bool flag_lastPage = true;

                emit this->completeReq_list_items_of_album(list_output, send_tracks, flag_lastPage);
            }

            if(p_jsonObj.contains("albums_same_artist")){
                QList<qobuz::AlbumItemData> list_output;
                QJsonObject jsonPar_album = ProcJsonEasy::getJsonObject(p_jsonObj, "albums_same_artist");
                QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_album, "items");

                bool flag_lastPage = true;

                if(ProcJsonEasy::get_flagOk(p_jsonObj)){

                    for(int i = 0 ; i < jsonArr_item.count() ; i++){
                        QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                        // 정보 담을 struct
                        AlbumItemData tmp_data = ConvertData::make_albumData_fromQobuzJsonObj(tmp_json);
                        tmp_data.album_total_cnt = jsonArr_item.count();
                        list_output.append(tmp_data);
                    }

                    QJsonObject tmpObj = QJsonObject();
                    tmpObj.insert("limit", jsonArr_item.count());
                    tmpObj.insert("offset", 0);
                    tmpObj.insert("total", jsonArr_item.count());

                    flag_lastPage = this->get_flag_lastPage(tmpObj);
                }

                emit this->completeReq_list_albums(list_output, jsonArr_item, flag_lastPage);
            }
        }
        else{

            AlbumItemData data_album;
            data_album.title = "error";

            emit this->completeReq_album(data_album);

            QList<qobuz::TrackItemData> list_track;
            QList<qobuz::AlbumItemData> list_album;
            QJsonArray jsonArr_item;
            bool flag_lastPage = true;

            emit this->completeReq_list_items_of_album(list_track, jsonArr_item, flag_lastPage);
            emit this->completeReq_list_albums(list_album, jsonArr_item, flag_lastPage);
        }
    }


    /**
     * @brief ProcCommon::setResult_artist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_artist(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){            
            ArtistItemData data_artist = ConvertData::make_artistData_fromQobuzJsonObj(p_jsonObj);
            emit this->completeReq_artist(data_artist);
        }
        else{
            ArtistItemData data_artist;
            emit this->completeReq_artist(data_artist);
        }
    }


    /**
     * @brief Artist의 신상정보를 text로 반환한다.
     * @param p_jsonObj
     */
    void ProcCommon::setResult_artist_bioText(const QJsonObject &p_jsonObj){
        QString bioText = "";
        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            bioText = ProcJsonEasy::getString(p_jsonObj, "text");
        }
        emit this->completeReq_artist_bioText(bioText);
    }


    /**
     * @brief ProcCommon::setResult_list_itmes_of_playlist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_itmes_of_playlist(const QJsonObject &p_jsonObj){        

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QList<qobuz::PlaylistItemData> plist_output;
            QList<qobuz::TrackItemData> list_output;

            QJsonObject jsonPar_playlist = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_playlist, "items");
            bool flag_lastPage = true;

            // 정보 담을 struct
            PlaylistItemData tmp_data = ConvertData::make_playlistData_fromQobuzJsonObj(p_jsonObj);
            plist_output.append(tmp_data);

            emit this->completeReq_playlist(tmp_data);

            for(int i = 0; i < jsonArr_item.count(); i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonPar_playlist, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonPar_playlist, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonPar_playlist, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);

            emit this->completeReq_list_items_of_playlist(list_output, jsonArr_item, flag_lastPage);

            if(p_jsonObj.contains("similarPlaylist")){
                QList<qobuz::PlaylistItemData> list_output;
                QJsonObject jsonPar_playlist = ProcJsonEasy::getJsonObject(p_jsonObj, "similarPlaylist");
                QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_playlist, "items");
                bool flag_lastPage = false;

                for(int i = 0; i < jsonArr_item.count(); i++){
                    QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                    // 정보 담을 struct
                    PlaylistItemData tmp_data = ConvertData::make_playlistData_fromQobuzJsonObj(tmp_json);
                    tmp_data.album_total_cnt = jsonArr_item.count();
                    list_output.append(tmp_data);
                }

                emit this->completeReq_list_playlists(list_output, jsonArr_item, flag_lastPage);
            }
            else{
                emit this->completeReq_list_playlists(plist_output, jsonArr_item, flag_lastPage);
            }
        }
        else{
            PlaylistItemData tmp_data;
            QList<qobuz::TrackItemData> list_output;
            QList<qobuz::PlaylistItemData> plist_output;
            QJsonArray jsonArr_item;
            bool flag_lastPage = false;

            emit this->completeReq_playlist(tmp_data);
            emit this->completeReq_list_items_of_playlist(list_output, jsonArr_item, flag_lastPage);
            emit this->completeReq_list_playlists(plist_output, jsonArr_item, flag_lastPage);
        }
    }


    /**
     * @brief
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_itmes_of_album(const QJsonObject &p_jsonObj){

        QList<qobuz::TrackItemData> list_output;
        QJsonArray send_tracks;

        QJsonObject jsonPar_album = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonPar_album, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            // Added Jeon 30/12/2020 jsonArr_item 재구성
            for(int i = 0; i < jsonArr_item.count(); i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                //qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);
                qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);

                // for album
                tmp_data.album_duration = ProcJsonEasy::getInt(p_jsonObj, "duration");
                tmp_data.album_hires = ProcJsonEasy::getBool(p_jsonObj, "hires");
                tmp_data.album_hires_streamable = ProcJsonEasy::getBool(p_jsonObj, "hires_streamable");
                tmp_data.album_id = ProcJsonEasy::getString(p_jsonObj, "id");

                QJsonObject jsonPar_image = ProcJsonEasy::getJsonObject(p_jsonObj, "image");
                tmp_data.album_image_large = ProcJsonEasy::getString(jsonPar_image, "large");
                tmp_data.album_image_small = ProcJsonEasy::getString(jsonPar_image, "small");
                tmp_data.album_image_thumbnail = ProcJsonEasy::getString(jsonPar_image, "thumbnail");
                tmp_data.album_streamable = ProcJsonEasy::getBool(p_jsonObj, "streamable");
                tmp_data.album_title = ProcJsonEasy::getString(p_jsonObj, "title");
                tmp_data.album_tracks_count = ProcJsonEasy::getInt(p_jsonObj, "tracks_count");

                QJsonObject jsonPar_comporser = ProcJsonEasy::getJsonObject(p_jsonObj, "composer");
                tmp_data.composer_id = ProcJsonEasy::getInt(jsonPar_comporser, "id");
                tmp_data.composer_name = ProcJsonEasy::getString(jsonPar_comporser, "name");

                tmp_data.qobuz_id = ProcJsonEasy::getInt(p_jsonObj, "qobuz_id");

                // for artist
                if(tmp_data.list_artist_albums_count.empty())
                {
                    QJsonObject jsonPar_artist = ProcJsonEasy::getJsonObject(p_jsonObj, "artist");
                    tmp_data.list_artist_albums_count.append(ProcJsonEasy::getInt(jsonPar_artist, "albums_count"));
                    tmp_data.list_artist_id.append(ProcJsonEasy::getInt(jsonPar_artist, "id"));
                    tmp_data.list_artist_name.append(ProcJsonEasy::getString(jsonPar_artist, "name"));
                }

                list_output.append(tmp_data);

                QJsonObject image;
                image.insert("large", tmp_data.album_image_large);
                image.insert("small", tmp_data.album_image_small);
                image.insert("thumbnail", tmp_data.album_image_thumbnail);

                QJsonObject album;
                album.insert("duration", tmp_data.album_duration);
                album.insert("hires", tmp_data.album_hires);
                album.insert("hires_streamable", tmp_data.album_hires_streamable);
                album.insert("id", tmp_data.album_id);
                album.insert("image", image);
                album.insert("streamable", tmp_data.album_streamable);
                album.insert("title", tmp_data.album_title);
                album.insert("tracks_count", tmp_data.album_tracks_count);

                QJsonObject artist;
                QJsonArray jsonArr_artist_albums_count;
                foreach(int tmp_id, tmp_data.list_artist_albums_count){
                    jsonArr_artist_albums_count.append(tmp_id);
                }
                artist.insert("list_artist_albums_count", jsonArr_artist_albums_count);

                QJsonArray jsonArr_id;
                foreach(int tmp_id, tmp_data.list_artist_id){
                    jsonArr_id.append(tmp_id);
                }
                artist.insert("list_artist_id", jsonArr_id);

                QJsonArray jsonArr_name;
                foreach(QString tmp_name, tmp_data.list_artist_name){
                    jsonArr_name.append(tmp_name);
                }
                artist.insert("list_artist_name", jsonArr_name);

                QJsonObject composer;
                composer.insert("id", tmp_data.composer_id);
                composer.insert("name", tmp_data.composer_name);

                QJsonObject performer;
                performer.insert("id", tmp_data.performer_id);
                performer.insert("name", tmp_data.performer_name);

                QJsonObject track;
                track.insert("album", album);
                track.insert("artist", artist);
                track.insert("composer", composer);
                track.insert("copyright", list_output.at(i).copyright);
                track.insert("duration", list_output.at(i).duration);
                track.insert("hires", list_output.at(i).hires);
                track.insert("hires_streamable", list_output.at(i).hires_streamable);
                track.insert("id", list_output.at(i).id);
                track.insert("maximum_bit_depth", list_output.at(i).maximum_bit_depth);
                track.insert("maximum_sampling_rate", list_output.at(i).maximum_sampling_rate);
                track.insert("media_number", list_output.at(i).media_number);
                track.insert("performer", performer);
                track.insert("performers", list_output.at(i).performers);
                track.insert("previewable", list_output.at(i).previewable);
                track.insert("purchasable", list_output.at(i).purchasable);
                track.insert("qobuz_id", list_output.at(i).qobuz_id);
                track.insert("sampleable", list_output.at(i).sampleable);
                track.insert("streamable", list_output.at(i).streamable);
                track.insert("title", list_output.at(i).title);
                if(!list_output.at(i).work.isEmpty()){
                    track.insert("work", list_output.at(i).work);//cheon210812-work
                }
                if(!list_output.at(i).version.isEmpty()){
                    track.insert("version", list_output.at(i).version);//cheon210812-work
                }
                track.insert("track_number", list_output.at(i).track_number);

                send_tracks.append(track);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonPar_album, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonPar_album, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonPar_album, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_items_of_album(list_output, send_tracks, flag_lastPage);
    }




    // about user's created playlist ------------------------------------------------------------------------------------------

    /**
     * @brief ProcCommon::setResult_list_myPlaylists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myCreatedPlaylists(const QJsonObject &p_jsonObj, QJsonObject jsonOb_opt){

        QList<qobuz::PlaylistItemData> list_output;
        QJsonObject json_par = ProcJsonEasy::getJsonObject(p_jsonObj, "playlists");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(json_par, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                PlaylistItemData tmp_data = ConvertData::make_playlistData_fromQobuzJsonObj(tmp_json);
                if(tmp_data.ownerName.contains("Qobuz")){
                    tmp_data.flagCreatedByUser = false;
                }
                else{
                    tmp_data.flagCreatedByUser = true;  // 사용자가 만든 playlist
                }
                tmp_data.ownerName = "";

                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(json_par, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(json_par, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(json_par, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myCreatedPlaylists(list_output, jsonArr_item, flag_lastPage, jsonOb_opt);
    }



    /**
     * @brief ProcCommon::setResult_delete_myPlaylist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_delete_myCreatedplaylist(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt){
        Q_UNUSED(p_jsonObj);
        Q_UNUSED(p_jsonObj_opt);
    }



    /**
     * @brief ProcCommon::setResult_add_items_in_myPlaylist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_add_items_in_myPlaylist(const QJsonObject& p_jsonObj){
        Q_UNUSED(p_jsonObj);
        emit this->completeReq_add_items_in_myPlaylist();
    }


    /**
     * @brief Album 의 트랙목록에 대해서, track_id 들을 추출해서 signal 발생
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_trackId_of_album(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt){

        QString playlist_uuid_target = ProcJsonEasy::getString(p_jsonObj_opt, "playlist_uuid_target");
        QList<QString> list_track_id;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();
                list_track_id.append(QString("%1").arg(ProcJsonEasy::getInt(tmp_json, "id")));
            }
        }

        if(!playlist_uuid_target.isEmpty() && list_track_id.count()>0){
            emit this->completeReq_list_trackIds_toAddPlaylist(playlist_uuid_target, list_track_id.join(","));
        }
        else{
            emit this->completeReq_list_trackIds_toAddPlaylist("", "");
        }

    }


    /**
     * @brief Album 의 트랙목록에 대해서, track_id 들을 추출해서 signal 발생
     * @param p_jsonObj
     * @param p_jsonObj_opt
     */
    void ProcCommon::setResult_list_trackId_of_playlist(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt){

        QString playlist_uuid_target = ProcJsonEasy::getString(p_jsonObj_opt, "playlist_uuid_target");
        QList<QString> list_track_id;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(jsonArr_item.at(i).toObject(), "item");
                list_track_id.append(QString("%1").arg(ProcJsonEasy::getInt(tmp_json, "id")));
            }
        }

        if(!playlist_uuid_target.isEmpty() && list_track_id.count()>0){
            emit this->completeReq_list_trackIds_toAddPlaylist(playlist_uuid_target, list_track_id.join(","));
        }
        else{
            emit this->completeReq_list_trackIds_toAddPlaylist("", "");
        }
    }



    /**
     * @brief ProcCommon::setResult_delete_items_in_myPlaylist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_delete_items_in_myPlaylist(const QJsonObject &p_jsonObj){
        Q_UNUSED(p_jsonObj);
        emit this->completeReq_delete_items_in_myPlaylist();
    }


    void ProcCommon::setResult_create_myPlaylist(const QJsonObject &p_jsonObj){

        int playlist_id = 0;
        if(p_jsonObj.contains("id")){
            playlist_id = ProcJsonEasy::getInt(p_jsonObj, "id");
        }

        emit this->completeReq_create_myPlaylist(playlist_id);
    }


    void ProcCommon::setResult_addTracks_myPlaylsit(const QJsonObject &p_jsonObj){

        int tracks_count = 0;
        if(p_jsonObj.contains("tracks_count")){
            tracks_count = ProcJsonEasy::getInt(p_jsonObj, "tracks_count");
        }

        emit this->completeReq_addTracks_myPlaylist(tracks_count);
    }



    // about my favorite collection ------------------------------------------------------------------------------------------

    /**
     * @brief ProcCommon::setResult_list_myFavoritePlaylists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myFavoritePlaylists(const QJsonObject &p_jsonObj){

        QList<qobuz::PlaylistItemData> list_output;
        QJsonArray jsonArr_item;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(tmp_jsonArr.at(i).toObject(), "item");
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                PlaylistItemData tmp_data = ConvertData::make_playlistData_fromQobuzJsonObj(tmp_json);
                tmp_data.album_total_cnt = ProcJsonEasy::getInt(p_jsonObj, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myFavoritePlaylists(list_output, jsonArr_item, flag_lastPage);
    }



    /**
     * @brief ProcCommon::setResult_list_myAlbums
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myFavoriteAlbums(const QJsonObject &p_jsonObj){

        QList<qobuz::AlbumItemData> list_output;
        QJsonArray jsonArr_item;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonObject par_json = ProcJsonEasy::getJsonObject(p_jsonObj, "albums");
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(par_json, "items");

            for(int i = 0; i < tmp_jsonArr.count(); i++){
                QJsonObject tmp_json = tmp_jsonArr.at(i).toObject();
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                AlbumItemData tmp_data = ConvertData::make_albumData_fromQobuzJsonObj(tmp_json);
                tmp_data.album_total_cnt = ProcJsonEasy::getInt(par_json, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(par_json, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(par_json, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(par_json, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myFavoriteAlbums(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_myTracks
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myFavoriteTracks(const QJsonObject &p_jsonObj){

        QList<qobuz::TrackItemData> list_output;
        QJsonArray jsonArr_item;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonObject par_json = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(par_json, "items");

            for(int i = 0; i < tmp_jsonArr.count(); i++){
                QJsonObject tmp_json = tmp_jsonArr.at(i).toObject();
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);
                tmp_data.appearson_count = ProcJsonEasy::getInt(par_json, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(par_json, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(par_json, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(par_json, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myFavoriteTracks(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_myFavoriteArtists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myFavoriteArtists(const QJsonObject &p_jsonObj){

        QList<qobuz::ArtistItemData> list_output;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonObject par_json = ProcJsonEasy::getJsonObject(p_jsonObj, "artists");
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(par_json, "items");

            for(int i = 0; i < tmp_jsonArr.count(); i++){
                QJsonObject tmp_json = tmp_jsonArr.at(i).toObject();

                // 정보 담을 struct
                ArtistItemData tmp_data = ConvertData::make_artistData_fromQobuzJsonObj(tmp_json);
                tmp_data.artist_total_cnt = ProcJsonEasy::getInt(par_json, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(par_json, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(par_json, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(par_json, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myFavoriteArtists(list_output, flag_lastPage);
    }



    /**
     * @brief ProcCommon::setResult_list_myFavoriteVideos
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myFavoriteVideos(const QJsonObject &p_jsonObj){

        QList<qobuz::VideoItemData> list_output;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(tmp_jsonArr.at(i).toObject(), "item");

                // 정보 담을 struct
                VideoItemData tmp_data = ConvertData::make_videoData_fromQobuzJsonObj(tmp_json);
                list_output.append(tmp_data);
            }

            flag_lastPage = this->get_flag_lastPage(p_jsonObj);
        }

        emit this->completeReq_list_myFavoriteVideos(list_output, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_listAll_myFavoritesIds
     * @param p_jsonObj
     */
    void ProcCommon::setResult_listAll_myFavoritesIds(const QJsonObject &p_jsonObj){

        emit this->completeReq_listAll_myFavoritesIds(p_jsonObj);
    }



    // about my favorite collection ------------------------------------------------------------------------------------------

    /**
     * @brief ProcCommon::setResult_list_myPurchasesAlbums
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myPurchasesAlbums(const QJsonObject &p_jsonObj){

        QList<qobuz::AlbumItemData> list_output;
        QJsonArray jsonArr_item;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonObject par_json = ProcJsonEasy::getJsonObject(p_jsonObj, "albums");
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(par_json, "items");

            for(int i = 0; i < tmp_jsonArr.count(); i++){
                QJsonObject tmp_json = tmp_jsonArr.at(i).toObject();
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                AlbumItemData tmp_data = ConvertData::make_albumData_fromQobuzJsonObj(tmp_json);
                tmp_data.album_total_cnt = ProcJsonEasy::getInt(par_json, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(par_json, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(par_json, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(par_json, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myPurchasesAlbums(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_myPurchasesTracks
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myPurchasesTracks(const QJsonObject &p_jsonObj){

        QList<qobuz::TrackItemData> list_output;
        QJsonArray jsonArr_item;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonObject par_json = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(par_json, "items");

            for(int i = 0; i < tmp_jsonArr.count(); i++){
                QJsonObject tmp_json = tmp_jsonArr.at(i).toObject();
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);
                tmp_data.appearson_count = ProcJsonEasy::getInt(par_json, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(par_json, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(par_json, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(par_json, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myPurchasesTracks(list_output, jsonArr_item, flag_lastPage);
    }



    // Searching ---------------------------------------------------------------------------------------------------------

    /**
     * @brief ProcCommon::setResult_search_getList_track
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_track(const QJsonObject &p_jsonObj){

        QList<qobuz::TrackItemData> list_output;
        QJsonObject jsonObj_track = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_track, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);
                tmp_data.appearson_count = ProcJsonEasy::getInt(jsonObj_track, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_track, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_track, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_track, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_tracks(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_search_getList_mostPopular
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_mostPopular(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            QJsonObject jsonObj_mostPopular = ProcJsonEasy::getJsonObject(p_jsonObj, "most_popular");
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_mostPopular, "items");

            QJsonObject tmpObj = QJsonObject();
            bool flag_lastPage = true;

            QJsonObject tmp_mostPopular = jsonArr_item.at(0).toObject();
            QJsonObject tmp_content = ProcJsonEasy::getJsonObject(tmp_mostPopular, "content");

            emit this->completeReq_list_mostPopular(tmp_content, ProcJsonEasy::getString(tmp_mostPopular, "type"));


            QList<qobuz::ArtistItemData> list_artist;
            QJsonObject jsonObj_artist = ProcJsonEasy::getJsonObject(p_jsonObj, "artists");
            jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_artist, "items");

            for(int i = 0; i < jsonArr_item.count(); i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::ArtistItemData tmp_data = ConvertData::make_artistData_fromQobuzJsonObj(tmp_json);
                tmp_data.artist_total_cnt = ProcJsonEasy::getInt(jsonObj_artist, "total");
                list_artist.append(tmp_data);
            }

            tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_artist, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_artist, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_artist, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);

            emit this->completeReq_list_artists(list_artist, flag_lastPage);


            QList<qobuz::AlbumItemData> list_album;
            QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(p_jsonObj, "albums");
            jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_album, "items");

            for(int i = 0; i < jsonArr_item.count(); i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::AlbumItemData tmp_data = ConvertData::make_albumData_fromQobuzJsonObj(tmp_json);
                tmp_data.album_total_cnt = ProcJsonEasy::getInt(jsonObj_album, "total");
                list_album.append(tmp_data);
            }

            tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_album, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_album, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_album, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);

            emit this->completeReq_list_albums(list_album, jsonArr_item, flag_lastPage);


            QList<qobuz::TrackItemData> list_track;
            QJsonObject jsonObj_track = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
            jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_track, "items");

            for(int i = 0; i < jsonArr_item.count(); i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(tmp_json);
                tmp_data.appearson_count = ProcJsonEasy::getInt(jsonObj_track, "total");
                list_track.append(tmp_data);
            }

            tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_track, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_track, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_track, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);

            emit this->completeReq_list_tracks(list_track, jsonArr_item, flag_lastPage);



            QList<qobuz::PlaylistItemData> list_playlist;
            QJsonObject jsonObj_playlist = ProcJsonEasy::getJsonObject(p_jsonObj, "playlists");
            jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_playlist, "items");

            for(int i =0; i < jsonArr_item.count(); i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::PlaylistItemData tmp_data = ConvertData::make_playlistData_fromQobuzJsonObj(tmp_json);
                tmp_data.album_total_cnt = ProcJsonEasy::getInt(jsonObj_playlist, "total");
                list_playlist.append(tmp_data);
            }

            tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_playlist, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_playlist, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_playlist, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);

            emit this->completeReq_list_playlists(list_playlist, jsonArr_item, flag_lastPage);
        }


        /*QList<qobuz::ArtistItemData> list_output;
        QJsonObject jsonObj_mostPopular = ProcJsonEasy::getJsonObject(p_jsonObj, "most_popular");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_mostPopular, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i = 0; i < jsonArr_item.count(); i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();
                QJsonObject tmp_content = ProcJsonEasy::getJsonObject(tmp_json, "content");

                // 정보 담을 struct
                qobuz::ArtistItemData tmp_data = ConvertData::make_artistData_fromQobuzJsonObj(tmp_content);
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_mostPopular, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_mostPopular, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_mostPopular, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_mostPopular(list_output, flag_lastPage);*/
    }


    /**
     * @brief ProcCommon::setResult_search_getList_artist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_artist(const QJsonObject &p_jsonObj){

        QList<qobuz::ArtistItemData> list_output;
        QJsonObject jsonObj_artist = ProcJsonEasy::getJsonObject(p_jsonObj, "artists");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_artist, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::ArtistItemData tmp_data = ConvertData::make_artistData_fromQobuzJsonObj(tmp_json);
                tmp_data.artist_total_cnt = ProcJsonEasy::getInt(jsonObj_artist, "total");
                list_output.append(tmp_data);                
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_artist, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_artist, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_artist, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_artists(list_output, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_search_getList_album
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_album(const QJsonObject &p_jsonObj){
        QList<qobuz::AlbumItemData> list_output;
        QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(p_jsonObj, "albums");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_album, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::AlbumItemData tmp_data = ConvertData::make_albumData_fromQobuzJsonObj(tmp_json);
                tmp_data.album_total_cnt = ProcJsonEasy::getInt(jsonObj_album, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_album, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_album, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_album, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_albums(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_search_getList_playlist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_playlist(const QJsonObject &p_jsonObj){
        QList<qobuz::PlaylistItemData> list_output;
        QJsonObject jsonObj_playlist = ProcJsonEasy::getJsonObject(p_jsonObj, "playlists");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_playlist, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::PlaylistItemData tmp_data = ConvertData::make_playlistData_fromQobuzJsonObj(tmp_json);
                tmp_data.album_total_cnt = ProcJsonEasy::getInt(jsonObj_playlist, "total");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_playlist, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_playlist, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_playlist, "total"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_playlists(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_search_getList_video
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_video(const QJsonObject &p_jsonObj){
        QList<qobuz::VideoItemData> list_output;
        QJsonObject jsonObj_video = ProcJsonEasy::getJsonObject(p_jsonObj, "videos");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_video, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                qobuz::VideoItemData tmp_data = ConvertData::make_videoData_fromQobuzJsonObj(tmp_json);
                list_output.append(tmp_data);
            }

            flag_lastPage = this->get_flag_lastPage(jsonObj_video);
        }

        emit this->completeReq_list_videos(list_output, flag_lastPage);
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : private functions
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief Query 를 순차적으로 실행 처리한다.
     * @param list_query 쿼리 문자열 list
     */
    void ProcCommon::procQueryList(const QList<QString> &list_query){

        SqliteHelper *sqliteHelper = new SqliteHelper(this);
        QSqlError err = sqliteHelper->addConnectionLocal();

        for(int i=0 ; i<list_query.length() ; i++){
            sqliteHelper->exec(list_query.at(i));
        }

        sqliteHelper->close();
        delete sqliteHelper;
    }






    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Data 관련 반환
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief @see list_genre 의 length를 반환
     * @return
     */
    int ProcCommon::getCount_listGenreInfo(){
        return list_genre.length();
    }

    /**
     * @brief @see list_mood 의 length를 반환
     * @return
     */
    int ProcCommon::getCount_listMoodsList(){
        return list_mood.length();
    }

    /**
     * @brief @see list_genre의 idx 인덱스에 해당되는 GenreInfo를 반환한다.
     * @param idx list_genre에 접근할 인덱스
     * @return idx가 유효하지 않으면, 비어있는 GenreInfo 반환.
     */
    qobuz::GenreInfo ProcCommon::getGenreInfo(int idx){
        if(idx<0){
            return qobuz::GenreInfo{};
        }
        else if(idx >= list_genre.length()){
            return qobuz::GenreInfo{};
        }
        else {
            return list_genre.at(idx);
        }
    }

    /**
     * @brief @see list_mood의 idx 인덱스에 해당되는 MoodInfo를 반환한다.
     * @param idx list_mood에 접근할 인덱스
     * @return idx가 유효하지 않으면, 비어있는 MoodInfo 반환.
     */
    qobuz::MoodInfo ProcCommon::getMoodInfo(int idx){
        if(idx<0){
            return qobuz::MoodInfo{};
        }
        else if(idx >= list_mood.length()){
            return qobuz::MoodInfo{};
        }
        else {
            return list_mood.at(idx);
        }
//        return qobuz::MoodOption{};
    }

    void ProcCommon::request_qobuz_drawNext(const int idx){
        emit completeReq_drawnext(idx);
    }


}
