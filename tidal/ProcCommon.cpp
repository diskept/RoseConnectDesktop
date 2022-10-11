#include "ConvertData.h"
#include "ProcCommon.h"
#include "ProcRosePlay_withTidal.h"
#include "common/global.h"
#include <QDebug>
#include <QException>
#include <common/networkhttp.h>
#include "common/ProcJsonEasy.h"
#include <common/sqlitehelper.h>


namespace tidal {

    //const QString QUERY_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS User_Tidal (email VARCHAR(50), password TEXT)";
    //const QString QUERY_DEL = "DELETE FROM User_Tidal";
    //const QString QUERY_SEL = "SELECT * FROM User_Tidal";

    const QString QUERY_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS User_Tidal (access_token VARCHAR(2047), refresh_token VARCHAR(2047), expires_in int, token_type VARCHAR(255), email VARCHAR(255))";
    const QString QUERY_DROP_TABLE = "DROP TABLE User_Tidal";
    const QString QUERY_DEL = "DELETE FROM User_Tidal";
    const QString QUERY_SEL = "SELECT * FROM User_Tidal";


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


    void ProcCommon::saveTokenInfo_tidalDB(TidalTokenInfo& p_tidalTokenInfo){

        QString strQuery_insert = "INSERT INTO User_Tidal (access_token, refresh_token, expires_in, token_type, email) VALUES";
        strQuery_insert.append( QString("('%1', '%2', %3, '%4', '%5')")
                                .arg(p_tidalTokenInfo.access_token)
                                .arg(p_tidalTokenInfo.refresh_token)
                                .arg(p_tidalTokenInfo.expires_in)
                                .arg(p_tidalTokenInfo.token_type)
                                .arg(p_tidalTokenInfo.email)
                                );

        // 항상 테이블 생성을 체크하고, 삭제처리 한다. for 초기화 (항상 레코드 최대 1개 유지)
        this->procQueryList(QList<QString>{ QUERY_CREATE_TABLE, QUERY_DEL, strQuery_insert });

    }


    void ProcCommon::clearTablesInfo_tidalDB(){

        // 항상 테이블 생성을 체크하고, 삭제처리 한다.
        this->procQueryList(QList<QString>{ QUERY_CREATE_TABLE, QUERY_DEL });
    }


    TidalTokenInfo ProcCommon::getTokenInfo_tidalDB(){

        TidalTokenInfo output_tokenInfo;

        SqliteHelper *sqliteHelper = new SqliteHelper(this);
        QSqlError err = sqliteHelper->addConnectionLocal();

        sqliteHelper->exec(QUERY_CREATE_TABLE);

        // SELECT 처리
        QVariantList *list = new QVariantList();
        sqliteHelper->exec(QUERY_SEL, *list);
        if(list->size() > 0){
            foreach(QVariant val, *list){
                QMap<QString, QVariant> map = val.toMap();
                output_tokenInfo.access_token = map["access_token"].toString();
                output_tokenInfo.refresh_token = map["refresh_token"].toString();
                output_tokenInfo.expires_in = map["expires_in"].toInt();
                output_tokenInfo.token_type = map["token_type"].toString();
                output_tokenInfo.email = map["email"].toString();
                break;
            }
        }

        sqliteHelper->close();
        delete sqliteHelper;

        return output_tokenInfo;
    }


    void ProcCommon::request_tidal_getTokenAndSave(){

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://auth.tidal.com/v1/oauth2/token

        QUrlQuery params;
        params.addQueryItem("code", global.tidalAccessToken);
        params.addQueryItem("client_id", global.tidal_client_id);
        params.addQueryItem("grant_type", "authorization_code");
        params.addQueryItem("redirect_uri", global.tidal_auth_res_redirectUrl);
        params.addQueryItem("restrictSignup", "true");
        params.addQueryItem("code_verifier", "rose");
        params.addQueryItem("scope", "r_usr+w_usr");

        network->request_forTidal(ProcCommon::HttpRequestType::Get_Token
                                 , QString("%1/oauth2/token").arg(global.tidal_api_rest_auth_url)
                                 , NetworkHttp::HeaderOption::Tidal_ReqAccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Post);
    }


    void ProcCommon::request_tidal_getRefreshTokenAndSave(){

        ProcCommon *proc = new ProcCommon();
        UserLoginInfo tidal_userLoginInfo = proc->getLoginInfo_tidalDB();

        // occur error
        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://auth.tidal.com/v1/oauth2/token
        //print_tidal_func();
        //qDebug() << global.user_forTidal.get_refresh_token() << "," << global.tidal_client_id << "\n";

        QUrlQuery params;
        params.addQueryItem("refresh_token", global.user_forTidal.get_refresh_token());
        params.addQueryItem("client_id", global.tidal_client_id);
        params.addQueryItem("client_secret", "ST*e!UJnST5%2N'9");
        params.addQueryItem("scope", "r_usr+w_usr");
        params.addQueryItem("grant_type", "refresh_token");

        network->request_forTidal(ProcCommon::HttpRequestType::Get_refreshToken
                                 , QString("%1/oauth2/token").arg(global.tidal_api_rest_auth_url)
                                 , NetworkHttp::HeaderOption::Tidal_ReqAccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Post);
    }


    void ProcCommon::request_tidal_getSession(){

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.tidal.com/v1/sessions

        QUrlQuery params;

        network->request_forTidal(ProcCommon::HttpRequestType::Get_Session
                                 , QString("%1/sessions").arg(global.tidal_api_rest_url)
                                 , NetworkHttp::HeaderOption::Tidal_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get);
    }


    void ProcCommon::request_tidal_getSubscription(int user_id){

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.tidal.com/v1/users/115968702/subscription

        QUrlQuery params;

        network->request_forTidal(ProcCommon::HttpRequestType::Get_Subscription
                                 , QString("%1/users/%2/subscription").arg(global.tidal_api_rest_url).arg(user_id)
                                 , NetworkHttp::HeaderOption::Tidal_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get);
    }


    void ProcCommon::request_tidal_setLogOut(){

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.tidal.com/v1/logout

        //QUrlQuery params;

        network->request_forTidal(ProcCommon::HttpRequestType::Logout
                                 , QString("%1/logout").arg(global.tidal_api_rest_url)
                                 , NetworkHttp::HeaderOption::Tidal_AccessToken
                                 //, params
                                 , QUrlQuery{}
                                 , NetworkHttp::RequestMethod::Post
                                 , true);
    }



    // MARK : public functions  --------------------------------------------------------------------------------------------------------

    /**
     * @brief 타이달 API에서 반환하는 이미지 key값에 대해서, 타이달 Resource URL로 변환하여 반환
     * @details  베어앱 노트 참고 - [[Rose Tidal - 이미지 URL화 방법]]
     * @param tidalImageKey 타이달API에서 반환하는 이미지 key값
     * @param imageSize 이미지 리소스에서 제공하는 가로세로 동일값 사이즈. 타이달API에 따라 차이가 있음.
     * @return 타이달 Resource Url 반환
     */
    QString ProcCommon::convertImagePath(const QString tidalImageKey, const int imageSize){

        if(tidalImageKey.isEmpty()){
            return "";
        }
        else if(imageSize <= 0){
            return "";
        }
        else{
            QString output = tidalImageKey;
            output.replace(QString("-"), QString("/"), Qt::CaseSensitive);
//            output.insert(0, "https://resources.tidal.com/images/");          // https
//            output.insert(0, "http://resources.tidal.com/images/");             // http
            output.insert(0, "https://resources.wimpmusic.com/images/");
            output.append( QString("/%1x%1.jpg").arg(imageSize) );
            return output;
        }
    }


    /**
     * @brief 타이달 로그인 정보를 sqllite3 정보로 저장함
     * @param username : 타이달 사용자 email 정보
     * @param password : 타이달 사용자 비밀번호 정보
     */
    void ProcCommon::saveLoginInfo_tidalDB(UserLoginInfo &userLoginInfo){

        //QString strQuery_insert = " INSERT INTO User_Tidal (email, password) VALUES ";
        //strQuery_insert.append( QString("('%1', '%2')").arg(userLoginInfo.username).arg(userLoginInfo.password));
        QString strQuery_insert = "INSERT INTO User_Tidal (access_token, refresh_token, expires_in, token_type, email) VALUES";
        strQuery_insert.append( QString("('%1', '%2', %3, '%4', '%5')")
                                .arg(userLoginInfo.access_token)
                                .arg(userLoginInfo.refresh_token)
                                .arg(userLoginInfo.expires_in)
                                .arg(userLoginInfo.token_type)
                                .arg(userLoginInfo.email)
                                );

        // 항상 테이블 생성을 체크하고, 삭제처리 한다. for 초기화 (항상 레코드 최대 1개 유지)
        this->procQueryList(QList<QString>{ QUERY_CREATE_TABLE, QUERY_DEL, strQuery_insert });
    }


    /**
     * @brief 타이달 로그인 정보를 clear 처리함
     */
    void ProcCommon::clearLoginInfo_tidalDB(){

        // 항상 테이블 생성을 체크하고, 삭제처리 한다.
        this->procQueryList(QList<QString>{ QUERY_CREATE_TABLE, QUERY_DEL });
    }


    /**
     * @brief 로그인 정보를 SELECT 해서 userLoginInfo에 세팅한다.
     * @param userLoginInfo
     */
    UserLoginInfo ProcCommon::getLoginInfo_tidalDB(){

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
                //output_userLoginInfo.username = map["email"].toString();
                //output_userLoginInfo.password = map["password"].toString();
                output_userLoginInfo.access_token = map["access_token"].toString();
                output_userLoginInfo.refresh_token = map["refresh_token"].toString();
                output_userLoginInfo.email = map["email"].toString();
                output_userLoginInfo.token_type = map["token_type"].toString();
                output_userLoginInfo.expires_in = map["expires_in"].toInt();
                output_userLoginInfo.flagSavedLoginInfo = true;

                //qDebug() << "ProcCommon::getLoginInfo_tidalDB()------------------------------";
                //qDebug() << output_userLoginInfo.username;
                //qDebug() << output_userLoginInfo.password;
                break;
            }
        }

        if(output_userLoginInfo.access_token.isEmpty())
        {
            sqliteHelper->exec(QUERY_DROP_TABLE);
        }

        sqliteHelper->close();
        delete sqliteHelper;

        return output_userLoginInfo;
    }


    // MARK : Request API - 일반 데이터 요청 (List or 단일 형태) -------------------------------------------------------------------

    /**
     * @brief HTTP 요청 - 타이달 로그인 API
     * @param userLoginInfo
     * @see https://documenter.getpostman.com/view/448610/HXVi?version=latest#11d1026f-3789-480c-aa99-57dae2a357b1
     */
    void ProcCommon::request_tidalLogin(UserLoginInfo userLoginInfo){

        // slot 에서 추가처리를 위해 멤버변수로 저장해둠
        this->userLoginInfo = userLoginInfo;

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        //params.addQueryItem("username", userLoginInfo.username);
        //params.addQueryItem("password", userLoginInfo.password);
        params.addQueryItem("clientUniqueKey", "");
        //qDebug() << "ProcCommon::request_tidalLogin()--------------";//cheon_debug01
        //qDebug() << userLoginInfo.username;
        //qDebug() << userLoginInfo.password;
        network->request_forTidal(Login,
                         QString("%1/login/username").arg(global.tidalAPI_url),
                         NetworkHttp::HeaderOption::Tidal_X_Token,
                         params,
                         NetworkHttp::RequestMethod::Post);

    }


    /**
     * @brief HTTP 요청 - 타이달 로그아웃 API
     * @see https://documenter.getpostman.com/view/448610/HXVi?version=latest#98e2e430-ce2e-455e-a171-e4552195c822
     */
    /*void ProcCommon::request_tidalLogout(){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        network->request_forTidal(Logout,
                         QString("%1/logout").arg(global.tidalAPI_url),
                         NetworkHttp::HeaderOption::Tidal_X_Session,
                         QUrlQuery{},
                         NetworkHttp::RequestMethod::Post);
    }*/
    void ProcCommon::request_tidalLogout(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.tidal.com/v1/logout

        network->request_forTidal(Logout,
                         QString("%1/logout").arg(global.tidal_api_rest_url),
                         NetworkHttp::HeaderOption::Tidal_AccessToken,
                         QUrlQuery{},
                         NetworkHttp::RequestMethod::Post);
    }


    /**
     * @brief HTTP 요청 - 타이달 Genres 종류 전체반환 API
     * @details - @see list_genre 변수에 데이터가 없는 경우에만 내부 동작한다. (최초 1번만 세팅하기 위함)
     */
    void ProcCommon::request_tidal_getListGenres(){

        if(list_genre.length() == 0){
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QUrlQuery params;
            params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());

            network->request_forTidal(GetList_Genres,
                             QString("%1/genres").arg(global.tidal_api_rest_url),
                             //NetworkHttp::HeaderOption::Tidal_X_Token,
                             NetworkHttp::HeaderOption::Tidal_AccessToken,
                             params,
                             NetworkHttp::RequestMethod::Get);
        }
    }


    /**
     * @brief HTTP 요청 - 타이달 Moods & Activities 종류 전체반환 API
     * @details - @see list_mood 변수에 데이터가 없는 경우에만 내부 동작한다. (최초 1번만 세팅하기 위함)
     */
    void ProcCommon::request_tidal_getListMoods(){

        if(list_mood.length() == 0){
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QUrlQuery params;
            params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
            network->request_forTidal(GetList_Moods,
                             QString("%1/moods").arg(global.tidal_api_rest_url),
                             //NetworkHttp::HeaderOption::Tidal_X_Token,
                             NetworkHttp::HeaderOption::Tidal_AccessToken,
                             params,
                             NetworkHttp::RequestMethod::Get);
        }
    }


    /**
     * @brief Tidal API - Playlist 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_mixes(const QString api_subPath, const int limit, const int offset){

        Q_UNUSED(limit);
        Q_UNUSED(offset);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());

        network->request_forTidal(HttpRequestType::GetList_Mixes,
                         QString("%1/%2").arg(global.tidal_api_rest_url).arg(api_subPath),
                         //NetworkHttp::HeaderOption::Tidal_X_Token,
                         NetworkHttp::HeaderOption::Tidal_AccessToken,
                         params,
                         NetworkHttp::RequestMethod::Get);
    }


    /**
     * @brief Tidal API - Playlist 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_playlists(const QString api_subPath, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }
        network->request_forTidal(HttpRequestType::GetList_Playlists
                                  , QString("%1/%2").arg(global.tidal_api_rest_url).arg(api_subPath)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get);
    }


    /**
     * @brief Tidal API - Album 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_albums(const QString api_subPath, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }
        network->request_forTidal(HttpRequestType::GetList_Albums,
                         QString("%1/%2").arg(global.tidal_api_rest_url).arg(api_subPath),
                         //NetworkHttp::HeaderOption::Tidal_X_Token,
                         NetworkHttp::HeaderOption::Tidal_AccessToken,
                         params,
                         NetworkHttp::RequestMethod::Get);
    }


    void ProcCommon::request_tidal_getList_visual(const QString api_subPath, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.tidal.com/v1/search?limit=50&offset=0&types=ALBUMS&query=visual%20album&countryCode=CA
        if(offset == 0){
            this->visual_album_offset = false;
        }
        else{
            this->visual_album_offset = true;
        }
        QUrlQuery params;
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }
        params.addQueryItem("types", "ALBUMS");
        params.addQueryItem("query", api_subPath);
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());

        network->request_forTidal(HttpRequestType::GetList_Visual,
                         QString("%1/search").arg(global.tidal_api_rest_url),
                         //NetworkHttp::HeaderOption::Tidal_X_Token,
                         NetworkHttp::HeaderOption::Tidal_AccessToken,
                         params,
                         NetworkHttp::RequestMethod::Get);
    }


    /**
     * @brief Tidal API - Artist 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_artists(const QString api_subPath, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }
        network->request_forTidal(HttpRequestType::GetList_Artists
                                  , QString("%1/%2").arg(global.tidal_api_rest_url).arg(api_subPath)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - Track 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_tracks(const QString api_subPath, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }
        network->request_forTidal(HttpRequestType::GetList_Tracks
                                  , QString("%1/%2").arg(global.tidal_api_rest_url).arg(api_subPath)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - Video 목록 요청함 (Geenral)
     * @param path
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_videos(const QString api_subPath, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());

        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forTidal(HttpRequestType::GetList_Videos
                                  , QString("%1/%2").arg(global.tidal_api_rest_url).arg(api_subPath)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - Playlist 1개에 대한 상세정보 요청
     * @param album_id
     */
    void ProcCommon::request_tidal_get_playlist(const QString playlist_id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        network->request_forTidal(HttpRequestType::Get_PlaylistInfo
                                  , QString("%1/playlists/%2").arg(global.tidal_api_rest_url).arg(playlist_id)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - Album 1개에 대한 상세정보 요청
     * @param album_id
     */
    void ProcCommon::request_tidal_get_album(const int album_id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        network->request_forTidal(HttpRequestType::Get_AlbumInfo
                                  , QString("%1/albums/%2").arg(global.tidal_api_rest_url).arg(album_id)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - Artist 1개에 대한 상세정보 요청
     * @param artist_id
     */
    void ProcCommon::request_tidal_get_artist(const int artist_id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        network->request_forTidal(HttpRequestType::Get_ArtistInfo
                                  , QString("%1/artists/%2").arg(global.tidal_api_rest_url).arg(artist_id)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - Artist 1개에 대하여 bio 정보 텍스트(description) 요청
     * @param artist_id
     */
    void ProcCommon::request_tidal_get_artist_bioText(const int artist_id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        network->request_forTidal(HttpRequestType::Get_ArtistBioText
                                  , QString("%1/artists/%2/bio").arg(global.tidal_api_rest_url).arg(artist_id)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - 나의 Created Playlist 목록을 요청
     * @param orderOpt 정렬옵션
     * @param orderDirection 정렬방향 (내림, 올림)
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_myCreatedPlaylists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset, const QJsonObject& jsonObj_opt){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.tidal.com/v1/users/115968702/playlists?offset=0&countryCode=CA&limit=50&orderDirection=DESC&order=DATE

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        params.addQueryItem("order", QString("%1").arg( QVariant::fromValue(orderOpt).toString() ));
        params.addQueryItem("orderDirection", QString("%1").arg( QVariant::fromValue(orderDirection).toString() ));
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forTidal(HttpRequestType::GetList_My_Created_Playlists
                                  , QString("%1/users/%2/playlists").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId())
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false
                                  , jsonObj_opt);
    }


    /**
     * @brief Tidal API - 나의 Favorite Playlist 목록을 요청
     * @param orderOpt
     * @param orderDirection
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_myFavoritePlaylists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.tidal.com/v1/users/115968702/favorites/playlists?offset=0&countryCode=CA&limit=50&orderDirection=DESC&order=DATE

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        params.addQueryItem("order", QString("%1").arg( QVariant::fromValue(orderOpt).toString() ));
        params.addQueryItem("orderDirection", QString("%1").arg( QVariant::fromValue(orderDirection).toString() ));
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forTidal(HttpRequestType::GetList_My_Favorite_Playlists
                                  , QString("%1/users/%2/favorites/playlists").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId())
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - 나의 Favorite Album 목록을 요청
     * @param orderOpt
     * @param orderDirection
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_myFavoriteAlbums(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        params.addQueryItem("order", QString("%1").arg( QVariant::fromValue(orderOpt).toString() ));
        params.addQueryItem("orderDirection", QString("%1").arg( QVariant::fromValue(orderDirection).toString() ));
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forTidal(HttpRequestType::GetList_My_Favorite_Albums
                                  , QString("%1/users/%2/favorites/albums").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId())
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - 나의 Favorite Track 목록을 요청
     * @param orderOpt
     * @param orderDirection
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_myFavoriteTracks(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        params.addQueryItem("order", QString("%1").arg( QVariant::fromValue(orderOpt).toString() ));
        params.addQueryItem("orderDirection", QString("%1").arg( QVariant::fromValue(orderDirection).toString() ));
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forTidal(HttpRequestType::GetList_My_Favorite_Tracks
                                  , QString("%1/users/%2/favorites/tracks").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId())
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - 나의 Favorite Artist 목록을 요청
     * @param orderOpt
     * @param orderDirection
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_myFavoriteArtists(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset) {

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        params.addQueryItem("order", QString("%1").arg( QVariant::fromValue(orderOpt).toString() ));
        params.addQueryItem("orderDirection", QString("%1").arg( QVariant::fromValue(orderDirection).toString() ));
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forTidal(HttpRequestType::GetList_My_Favorite_Artists
                                  , QString("%1/users/%2/favorites/artists").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId())
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - 나의 Favorite Video 목록을 요청
     * @param orderOpt
     * @param orderDirection
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_myFavoriteVideos(const ProcCommon::DataOrderOption orderOpt, const ProcCommon::DataOrderDirection orderDirection, const int limit, const int offset) {

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        params.addQueryItem("order", QString("%1").arg( QVariant::fromValue(orderOpt).toString() ));
        params.addQueryItem("orderDirection", QString("%1").arg( QVariant::fromValue(orderDirection).toString() ));
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }

        network->request_forTidal(HttpRequestType::GetList_My_Favorite_Videos
                                  , QString("%1/users/%2/favorites/videos").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId())
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - mixes 에 있는 track 목록을 요청
     * @param playlist_uuid
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_items_of_mixes(const QString playlist_uuid, const int limit, const int offset){

        Q_UNUSED(limit);
        Q_UNUSED(offset);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());

        network->request_forTidal(GetList_Items_Mixes
                                  , QString("%1/mixes/%2/items").arg(global.tidal_api_rest_url).arg(playlist_uuid)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - Playlist 에 있는 track 목록을 요청
     * @param playlist_uuid
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_getList_items_of_playlist(const QString playlist_uuid, const int limit, const int offset){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        if(offset >= 0){
            params.addQueryItem("offset", QString("%1").arg(offset));
        }
        network->request_forTidal(GetList_Items_Playlist
                                  , QString("%1/playlists/%2/items").arg(global.tidal_api_rest_url).arg(playlist_uuid)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - Album 에 있는 track 목록을 요청.
     * @details limit, offset 없음. 보통 alubm 내에 트랙 개수는 적으니까,,, Tidal Api에 옵션이 없음.
     * @param album_id
     */
    void ProcCommon::request_tidal_getList_items_of_album(const int album_id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        network->request_forTidal(GetList_Items_Album
                                  , QString("%1/albums/%2/tracks").arg(global.tidal_api_rest_url).arg(album_id)
                                  //, QString("%1/albums/%2/items").arg(global.tidal_api_rest_url).arg(album_id)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    /**
     * @brief Tidal API - Album 에 있는 track 목록을 요청.
     * @details limit, offset 없음. 보통 alubm 내에 트랙 개수는 적으니까,,, Tidal Api에 옵션이 없음.
     * @param album_id
     */
    void ProcCommon::request_tidal_getList_items_of_visual(const int album_id, const int limit){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        if(limit >= 0){
            params.addQueryItem("limit", QString("%1").arg(limit));
        }
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        network->request_forTidal(GetList_Items_Visual
                                  , QString("%1/albums/%2/items").arg(global.tidal_api_rest_url).arg(album_id)
                                  //, QString("%1/albums/%2/items").arg(global.tidal_api_rest_url).arg(album_id)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    // MARK : Favorites 처리 ( get & set ) -----------------------------------------------------------------------------------

    /**
     * @brief TIDAL API 요청 - 좋아요 체크 여부를 확인하기 위해 필요한 정보임
     */
    void ProcCommon::request_tidal_getAll_favorites(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        network->request_forTidal(HttpRequestType::GetAll_UserFavorites
                                  , QString("%1/users/%2/favorites/ids").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId())
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    void ProcCommon::request_tidal_set_favorite(const QString type, const QString id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.tidal.com/v1/users/183114148/favorites/playlists?countryCode=CA
        //uuids=4b7afd58-c129-43d0-b463-378afa62e5fd
        //trackIds=215369262

        QUrlQuery params_get;
        params_get.addQueryItem("countryCode", global.user_forTidal.getCountryCode());

        QUrlQuery params_post;
        if(type == "playlists"){
            params_post.addQueryItem("uuids", id);
        }
        else if(type == "albums"){
            params_post.addQueryItem("albumIds", id);
        }
        else if(type == "tracks"){
            params_post.addQueryItem("trackIds", id);
        }

        network->request_forTidal(HttpRequestType::GetAll_UserFavorites
                                  , QString("%1/users/%2/favorites/%3").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId()).arg(type)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params_post
                                  , params_get
                                  , false);
    }


    void ProcCommon::request_tidal_del_favorite(const QString type, const QString id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        network->request_forTidal(HttpRequestType::GetAll_UserFavorites,
                                  QString("%1/users/%2/favorites/%3/%4").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId()).arg(type).arg(id),
                                  //NetworkHttp::HeaderOption::Tidal_X_Session,
                                  NetworkHttp::HeaderOption::Tidal_AccessToken,
                                  QUrlQuery(),
                                  NetworkHttp::RequestMethod::Delete,
                                  false);
    }


    // 즐겨찾기 추가 ---------------------------------------------------------------------------------------------

    /**
     * @brief TIDAL API 요청 - 즐겨찾기 추가 (Album)
     * @param album_id
     */
    void ProcCommon::request_tidal_addFavorite_album(const int album_id){

        this->request_tidal_addFavorie(Favorite_Add_Album, "albumIds", album_id, "albums");
    }


    /**
     * @brief TIDAL API 요청 - 즐겨찾기 추가 (Aritst)
     * @param artist_id
     */
    void ProcCommon::request_tidal_addFavorite_artist(const int artist_id){

        this->request_tidal_addFavorie(Favorite_Add_Artist, "artistIds", artist_id, "artists");
    }


    /**
     * @brief TIDAL API 요청 - 즐겨찾기 추가 (Track)
     * @param artist_id
     */
    void ProcCommon::request_tidal_addFavorite_track(const int track_id){

        this->request_tidal_addFavorie(Favorite_Add_Track, "trackIds", track_id, "tracks");
    }


    /**
     * @brief TIDAL API 요청 - 즐겨찾기 추가 (Playlist)
     * @param artist_id
     */
    void ProcCommon::request_tidal_addFavorite_playlist(const QString playlist_uuid){

        this->request_tidal_addFavorie(Favorite_Add_Playlist, "uuids", playlist_uuid, "playlists");
    }


    /**
     * @brief TIDAL API 요청 - 즐겨찾기 추가 (Video)
     * @param artist_id
     */
    void ProcCommon::request_tidal_addFavorite_video(const int video_id){

        this->request_tidal_addFavorie(Favorite_Add_Video, "videoIds", video_id, "videos");
    }


    /**
     * @brief TIDAL API 요청 - 각 타입별로 즐겨찾기 추가에 대해 요청
     * @param requestType
     * @param id_key
     * @param id_val
     * @param subPath
     */
    void ProcCommon::request_tidal_addFavorie(const HttpRequestType requestType, const QString id_key, const QVariant id_val, const QString subPath){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        //https://api.tidal.com/v1/users/183114148/favorites/playlists?countryCode=CA

        QUrlQuery params_post;
        params_post.addQueryItem(id_key, id_val.toString());

        QUrlQuery params_get;
        params_get.addQueryItem("countryCode", global.user_forTidal.getCountryCode());

        network->request_forTidal(requestType,
                                  QString("%1/users/%2/favorites/%3").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId()).arg(subPath),
                                  //NetworkHttp::HeaderOption::Tidal_X_Session,
                                  NetworkHttp::HeaderOption::Tidal_AccessToken,
                                  params_post,
                                  params_get,
                                  false);
    }


    // 즐겨찾기 삭제 ---------------------------------------------------------------------------------------------

    /**
     * @brief TIDAL API 요청 - 즐겨찾기 삭제 (Album)
     * @param album_id
     */
    void ProcCommon::request_tidal_deleteFavorite_album(const int album_id){

        this->request_tidal_deleteFavorie(Favorite_Delete_Album, album_id, "albums");
    }


    /**
     * @brief TIDAL API 요청 - 즐겨찾기 삭제 (Artist)
     * @param artist_id
     */
    void ProcCommon::request_tidal_deleteFavorite_artist(const int artist_id){

        this->request_tidal_deleteFavorie(Favorite_Delete_Artist, artist_id, "artists");
    }


    /**
     * @brief TIDAL API 요청 - 즐겨찾기 삭제 (Track)
     * @param track_id
     */
    void ProcCommon::request_tidal_deleteFavorite_track(const int track_id){

        this->request_tidal_deleteFavorie(Favorite_Delete_Track, track_id, "tracks");
    }


    /**
     * @brief TIDAL API 요청 - 즐겨찾기 삭제 (Playlist)
     * @param playlist_uuid
     */
    void ProcCommon::request_tidal_deleteFavorite_playlist(const QString playlist_uuid){

        this->request_tidal_deleteFavorie(Favorite_Delete_Playlist, playlist_uuid, "playlists");
    }


    /**
     * @brief TIDAL API 요청 - 즐겨찾기 삭제 (Video)
     * @param video_id
     */
    void ProcCommon::request_tidal_deleteFavorite_video(const int video_id){

        this->request_tidal_deleteFavorie(Favorite_Delete_Video, video_id, "videos");
    }


    /**
     * @brief TIDAL API 요청 - 각 타입별로 즐겨찾기 삭제에 대해 요청
     * @param requestType
     * @param id_val
     * @param subPath
     */
    void ProcCommon::request_tidal_deleteFavorie(const HttpRequestType requestType, const QVariant id_val, const QString subPath){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        network->request_forTidal(requestType,
                                  QString("%1/users/%2/favorites/%3/%4").arg(global.tidal_api_rest_url).arg(global.user_forTidal.getUserId()).arg(subPath).arg(id_val.toString()),
                                  //NetworkHttp::HeaderOption::Tidal_X_Session,
                                  NetworkHttp::HeaderOption::Tidal_AccessToken,
                                  QUrlQuery(),
                                  NetworkHttp::RequestMethod::Delete,
                                  false);
    }


    // User's Playlist 관련 -------------------------------------------------------------------------------------------

    /**
     * @brief User created playlist 를 삭제 요청
     * @param playlist_uuid
     */
    void ProcCommon::request_tidal_delete_myCreatedPlaylist(const QString playlist_uuid, const QJsonObject& jsonObj_opt){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        network->request_forTidal(ProcCommon::HttpRequestType::UserCreatePlaylist_Delete
                                  , QString("%1/playlists/%2").arg(global.tidal_api_rest_url).arg(playlist_uuid)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , QUrlQuery()
                                  , NetworkHttp::RequestMethod::Delete
                                  , false
                                  , jsonObj_opt);
    }


    /**
     * @brief request_tidal_addPlaylist_track
     * @param playlist_uuid
     * @param track_id
     */
    void ProcCommon::request_tidal_add_track_in_myPlaylist(const QString playlist_uuid_target, const int track_id){

        this->request_tidal_addPlaylist_items(playlist_uuid_target, QString("%1").arg(track_id));
    }


    /**
     * @brief ProcCommon::request_tidal_add_album_in_myPlaylist
     * @param playlist_uuid_target
     * @param album_id
     */
    void ProcCommon::request_tidal_add_album_in_myPlaylist(const QString playlist_uuid_target, const int album_id){

        // album_id 로 내부 트랙 목록 요청 필요
        QJsonObject jsonObj_opt;
        jsonObj_opt.insert("playlist_uuid_target", playlist_uuid_target);
        connect(this, &ProcCommon::completeReq_list_trackIds_toAddPlaylist, this, &ProcCommon::slot_getDone_trackIds_toAddPlaylist);
        this->request_tidal_getList_trackIds_of_album(album_id, jsonObj_opt);
    }


    /**
     * @brief ProcCommon::request_tidal_add_playlist_in_myPlaylist
     * @param playlist_uuid_target
     * @param playlist_uuid
     */
    void ProcCommon::request_tidal_add_playlist_in_myPlaylist(const QString playlist_uuid_target, const QString playlist_uuid){

        // playlist_uuid 로 내부 트랙 목록 요청 필요
        QJsonObject jsonObj_opt;
        jsonObj_opt.insert("playlist_uuid_target", playlist_uuid_target);
        connect(this, &ProcCommon::completeReq_list_trackIds_toAddPlaylist, this, &ProcCommon::slot_getDone_trackIds_toAddPlaylist);
        this->request_tidal_getList_trackIds_of_playlist(playlist_uuid, jsonObj_opt);
    }


    /**
     * @brief 플레이리스트 담기 요청
     * @param uuid
     * @param itemIds
     */
    void ProcCommon::request_tidal_addPlaylist_items(QString playlist_uuid, QString itemIds){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip
        //https://api.tidal.com/v1/playlists/e2950c73-1cd3-40fc-b3e6-3883e2448dfb/items

        QUrlQuery params;
        params.addQueryItem("toIndex", "0");
        params.addQueryItem("itemIds", itemIds);

        network->request_forTidal(ProcCommon::HttpRequestType::UserCreatePlaylist_Add_Tracks
                                  , QString("%1/playlists/%2/items").arg(global.tidal_api_rest_url).arg(playlist_uuid)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session_with_IF_None_Match
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken_with_IF_None_Match
                                  , params
                                  , NetworkHttp::RequestMethod::Post
                                  , false);
    }


    /**
     * @brief ProcCommon::request_tidal_getList_trackIds_of_playlist
     * @param playlist_uuid
     */
    void ProcCommon::request_tidal_getList_trackIds_of_playlist(const QString playlist_uuid, const QJsonObject& jsonObj_opt){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        params.addQueryItem("offset", "0");
        params.addQueryItem("limit", "99");         // 99 몇개를 넣어야할까.
        network->request_forTidal(GetList_TrackIds_Playlist
                                  , QString("%1/playlists/%2/items").arg(global.tidal_api_rest_url).arg(playlist_uuid)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false
                                  , jsonObj_opt);
    }


    /**
     * @brief Album 안에 있는 모든 트랙에 대한 id 만을 따로 추출하기 위해 요청함.
     * @details 원래 @see request_tidal_getList_items_of_album 함수와 바디는 같다.
     * 그러나 결과물 처리 및 연결 signal을 구분하기 위해서 함수를 따로 분리하였음.
     * @param album_id
     */
    void ProcCommon::request_tidal_getList_trackIds_of_album(const int album_id, const QJsonObject& jsonObj_opt){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        network->request_forTidal(GetList_TrackIds_Album
                                  , QString("%1/albums/%2/tracks").arg(global.tidal_api_rest_url).arg(album_id)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false
                                  , jsonObj_opt);
    }


    /**
     * @brief completeReq_list_trackIds_toAddPlaylist 시그널에 대한 처리를 진행
     */
    void ProcCommon::slot_getDone_trackIds_toAddPlaylist(const QString playlist_uuid_target, const QString track_ids){

        this->request_tidal_addPlaylist_items(playlist_uuid_target, track_ids);
    }


    /**
     * @brief ProcCommon::request_tidal_delete_items_in_myPlaylist
     * @param playlist_uuid_target
     * @param list_itemIds
     */
    void ProcCommon::request_tidal_delete_items_in_myPlaylist(const QString playlist_uuid, const QList<QString>& list_itemIds){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        network->request_forTidal(DeleteItems_in_MyCreatedPlaylist
                                  , QString("%1/playlists/%2/items/%3").arg(global.tidal_api_rest_url).arg(playlist_uuid).arg(list_itemIds.join(","))
                                  //, NetworkHttp::HeaderOption::Tidal_X_Session_with_IF_None_Match
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken_with_IF_None_Match
                                  , params
                                  , NetworkHttp::RequestMethod::Delete
                                  , false);
    }


    void ProcCommon::request_tidal_create_myPlaylist(const int user_id, const QString playlsit_title){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.tidal.com/v1/users/183114148/playlists
        //description=&title=PLAYLIST%20CREAT%20TEST
        QUrlQuery params;
        params.addQueryItem("description", "");
        params.addQueryItem("title", playlsit_title);

        network->request_forTidal(Create_userPlaylist
                                  , QString("%1/users/%2/playlists").arg(global.tidal_api_rest_url).arg(user_id)
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Post
                                  );
    }


    void ProcCommon::request_tidal_addTracks_myPlaylist(const QString playlist_id, const QString track_id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //https://api.tidal.com/v1/playlists/26da0c32-98f9-489b-b275-78e408067930/items
        //toIndex=0&itemIds=208447786%2C208447787%2C208447788%2C208447789%2C208447790%2C208447791%2C208447792%2C208447793%2C208447794%2C208447795%2C208447796%2C208447797%2C208447798%2C208447799%2C208447800%2C208447801%2C208447802%2C208447803%2C
        QUrlQuery params;
        params.addQueryItem("toIndex", "0");
        params.addQueryItem("itemIds", track_id);

        network->request_forTidal(AddTracks_userPlaylist
                                  , QString("%1/playlists/%2/items").arg(global.tidal_api_rest_url).arg(playlist_id)
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken_with_IF_None_Match
                                  , params
                                  , NetworkHttp::RequestMethod::Post
                                  );
    }


    /**
     * @brief ProcCommon::request_tidal_searchAllItems
     * @param search_word
     * @param limit
     * @param offset
     */
    void ProcCommon::request_tidal_searchAllItems(const QString search_word, const int limit, const int offset, const TidalSearch_ContentType contentType){

        Q_UNUSED(contentType);

        HttpRequestType httpRequestType = HttpRequestType::Search_GetList_Track;
        if(contentType == TidalSearch_ContentType::Tracks){
            httpRequestType = HttpRequestType::Search_GetList_Track;
        }
        else if(contentType == TidalSearch_ContentType::Artists){
            httpRequestType = HttpRequestType::Search_GetList_Artist;
        }
        else if(contentType == TidalSearch_ContentType::Albums){
            httpRequestType = HttpRequestType::Search_GetList_Album;
        }
        else if(contentType == TidalSearch_ContentType::Playlists){
            httpRequestType = HttpRequestType::Search_GetList_Playlist;
        }
        else if(contentType == TidalSearch_ContentType::Videos){
            httpRequestType = HttpRequestType::Search_GetList_Video;
        }
        else if(contentType == TidalSearch_ContentType::TopHit){
            httpRequestType = HttpRequestType::Search_GetList_TopHit;
        }

        QString str_contentType = "";
        if(contentType == TidalSearch_ContentType::TopHit){
            str_contentType = "TRACKS,ALBUMS,PLAYLISTS,ARTISTS,VIDEOS";
        }
        else{
            str_contentType = QVariant::fromValue(contentType).toString().toUpper();
        }

        // request
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("types", str_contentType);
        params.addQueryItem("offset", QString("%1").arg(offset));
        params.addQueryItem("countryCode", global.user_forTidal.getCountryCode());
        params.addQueryItem("query", search_word);
        params.addQueryItem("limit", QString("%1").arg(limit));

        network->request_forTidal(httpRequestType
                                  , QString("%1/search").arg(global.tidalAPI_url)
                                  //, NetworkHttp::HeaderOption::Tidal_X_Token
                                  , NetworkHttp::HeaderOption::Tidal_AccessToken
                                  , params
                                  , NetworkHttp::RequestMethod::Get
                                  , false);
    }


    // MARK : Http 요청 결과 처리 -------------------------------------------------------------------------------------------

    /**
     * @brief [슬롯] Http 요청 결과 처리. p_id에 따라 분기 처리 진행된다.
     * @details
     *      @li 로그인 처리 : @see setResult_loginTidal
     *      @li 로그아웃 처리 : @see setResult_logoutTidal
     * @param p_id 요청 아이디
     * @param p_jsonObject HTTP 응답 결과
     */
    void ProcCommon::slot_responseHttp(int p_id, QJsonObject p_jsonObj){
        //print_tidal_func();
        //QJsonDocument doc(p_jsonObj);                                  //cheon4_code
        //QString strJson(doc.toJson(QJsonDocument::Compact));            //cheon4_code
        //qDebug() << "p_id : " << p_id;                                  //cheon4_code
        //Debug() << "p_jsonObj" << strJson;                             //cheon4_code
        NetworkHttp* sender_http = qobject_cast<NetworkHttp*>(sender());

        switch (p_id) {
            case Login:             setResult_loginTidal(p_jsonObj);        break;
            case Get_Token:         setResult_getToken(p_jsonObj);          break;
            case Get_refreshToken:  setResult_getRefreshToken(p_jsonObj);   break;
            case Get_Session:       setResult_getSessionInfo(p_jsonObj);    break;
            case Get_Subscription:  setResult_getSubscription(p_jsonObj);   break;

            case Logout:           setResult_logoutTidal(p_jsonObj);    break;

            case GetList_Genres:   setResult_listGenres(p_jsonObj);     break;

            case GetList_Moods:    setResult_listMoods(p_jsonObj);      break;

            case HttpRequestType::GetList_Mixes :       setResult_list_mixeslists(p_jsonObj);       break;
            case HttpRequestType::GetList_Playlists :   setResult_list_playlists(p_jsonObj);        break;
            case HttpRequestType::GetList_Albums :      setResult_list_albums(p_jsonObj);           break;
            case HttpRequestType::GetList_Visual :      setResult_list_viuals(p_jsonObj);           break;
            case HttpRequestType::GetList_Artists :     setResult_list_artists(p_jsonObj);          break;
            case HttpRequestType::GetList_Tracks :      setResult_list_tracks(p_jsonObj);           break;
            case HttpRequestType::GetList_Videos :      setResult_list_videos(p_jsonObj);           break;

            case HttpRequestType::Get_PlaylistInfo :    setResult_playlist(p_jsonObj);              break;
            case HttpRequestType::Get_AlbumInfo :       setResult_album(p_jsonObj);                 break;
            case HttpRequestType::Get_ArtistInfo :      setResult_artist(p_jsonObj);                break;

            case HttpRequestType::Get_ArtistBioText :   setResult_artist_bioText(p_jsonObj);        break;


            case HttpRequestType::GetList_My_Favorite_Playlists :       setResult_list_myFavoritePlaylists(p_jsonObj);      break;
            case HttpRequestType::GetList_My_Favorite_Albums :          setResult_list_myFavoriteAlbums(p_jsonObj);         break;
            case HttpRequestType::GetList_My_Favorite_Tracks :          setResult_list_myFavoriteTracks(p_jsonObj);         break;
            case HttpRequestType::GetList_My_Favorite_Artists :         setResult_list_myFavoriteArtists(p_jsonObj);        break;
            case HttpRequestType::GetList_My_Favorite_Videos :          setResult_list_myFavoriteVideos(p_jsonObj);         break;

            case HttpRequestType::GetList_Items_Mixes :                 setResult_list_itmes_of_mixes(p_jsonObj);           break;
            case HttpRequestType::GetList_Items_Playlist :              setResult_list_itmes_of_playlist(p_jsonObj);        break;
            case HttpRequestType::GetList_Items_Album :                 setResult_list_itmes_of_album(p_jsonObj);           break;
            case HttpRequestType::GetList_Items_Visual :                setResult_list_itmes_of_visual(p_jsonObj);           break;

            case HttpRequestType::GetAll_UserFavorites :                setResult_listAll_myFavoritesIds(p_jsonObj);        break;


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

            case HttpRequestType::Search_GetList_Track :        this->setResult_search_getList_track(p_jsonObj);    break;
            case HttpRequestType::Search_GetList_Artist :       this->setResult_search_getList_artist(p_jsonObj);   break;
            case HttpRequestType::Search_GetList_Album :        this->setResult_search_getList_album(p_jsonObj);    break;
            case HttpRequestType::Search_GetList_Playlist :     this->setResult_search_getList_playlist(p_jsonObj); break;
            case HttpRequestType::Search_GetList_Video :        this->setResult_search_getList_video(p_jsonObj);    break;
            case HttpRequestType::Search_GetList_TopHit :       this->setResult_search_getList_topHit(p_jsonObj);    break;


        }

        sender()->deleteLater();
    }


    /**
     * @brief 타이달 로그인 요청결과 처리 함수
     * @param p_jsonData QJsonObject API response
     * @see https://documenter.getpostman.com/view/448610/HXVi?version=latest#11d1026f-3789-480c-aa99-57dae2a357b1
     */
    void ProcCommon::setResult_loginTidal(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            const QString sessionId = ProcJsonEasy::getString(p_jsonObj, "sessionId");
            const QString countryCode = ProcJsonEasy::getString(p_jsonObj, "countryCode");
            const int userId = ProcJsonEasy::getInt(p_jsonObj, "userId");

            if(userId == 0 || sessionId.isEmpty() || countryCode.isEmpty()){
                emit this->failedLogin(tr("Unable to login to TIDAL service. (2)"));
                //emit this->failedLogin(tr("TIDAL 서비스에 로그인할 수 없습니다. (2)"));
            }
            else{
                // 정보 저장 (어플 사용하는 동안 필요한 정보임)
                global.user_forTidal.setLogin(sessionId, countryCode, userId, userLoginInfo.email, userLoginInfo.flagSavedLoginInfo);
                //global.user_forTidal.setUsername(userLoginInfo.email);

                // DB 정보 처리
                if(userLoginInfo.flagSavedLoginInfo){
                    this->saveLoginInfo_tidalDB(userLoginInfo);
                }
                else{
                    this->clearLoginInfo_tidalDB();
                }

                // 시그널 발생
                emit this->successLogin();
            }
        }
        else{
            emit this->failedLogin(tr("Unable to login to TIDAL service. (1)"));
            //emit this->failedLogin(tr("TIDAL 서비스에 로그인할 수 없습니다. (1)"));
        }
    }


    /**
     * @brief ProcBugsAPI::setResult_getToken
     * @param p_jsonObj
     */
    void ProcCommon::setResult_getToken(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj) == false){
            //emit this->signal_completeReq_getToken(false, tr("Tidal 서비스를 이용할 수 없습니다."));
            emit this->signal_completeReq_getToken(false, tr("Tidal Service is unavailable."));
        }
        else{
            // DB에 토큰정보 저장. (only DB)
            tidal::TidalTokenInfo tokenInfo_output;
            tokenInfo_output.access_token = ProcJsonEasy::getString(p_jsonObj, "access_token");
            tokenInfo_output.refresh_token = ProcJsonEasy::getString(p_jsonObj, "refresh_token");
            tokenInfo_output.expires_in = ProcJsonEasy::getInt(p_jsonObj, "expires_in");
            tokenInfo_output.token_type = ProcJsonEasy::getString(p_jsonObj, "token_type");

            QJsonObject par_json = ProcJsonEasy::getJsonObject(p_jsonObj, "user");
            tokenInfo_output.email = ProcJsonEasy::getString(par_json, "username");

            this->saveTokenInfo_tidalDB(tokenInfo_output);

            emit this->signal_completeReq_getToken(true, "");
        }
    }


    void ProcCommon::setResult_getRefreshToken(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj) == false){
            //emit this->signal_completeReq_getToken(false, tr("Tidal 서비스를 이용할 수 없습니다."));
            emit this->signal_completeReq_getToken(false, tr("Tidal Service is unavailable."));
            emit this->signal_completeReq_getRefreshToken(false, "");
        }
        else{
            // DB에 토큰정보 저장. (only DB)
            tidal::TidalTokenInfo tokenInfo_output;
            tokenInfo_output.access_token = ProcJsonEasy::getString(p_jsonObj, "access_token");
            tokenInfo_output.refresh_token = global.user_forTidal.get_refresh_token();
            tokenInfo_output.expires_in = ProcJsonEasy::getInt(p_jsonObj, "expires_in");
            tokenInfo_output.token_type = ProcJsonEasy::getString(p_jsonObj, "token_type");

            tokenInfo_output.email = global.user_forTidal.getUsername();

            //this->saveTokenInfo_tidalDB(tokenInfo_output);
            global.user_forTidal.set_loginState(tokenInfo_output.access_token, tokenInfo_output.refresh_token, global.user_forTidal.getCountryCode(), global.user_forTidal.getSessionId(), global.user_forTidal.getUserId(), tokenInfo_output.email);

            emit this->signal_completeReq_getRefreshToken(true, "");
        }
    }


    /**
     * @brief ProcBugsAPI::setResult_getMemberInfo
     * @param p_jsonObj
     */
    void ProcCommon::setResult_getSessionInfo(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj))
        {
            // success
            emit this->signal_completeReq_getSession("", p_jsonObj);
        }
        else
        {
            /*SqliteHelper *sqliteHelper = new SqliteHelper(this);
            QSqlError err = sqliteHelper->addConnectionLocal();

            QVariantList *list = new QVariantList();
            sqliteHelper->exec(QUERY_SEL, *list);
            if(list->size() > 0){
                sqliteHelper->exec(QUERY_DROP_TABLE);
            }

            sqliteHelper->close();
            delete sqliteHelper;*/

            emit this->signal_completeReq_getSession("error", p_jsonObj);
        }
    }


    void ProcCommon::setResult_getSubscription(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj))
        {
            // success
            emit this->signal_completeReq_getSubscription("", p_jsonObj);
        }
        else
        {
            // occur error
            Q_UNUSED(p_jsonObj);

            global.user_forTidal.setLogout();
            this->userLoginInfo = UserLoginInfo{};
            this->clearLoginInfo_tidalDB();
        }
    }


    /**
     * @brief 타이달 로그아웃 요청결과 처리 함수
     * @param p_jsonObj
     * @see https://documenter.getpostman.com/view/448610/HXVi?version=latest#98e2e430-ce2e-455e-a171-e4552195c822
     */
    void ProcCommon::setResult_logoutTidal(const QJsonObject &p_jsonObj){

        Q_UNUSED(p_jsonObj);

        global.user_forTidal.setLogout();
        this->userLoginInfo = UserLoginInfo{};
        this->clearLoginInfo_tidalDB();

        emit this->successLogout();
    }


    /**
     * @brief 장르 종류 결과처리
     * @param p_jsonObject
     */
    void ProcCommon::setResult_listGenres(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray jsonArr_list = ProcJsonEasy::getJsonArray(p_jsonObj, "array");

            for(int i = 0; i < jsonArr_list.count(); i++){
                QJsonObject tmp_jsonObj = jsonArr_list.at(i).toObject();
                QString tmp_name = ProcJsonEasy::getString(tmp_jsonObj, "name");
                QString tmp_path = ProcJsonEasy::getString(tmp_jsonObj, "path");

                if(!tmp_name.isEmpty() && !tmp_path.isEmpty()){
                    tidal::GenreInfo tmp_data;
                    tmp_data.name = tmp_name;
                    tmp_data.path = tmp_path;
                    list_genre.append(tmp_data);
                }
            }
            if(list_genre.count() > 0){
                emit signal_completeReq_getListGenre(true, "");
            }
            else{
                emit signal_completeReq_getListGenre(false, "error");
            }
        }
        else if(ProcJsonEasy::get_flagOk(p_jsonObj) == false)
        {
            /*ProcCommon *proc = new ProcCommon();
            connect(proc, &ProcCommon::signal_completeReq_getRefreshToken, this, &ProcCommon::slot_getToken);
            proc->request_tidal_getRefreshTokenAndSave();*/
            emit signal_completeReq_getListGenre(false, "error");
        }
    }


    void ProcCommon::slot_getToken(const bool flagSuccess, const QString errorMsg)
    {
        Q_UNUSED(flagSuccess)

        if(errorMsg.isEmpty())
        {
            // 전체 장르 정보를 요청함 - 공통으로 사용 (세팅 안된 경우)
            // Genre, Mood 종류 요청 (Tidal 공통사용)
            ProcCommon *procTidal = new ProcCommon();
            procTidal->request_tidal_getListGenres();
            procTidal->request_tidal_getListMoods();

            // Rose 에게 tidal session 정보를 저장
            tidal::RoseSessionInfo_forTidal sessionInfo;
            sessionInfo.TIDAL_AccessToken = global.user_forTidal.get_access_token();            //ptokenInfo.access_token;
            sessionInfo.TIDAL_RefreshToken = global.user_forTidal.get_refresh_token();          //ptokenInfo.refresh_token;
            sessionInfo.TIDAL_CountryCode = global.user_forTidal.getCountryCode();              //ptokenInfo.country_code;
            sessionInfo.TIDAL_SessionId = global.user_forTidal.getSessionId();                  //ptokenInfo.session_id;
            sessionInfo.TIDAL_UserID = global.user_forTidal.getUserId();
            sessionInfo.TIDAL_UserName = global.user_forTidal.getUsername();                    //ptokenInfo.email;
            sessionInfo.TIDAL_HighestSoundQuality = global.user_forTidal.getHeighestSound();
            sessionInfo.TIDAL_SoundQuality = global.user_forTidal.getSoundQuality();

            ProcRosePlay_withTidal *procRose = new ProcRosePlay_withTidal();
            procRose->request_set_session_info(sessionInfo);
        }
    }


    /**
     * @brief Mood & Activity 종류 결과처리
     * @param p_jsonObject
     */
    void ProcCommon::setResult_listMoods(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray jsonArr_list = ProcJsonEasy::getJsonArray(p_jsonObj, "array");

            for(int i = 0; i < jsonArr_list.count(); i++){
                QJsonObject tmp_jsonObj = jsonArr_list.at(i).toObject();
                QString tmp_name = ProcJsonEasy::getString(tmp_jsonObj, "name");
                QString tmp_path = ProcJsonEasy::getString(tmp_jsonObj, "path");

                if(!tmp_name.isEmpty() && !tmp_path.isEmpty()){
                    tidal::MoodInfo tmp_data;
                    tmp_data.name = tmp_name;
                    tmp_data.path = tmp_path;
                    list_mood.append(tmp_data);
                }
            }

            if(list_mood.count() > 0){
                emit signal_completeReq_getListMoods(true, "");
            }
            else{
                emit signal_completeReq_getListMoods(false, "error");
            }
        }
        else if(ProcJsonEasy::get_flagOk(p_jsonObj) == false){
            emit signal_completeReq_getListMoods(false, "error");
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

        int offset = ProcJsonEasy::getInt(p_jsonObj, "offset");
        int limit = ProcJsonEasy::getInt(p_jsonObj, "limit");
        int totalNumberOfItems = ProcJsonEasy::getInt(p_jsonObj, "total");

        return (totalNumberOfItems > offset + limit) ? false : true;
    }


    /**
     * @brief ProcCommon::setResult_list_mixeslists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_mixeslists(const QJsonObject &p_jsonObj){

        QList<tidal::PlaylistItemData> list_output;
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "array");
        bool flag_lastPage = true;      // default

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i = 0; i < jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                PlaylistItemData tmp_data = ConvertData::make_mixesData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = jsonArr_item.count();
                list_output.append(tmp_data);
            }
        }

        emit this->completeReq_list_mixes(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_playlists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_playlists(const QJsonObject &p_jsonObj){

        QList<tidal::PlaylistItemData> list_output;
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");
        bool flag_lastPage = true;      // default

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                PlaylistItemData tmp_data = ConvertData::make_playlistData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_playlists(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_albums
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_albums(const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << strJson;*/

        QList<tidal::AlbumItemData> list_output;
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                AlbumItemData tmp_data = ConvertData::make_albumData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_albums(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_albums
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_viuals(const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << strJson;*/

        int video_cnt = 0;

        QList<tidal::AlbumItemData> list_output;
        QJsonObject jsonObj_item = ProcJsonEasy::getJsonObject(p_jsonObj, "albums");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_item, "items");

        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            int maxNumber = jsonArr_item.count();
            if(this->visual_album_offset == false){                
                int tmp = 0;
                int save[50] = {};
                int count = 0;
                int isSame = 0;
                srand(time(0));

                while (count < maxNumber) {
                    isSame = 0;
                    tmp = rand() % maxNumber;
                    for (int i = 0; i < count; i++) {
                        if (tmp == save[i]) {
                            isSame = 1;
                            break;
                        }
                    }
                    if (isSame == 0) {
                        save[count] = tmp;
                        count++;
                    }
                }

                for(int i = 0; i < jsonArr_item.count(); i++){
                    int idx = save[i];
                    QJsonObject tmp_json = jsonArr_item.at(idx).toObject();

                    // 정보 담을 struct
                    AlbumItemData tmp_data = ConvertData::make_albumData_fromTidalJsonObj(tmp_json);
                    tmp_data.totalCount = ProcJsonEasy::getInt(jsonObj_item, "totalNumberOfItems");
                    video_cnt = ProcJsonEasy::getInt(tmp_json, "numberOfVideos");
                    if(video_cnt > 0){
                        tmp_data.isVideo = true;
                    }
                    tmp_data.visualAlbum = true;
                    list_output.append(tmp_data);
                }

                QJsonObject tmpObj = QJsonObject();
                tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_item, "limit"));
                tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_item, "offset"));
                tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_item, "totalNumberOfItems"));

                flag_lastPage = this->get_flag_lastPage(tmpObj);
            }
            else{

                for(int i = 0; i < jsonArr_item.count(); i++){
                    QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                    // 정보 담을 struct
                    AlbumItemData tmp_data = ConvertData::make_albumData_fromTidalJsonObj(tmp_json);
                    tmp_data.totalCount = ProcJsonEasy::getInt(jsonObj_item, "totalNumberOfItems");
                    video_cnt = ProcJsonEasy::getInt(tmp_json, "numberOfVideos");
                    if(video_cnt > 0){
                        tmp_data.isVideo = true;
                    }
                    tmp_data.visualAlbum = true;
                    list_output.append(tmp_data);
                }

                QJsonObject tmpObj = QJsonObject();
                tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_item, "limit"));
                tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_item, "offset"));
                tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_item, "totalNumberOfItems"));

                flag_lastPage = this->get_flag_lastPage(tmpObj);
            }
        }

        emit this->completeReq_list_visuals(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_artists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_artists(const QJsonObject &p_jsonObj){

        QList<tidal::ArtistItemData> list_output;
        QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(p_jsonObj, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = tmp_jsonArr.at(i).toObject();

                // 정보 담을 struct
                ArtistItemData tmp_data = ConvertData::make_artistData_fromTidalJsonObj(tmp_json);
                tmp_data.artist_total_cnt = ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_artists(list_output, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_tracks
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_tracks(const QJsonObject &p_jsonObj){

        QList<tidal::TrackItemData> list_output;
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            if(jsonArr_item.count() > 0){
                for(int i = 0; i < jsonArr_item.count(); i++){
                    QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                    // 정보 담을 struct
                    tidal::TrackItemData tmp_data = ConvertData::make_trackData_fromTidalJsonObj(tmp_json);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems");
                    list_output.append(tmp_data);
                }

                QJsonObject tmpObj = QJsonObject();
                tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
                tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
                tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

                flag_lastPage = this->get_flag_lastPage(tmpObj);
            }
            else{
                QJsonObject tmp_json = p_jsonObj;
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                tidal::TrackItemData tmp_data = ConvertData::make_trackData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = 1;
                list_output.append(tmp_data);
            }
        }

        emit this->completeReq_list_tracks(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_videos
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_videos(const QJsonObject &p_jsonObj){

        QList<tidal::VideoItemData> list_output;
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            if(jsonArr_item.count() > 0){
                for(int i = 0; i < jsonArr_item.count(); i++){
                    QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                    // 정보 담을 struct
                    tidal::VideoItemData tmp_data = ConvertData::make_videoData_fromTidalJsonObj(tmp_json);
                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems");
                    list_output.append(tmp_data);
                }

                QJsonObject tmpObj = QJsonObject();
                tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
                tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
                tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

                flag_lastPage = this->get_flag_lastPage(tmpObj);
            }
            else{
                QJsonObject tmp_json = p_jsonObj;
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                tidal::VideoItemData tmp_data = ConvertData::make_videoData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = 1;
                list_output.append(tmp_data);
            }
        }

        emit this->completeReq_list_videos(list_output, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_playlist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_playlist(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            PlaylistItemData data_playlist = ConvertData::make_playlistData_fromTidalJsonObj(p_jsonObj);
            emit this->completeReq_playlist(data_playlist);
        }
        else{
            PlaylistItemData data_playlist;
            emit this->completeReq_playlist(data_playlist);
        }
    }


    /**
     * @brief ProcCommon::setResult_album
     * @param p_jsonObj
     */
    void ProcCommon::setResult_album(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            AlbumItemData data_album = ConvertData::make_albumData_fromTidalJsonObj(p_jsonObj);
            emit this->completeReq_album(data_album);
        }
        else{
            AlbumItemData data_album;
            data_album.title = "error";

            emit this->completeReq_album(data_album);
        }
    }


    /**
     * @brief ProcCommon::setResult_artist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_artist(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            ArtistItemData data_artist = ConvertData::make_artistData_fromTidalJsonObj(p_jsonObj);
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
    void ProcCommon::setResult_list_itmes_of_mixes(const QJsonObject &p_jsonObj){

        QList<tidal::TrackItemData> list_output;
        QJsonArray jsonArr_item;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(tmp_jsonArr.at(i).toObject(), "item");
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                tidal::TrackItemData tmp_data = ConvertData::make_trackData_fromTidalJsonObj(tmp_json);
                list_output.append(tmp_data);
            }

            flag_lastPage = this->get_flag_lastPage(p_jsonObj);
        }

        emit this->completeReq_list_items_of_mixes(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_itmes_of_playlist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_itmes_of_playlist(const QJsonObject &p_jsonObj){

        QList<tidal::TrackItemData> list_output;
        QJsonArray jsonArr_item;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(tmp_jsonArr.at(i).toObject(), "item");
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                tidal::TrackItemData tmp_data = ConvertData::make_trackData_fromTidalJsonObj(tmp_json);
                list_output.append(tmp_data);
            }

            flag_lastPage = this->get_flag_lastPage(p_jsonObj);
        }

        emit this->completeReq_list_items_of_playlist(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_itmes_of_album(const QJsonObject &p_jsonObj){

        QList<tidal::TrackItemData> list_output;
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            for(int i=0 ; i<jsonArr_item.count() ; i++){
                //QJsonObject tmp_json = ProcJsonEasy::getJsonObject(jsonArr_item.at(i).toObject(), "item");
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                tidal::TrackItemData tmp_data = ConvertData::make_trackData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            flag_lastPage = this->get_flag_lastPage(p_jsonObj);
        }

        emit this->completeReq_list_items_of_album(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_itmes_of_visual(const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << strJson;*/

        QList<tidal::TrackItemData> list_output;
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");
        QJsonArray jsonArr_send;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            for(int i = 0; i < jsonArr_item.count(); i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(jsonArr_item.at(i).toObject(), "item");
                jsonArr_send.append(tmp_json);

                // 정보 담을 struct
                tidal::TrackItemData tmp_data = ConvertData::make_visualData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = jsonArr_item.count();
                list_output.append(tmp_data);
            }

            flag_lastPage = this->get_flag_lastPage(p_jsonObj);
        }

        emit this->completeReq_list_items_of_visual(list_output, jsonArr_send, flag_lastPage);
    }


    // about user's created playlist ------------------------------------------------------------------------------------------

    /**
     * @brief ProcCommon::setResult_list_myPlaylists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myCreatedPlaylists(const QJsonObject &p_jsonObj, QJsonObject jsonOb_opt){

        QList<tidal::PlaylistItemData> list_output;
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                PlaylistItemData tmp_data = ConvertData::make_playlistData_fromTidalJsonObj(tmp_json);
                tmp_data.flagCreatedByUser = true;      // 사용자가 만든 playlist
                tmp_data.creatorName = "ME";            // 사용자가 만든 playlist
                tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

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
        print_tidal_func();
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

        if(!playlist_uuid_target.isEmpty() && list_track_id.count() > 0){
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

        QString uuid = "";
        if(p_jsonObj.contains("uuid")){
            uuid = ProcJsonEasy::getString(p_jsonObj, "uuid");
        }

        emit this->completeReq_create_myPlaylist(uuid);
    }


    void ProcCommon::setResult_addTracks_myPlaylsit(const QJsonObject &p_jsonObj){

        QJsonObject sendObj = p_jsonObj;
        emit this->completeReq_addTracks_myPlaylist(true);
    }


    // about my favorite collection ------------------------------------------------------------------------------------------

    /**
     * @brief ProcCommon::setResult_list_myFavoritePlaylists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myFavoritePlaylists(const QJsonObject &p_jsonObj){

        QList<tidal::PlaylistItemData> list_output;
        QJsonArray jsonArr_item;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(tmp_jsonArr.at(i).toObject(), "item");
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                PlaylistItemData tmp_data = ConvertData::make_playlistData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myFavoritePlaylists(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_myAlbums
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myFavoriteAlbums(const QJsonObject &p_jsonObj){

        QList<tidal::AlbumItemData> list_output;
        QJsonArray jsonArr_item;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(tmp_jsonArr.at(i).toObject(), "item");
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                AlbumItemData tmp_data = ConvertData::make_albumData_fromTidalJsonObj(tmp_json);
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myFavoriteAlbums(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_myTracks
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myFavoriteTracks(const QJsonObject &p_jsonObj){

        //print_tidal_func();

        QList<tidal::TrackItemData> list_output;
        QJsonArray jsonArr_item;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(tmp_jsonArr.at(i).toObject(), "item");
                jsonArr_item.append(tmp_json);

                // 정보 담을 struct
                TrackItemData tmp_data = ConvertData::make_trackData_fromTidalJsonObj(tmp_json);
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myFavoriteTracks(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_myFavoriteArtists
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myFavoriteArtists(const QJsonObject &p_jsonObj){

        QList<tidal::ArtistItemData> list_output;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(tmp_jsonArr.at(i).toObject(), "item");

                // 정보 담을 struct
                ArtistItemData tmp_data = ConvertData::make_artistData_fromTidalJsonObj(tmp_json);
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myFavoriteArtists(list_output, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_list_myFavoriteVideos
     * @param p_jsonObj
     */
    void ProcCommon::setResult_list_myFavoriteVideos(const QJsonObject &p_jsonObj){

        QList<tidal::VideoItemData> list_output;
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QJsonArray tmp_jsonArr = ProcJsonEasy::getJsonArray(p_jsonObj, "items");

            for(int i=0 ; i<tmp_jsonArr.count() ; i++){
                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(tmp_jsonArr.at(i).toObject(), "item");

                // 정보 담을 struct
                VideoItemData tmp_data = ConvertData::make_videoData_fromTidalJsonObj(tmp_json);
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(p_jsonObj, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(p_jsonObj, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(p_jsonObj, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_myFavoriteVideos(list_output, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_listAll_myFavoritesIds
     * @param p_jsonObj
     */
    void ProcCommon::setResult_listAll_myFavoritesIds(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            emit this->completeReq_listAll_myFavoritesIds(p_jsonObj);
        }
        else{
            QJsonObject tmpJson;
            emit this->completeReq_listAll_myFavoritesIds(tmpJson);
        }
    }


    // Searching ---------------------------------------------------------------------------------------------------------

    /**
     * @brief ProcCommon::setResult_search_getList_track
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_track(const QJsonObject &p_jsonObj){

        //print_tidal_func();

        QList<tidal::TrackItemData> list_output;
        QJsonObject jsonObj_track = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_track, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                tidal::TrackItemData tmp_data = ConvertData::make_trackData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = ProcJsonEasy::getInt(jsonObj_track, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_track, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_track, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_track, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_tracks(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_search_getList_artist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_artist(const QJsonObject &p_jsonObj){

        QList<tidal::ArtistItemData> list_output;
        QJsonObject jsonObj_artist = ProcJsonEasy::getJsonObject(p_jsonObj, "artists");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_artist, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                tidal::ArtistItemData tmp_data = ConvertData::make_artistData_fromTidalJsonObj(tmp_json);
                tmp_data.artist_total_cnt = ProcJsonEasy::getInt(jsonObj_artist, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_artist, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_artist, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_artist, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_artists(list_output, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_search_getList_album
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_album(const QJsonObject &p_jsonObj){

        QList<tidal::AlbumItemData> list_output;
        QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(p_jsonObj, "albums");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_album, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                tidal::AlbumItemData tmp_data = ConvertData::make_albumData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = ProcJsonEasy::getInt(jsonObj_album, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_album, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_album, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_album, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_albums(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_search_getList_playlist
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_playlist(const QJsonObject &p_jsonObj){

        QList<tidal::PlaylistItemData> list_output;
        QJsonObject jsonObj_playlist = ProcJsonEasy::getJsonObject(p_jsonObj, "playlists");
        QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_playlist, "items");
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                tidal::PlaylistItemData tmp_data = ConvertData::make_playlistData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = ProcJsonEasy::getInt(jsonObj_playlist, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_playlist, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_playlist, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_playlist, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_playlists(list_output, jsonArr_item, flag_lastPage);
    }


    /**
     * @brief ProcCommon::setResult_search_getList_video
     * @param p_jsonObj
     */
    void ProcCommon::setResult_search_getList_video(const QJsonObject &p_jsonObj){

        QList<tidal::VideoItemData> list_output;
        QJsonArray jsonArr_item = QJsonArray();
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            QJsonObject jsonObj_video = ProcJsonEasy::getJsonObject(p_jsonObj, "videos");
            jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_video, "items");

            for(int i = 0; i < jsonArr_item.count(); i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                tidal::VideoItemData tmp_data = ConvertData::make_videoData_fromTidalJsonObj(tmp_json);
                tmp_data.totalCount = ProcJsonEasy::getInt(jsonObj_video, "totalNumberOfItems");
                list_output.append(tmp_data);
            }

            QJsonObject tmpObj = QJsonObject();
            tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_video, "limit"));
            tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_video, "offset"));
            tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_video, "totalNumberOfItems"));

            flag_lastPage = this->get_flag_lastPage(tmpObj);
        }

        emit this->completeReq_list_videos(list_output, flag_lastPage);
    }


    void ProcCommon::setResult_search_getList_topHit(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            QList<tidal::ArtistItemData> list_topHit;
            QList<tidal::TrackItemData> list_track;
            QList<tidal::ArtistItemData> list_artist;
            QList<tidal::AlbumItemData> list_album;
            QList<tidal::PlaylistItemData> list_playlist;
            QList<tidal::VideoItemData> list_video;

            QJsonArray jsonArr_item = QJsonArray();
            bool flag_lastPage = true;

            if(p_jsonObj.contains("topHit")){
                QJsonObject jsonObj_topHit = ProcJsonEasy::getJsonObject(p_jsonObj, "topHit");

                if(!jsonObj_topHit.isEmpty()){
                    QJsonObject value_Obj = ProcJsonEasy::getJsonObject(jsonObj_topHit, "value");

                    tidal::ArtistItemData tmp_data;
                    tmp_data.name = ProcJsonEasy::getString(value_Obj, "name");
                    tmp_data.image = tidal::ConvertData::convertImagePath(ProcJsonEasy::getString(value_Obj, "picture"), tidal::ConvertData::ImageSize_Square::Size_750x750);
                    tmp_data.id = ProcJsonEasy::getInt(value_Obj, "id");

                    list_topHit.append(tmp_data);
                }
            }
            emit this->completeReq_list_topHit(list_topHit, flag_lastPage);

            flag_lastPage = true;
            jsonArr_item = QJsonArray();
            if(p_jsonObj.contains("tracks")){
                QJsonObject jsonObj_track = ProcJsonEasy::getJsonObject(p_jsonObj, "tracks");
                jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_track, "items");

                if(jsonArr_item.size() > 0){
                    for(int i = 0; i < jsonArr_item.count(); i++){
                        QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                        // 정보 담을 struct
                        tidal::TrackItemData tmp_data = ConvertData::make_trackData_fromTidalJsonObj(tmp_json);
                        tmp_data.totalCount = ProcJsonEasy::getInt(jsonObj_track, "totalNumberOfItems");
                        list_track.append(tmp_data);
                    }

                    QJsonObject tmpObj = QJsonObject();
                    tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_track, "limit"));
                    tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_track, "offset"));
                    tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_track, "totalNumberOfItems"));

                    flag_lastPage = this->get_flag_lastPage(jsonObj_track);
                }
            }
            emit this->completeReq_list_tracks(list_track, jsonArr_item, flag_lastPage);

            flag_lastPage = true;
            jsonArr_item = QJsonArray();
            if(p_jsonObj.contains("artists")){
                QJsonObject jsonObj_artist = ProcJsonEasy::getJsonObject(p_jsonObj, "artists");
                jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_artist, "items");

                if(jsonArr_item.size() > 0){
                    for(int i = 0; i < jsonArr_item.count(); i++){
                        QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                        // 정보 담을 struct
                        tidal::ArtistItemData tmp_data = ConvertData::make_artistData_fromTidalJsonObj(tmp_json);
                        tmp_data.artist_total_cnt = ProcJsonEasy::getInt(jsonObj_artist, "totalNumberOfItems");
                        list_artist.append(tmp_data);
                    }

                    QJsonObject tmpObj = QJsonObject();
                    tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_artist, "limit"));
                    tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_artist, "offset"));
                    tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_artist, "totalNumberOfItems"));

                    flag_lastPage = this->get_flag_lastPage(tmpObj);
                }
            }
            emit this->completeReq_list_artists(list_artist, flag_lastPage);

            flag_lastPage = true;
            jsonArr_item = QJsonArray();
            if(p_jsonObj.contains("albums")){
                QJsonObject jsonObj_album = ProcJsonEasy::getJsonObject(p_jsonObj, "albums");
                jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_album, "items");

                if(jsonArr_item.size() > 0){
                    for(int i = 0; i < jsonArr_item.count(); i++){
                        QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                        // 정보 담을 struct
                        tidal::AlbumItemData tmp_data = ConvertData::make_albumData_fromTidalJsonObj(tmp_json);
                        tmp_data.totalCount = ProcJsonEasy::getInt(jsonObj_album, "totalNumberOfItems");
                        list_album.append(tmp_data);
                    }

                    QJsonObject tmpObj = QJsonObject();
                    tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_album, "limit"));
                    tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_album, "offset"));
                    tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_album, "totalNumberOfItems"));

                    flag_lastPage = this->get_flag_lastPage(tmpObj);
                }
            }
            emit this->completeReq_list_albums(list_album, jsonArr_item, flag_lastPage);

            flag_lastPage = true;
            jsonArr_item = QJsonArray();
            if(p_jsonObj.contains("playlists")){
                QJsonObject jsonObj_playlist = ProcJsonEasy::getJsonObject(p_jsonObj, "playlists");
                jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_playlist, "items");

                if(jsonArr_item.size() > 0){
                    for(int i = 0; i < jsonArr_item.count(); i++){
                        QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                        // 정보 담을 struct
                        tidal::PlaylistItemData tmp_data = ConvertData::make_playlistData_fromTidalJsonObj(tmp_json);
                        tmp_data.totalCount = ProcJsonEasy::getInt(jsonObj_playlist, "totalNumberOfItems");
                        list_playlist.append(tmp_data);
                    }

                    QJsonObject tmpObj = QJsonObject();
                    tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_playlist, "limit"));
                    tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_playlist, "offset"));
                    tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_playlist, "totalNumberOfItems"));

                    flag_lastPage = this->get_flag_lastPage(tmpObj);
                    }
            }
            emit this->completeReq_list_playlists(list_playlist, jsonArr_item, flag_lastPage);

            flag_lastPage = true;
            jsonArr_item = QJsonArray();
            if(p_jsonObj.contains("videos")){
                QJsonObject jsonObj_video = ProcJsonEasy::getJsonObject(p_jsonObj, "videos");
                jsonArr_item = ProcJsonEasy::getJsonArray(jsonObj_video, "items");

                if(jsonArr_item.size() > 0){
                    for(int i = 0; i < jsonArr_item.count(); i++){
                        QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                        // 정보 담을 struct
                        tidal::VideoItemData tmp_data = ConvertData::make_videoData_fromTidalJsonObj(tmp_json);
                        tmp_data.totalCount = ProcJsonEasy::getInt(jsonObj_video, "totalNumberOfItems");
                        list_video.append(tmp_data);
                    }

                    QJsonObject tmpObj = QJsonObject();
                    tmpObj.insert("limit", ProcJsonEasy::getInt(jsonObj_video, "limit"));
                    tmpObj.insert("offset", ProcJsonEasy::getInt(jsonObj_video, "offset"));
                    tmpObj.insert("total", ProcJsonEasy::getInt(jsonObj_video, "totalNumberOfItems"));

                    flag_lastPage = this->get_flag_lastPage(tmpObj);
                }
            }
            emit this->completeReq_list_videos(list_video, flag_lastPage);
        }
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
    tidal::GenreInfo ProcCommon::getGenreInfo(int idx){

        if(idx < 0){
            return tidal::GenreInfo{};
        }
        else if(idx >= list_genre.length()){
            return tidal::GenreInfo{};
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
    tidal::MoodInfo ProcCommon::getMoodInfo(int idx){

        if(idx < 0){
            return tidal::MoodInfo{};
        }
        else if(idx >= list_mood.length()){
            return tidal::MoodInfo{};
        }
        else {
            return list_mood.at(idx);
        }
//        return tidal::MoodOption{};
    }


}
