#include "BugsMain.h"
#include "ConvertData_forBugs.h"
#include "DataCommon_Bugs.h"
#include "ProcBugsAPI.h"

#include <QMetaEnum>
#include "common/global.h"
#include <QDebug>
//#include <QException>
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include <common/sqlitehelper.h>
#include "bugs/bugs_struct.h"


namespace bugs {

//    const QString QUERY_CREATE_TABLE = " CREATE TABLE User_Bugs (access_token VARCHAR(255), refresh_token VARCHAR(255), expires_in int, scope VARCHAR(20), token_type VARCHAR(20)) ";
    const QString QUERY_CREATE_TABLE = " CREATE TABLE IF NOT EXISTS User_Bugs (access_token VARCHAR(2047), refresh_token VARCHAR(2047), expires_in int, scope VARCHAR(255), token_type VARCHAR(255)) ";
    const QString QUERY_DROP_TABLE = " DROP TABLE User_Bugs ";
    const QString QUERY_DEL = " DELETE FROM User_Bugs ";
    const QString QUERY_SEL = " SELECT * FROM User_Bugs ";

//    QString token_type;

    /**
     * @brief ProcBugsAPI::ProcBugsAPI
     */
    ProcBugsAPI::ProcBugsAPI(QWidget *parent) : QObject(parent) {
        this->linker = Linker::getInstance();//c230421
    }


    /**
     * @brief ProcBugsAPI::~ProcBugsAPI
     */
    ProcBugsAPI::~ProcBugsAPI(){
    }



    /**
     * @brief 타이달 로그인 정보를 sqllite3 정보로 저장함
     * @param username : 타이달 사용자 email 정보
     * @param password : 타이달 사용자 비밀번호 정보
     */
    void ProcBugsAPI::saveTokenInfo_bugsDB(BugsTokenInfo& p_bugsTokenInfo){

        QString strQuery_insert = " INSERT INTO User_Bugs (access_token, refresh_token, expires_in, scope, token_type) VALUES ";
        strQuery_insert.append( QString("('%1', '%2', %3, '%4', '%5')")
                                .arg(p_bugsTokenInfo.access_token)
                                .arg(p_bugsTokenInfo.refresh_token)
                                .arg(p_bugsTokenInfo.expires_in)
                                .arg(p_bugsTokenInfo.scope)
                                .arg(p_bugsTokenInfo.token_type)
                                );

        // 항상 테이블 생성을 체크하고, 삭제처리 한다. for 초기화 (항상 레코드 최대 1개 유지)
        this->procQueryList(QList<QString>{ QUERY_CREATE_TABLE, QUERY_DEL, strQuery_insert });

    }

    /**
     * @brief 타이달 로그인 정보를 clear 처리함
     */
    void ProcBugsAPI::clearTablesInfo_bugsDB(){
        // 항상 테이블 생성을 체크하고, 삭제처리 한다.
        this->procQueryList(QList<QString>{ QUERY_CREATE_TABLE, QUERY_DEL });
    }


    /**
     * @brief 로그인 정보를 SELECT 해서 userLoginInfo에 세팅한다.
     * @param userLoginInfo
     */
    BugsTokenInfo ProcBugsAPI::getTokenInfo_bugsDB(){

        BugsTokenInfo output_tokenInfo;

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
                output_tokenInfo.scope = map["scope"].toString();
                output_tokenInfo.token_type = map["token_type"].toString();
                break;
            }
        }

        if(output_tokenInfo.access_token.isEmpty())
        {
            sqliteHelper->exec(QUERY_DROP_TABLE);
        }

        sqliteHelper->close();
        delete sqliteHelper;

        return output_tokenInfo;
    }


    /**
     * @brief Query 를 순차적으로 실행 처리한다.
     * @param list_query 쿼리 문자열 list
     */
    void ProcBugsAPI::procQueryList(const QList<QString> &list_query){

        bool flagOk;
        SqliteHelper *sqliteHelper = new SqliteHelper(this);
        QSqlError err = sqliteHelper->addConnectionLocal();

        for(int i=0 ; i<list_query.length() ; i++){
            sqliteHelper->exec(list_query.at(i));
             flagOk = sqliteHelper->exec(list_query.at(i));

        }

        sqliteHelper->close();
        delete sqliteHelper;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Request API
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief ProcBugsAPI::get_headerForRequest_userAgent
     * @return
     */
    QString ProcBugsAPI::get_headerForRequest_userAgent(){
        bugs::BugsReqHeader reqHeader;
        reqHeader.client = global.BUGS_API_HEADER___client;
        reqHeader.app_name = global.BUGS_API_HEADER___appName;
        reqHeader.app_version = global.BUGS_API_HEADER___versionPC;
//        reqHeader.platform_name = global.BUGS_API_HEADER___platform_name__PREFIX + global.device.getDeviceName();
        reqHeader.platform_name = global.device.getDeviceRoseToken();
        reqHeader.platform_version = global.device.getDeviceVersion();
        reqHeader.pc_model_name = "PC";
        reqHeader.pc_model_comp = "COMPANY";

        return ProcBugsAPI::makeStr_header_userAgent(reqHeader);
    }

    /**
     * @brief 문자열 만들기 - userAgent
     * @param reqHeader
     * @return
     */
    QString ProcBugsAPI::makeStr_header_userAgent(bugs::BugsReqHeader& p_reqHeader){
        QString output ;
        output += p_reqHeader.client;
        output += "|" + p_reqHeader.app_name;
        output += "|" + p_reqHeader.app_version;
        output += "|" + p_reqHeader.platform_name;
        output += "|" + p_reqHeader.platform_version;
        output += "|" + p_reqHeader.pc_model_name;
        output += "|" + p_reqHeader.pc_model_comp;
        return output;
    }


    /**
     * @brief ProcBugsAPI::request_bugs_getToken
     * @param p_login_authCode
     */
    void ProcBugsAPI::request_bugs_getTokenAndSave(){//로그인정보로 벅스 서버로 부터 토큰을 가져옮

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("grant_type", "authorization_code");
        params.addQueryItem("client_secret", global.bugs_client_secret);
        params.addQueryItem("code", global.user_forBugs.get_login_authCode());

        network->request_forBugs(ProcBugsAPI::HttpRequestType::Get_Token
                                 , QString("%1/oauth/%2/token").arg(global.bugs_auth_url).arg(global.bugs_client_id)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Post
                                 , false);
    }


    /**
     * @brief Bugs API - 회원 정보 요청
     */
    void ProcBugsAPI::request_bugs_getMemberInfo(){
        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());

        network->request_forBugs(ProcBugsAPI::HttpRequestType::Get_MemberInfo
                                 , QString("%1/%2/member/info").arg(global.bugsAPI_url).arg(global.bugsAPI_version)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }


    /**
     * @brief 벅스 로그아웃 처리
     */
    void ProcBugsAPI::request_logout(){
        global.user_forBugs.set_logoutState();
        this->clearTablesInfo_bugsDB();        
    }



    // All List of Genres ---------------------------------------------------------------------------------------------------



    /**
     * @brief HTTP 요청 - 타이달 Genres 종류 전체반환 API
     * @details - @see list_genre 변수에 데이터가 없는 경우에만 내부 동작한다. (최초 1번만 세팅하기 위함)
     */
    void ProcBugsAPI::request_bugs_getListGenres(){

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());

        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Genres
                                 , QString("%1/%2/genre").arg(global.bugsAPI_url).arg(global.bugsAPI_version)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);

    }


    /**
     * @brief Theme 테마 리스트 조회
     */
    void ProcBugsAPI::request_bugs_getListThemes(){

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());

        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Theme
                                 , QString("%1/%2/theme").arg(global.bugsAPI_url).arg(global.bugsAPI_version)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);

    }


    /**
     * @brief theme 내의 tag 목록 요청
     * @param theme_code
     */
    void ProcBugsAPI::request_bugs_getListTags_in_theme(const QString theme_code){
        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());

        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Tags_inTheme
                                 , QString("%1/%2/theme/%3/tag").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(theme_code)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }



    // Track ---------------------------------------------------------------------------------------------------

    /**
     * @brief ProcBugsAPI::request_tidal_getList_tracks
     * @param api_subPath
     * @param size
     * @param page
     */
    void ProcBugsAPI::request_bugs_getList_tracks(const QString api_subPath, const int size, const int page){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        if(size > 0){
            params.addQueryItem("size", QString("%1").arg(size));
        }
        if(page > 0){
            params.addQueryItem("page", QString("%1").arg(page));
        }
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Tracks
                                 , QString("%1/%2/%3").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(api_subPath)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);


    }


    /**
     * @brief 내가 좋아한 음악 - 트랙정보 반환
     * @param size
     * @param page
     * @param sortOpt
     */
    void ProcBugsAPI::request_bugs_getList_tracks_of_myMusic(const int size, const int page, const BugsMyMusic_Sort sortOpt){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        if(size > 0){
            params.addQueryItem("size", QString("%1").arg(size));
        }
        if(page > 0){
            params.addQueryItem("page", QString("%1").arg(page));
        }
        params.addQueryItem("sort", QVariant::fromValue(sortOpt).toString().toLower());

        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Tracks
                                 , QString("%1/%2/mymusic/like/track").arg(global.bugsAPI_url).arg(global.bugsAPI_version)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }


    /**
     * @brief 아티스트의 트랙목록 요청
     * @param artist_id
     * @param size
     * @param page
     * @param sortOpt
     */
    void ProcBugsAPI::request_bugs_getList_tracks_of_artist(const int artist_id, const int size, const int page, const BugsArtist_TrackFilter filterOpt, const BugsAritst_TrackSort sortOpt){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        params.addQueryItem("size", QString("%1").arg(size));
        params.addQueryItem("page", QString("%1").arg(page));
        params.addQueryItem("filter", QVariant::fromValue(filterOpt).toString());
        params.addQueryItem("sort", QVariant::fromValue(sortOpt).toString().toUpper());

        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Tracks
                                 , QString("%1/%2/artist/%3/track").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(artist_id)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }



    /**
     * @brief ProcBugsAPI::request_bugs_getList_albums_of_artist
     * @param artist_id
     * @param size
     * @param page
     * @param filterOpt
     */
    void ProcBugsAPI::request_bugs_getList_albums_of_artist(const int artist_id, const int size, const int page, const BugsArtist_Albumfilter filterOpt){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        params.addQueryItem("size", QString("%1").arg(size));
        params.addQueryItem("page", QString("%1").arg(page));
        params.addQueryItem("filter", QVariant::fromValue(filterOpt).toString().toUpper());

        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Albums
                                 , QString("%1/%2/artist/%3/album").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(artist_id)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }



    // Album ---------------------------------------------------------------------------------------------------

    /**
     * @brief ProcBugsAPI::request_tidal_getList_tracks
     * @param api_subPath
     * @param size
     * @param page
     */
    void ProcBugsAPI::request_bugs_getList_albums(const QString api_subPath, const int size, const int page){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        if(size > 0){
            params.addQueryItem("size", QString("%1").arg(size));
        }
        if(page > 0){
            params.addQueryItem("page", QString("%1").arg(page));
        }
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Albums
                                 , QString("%1/%2/%3").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(api_subPath)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , true);
    }

    /**
     * @brief 내가 좋아한 음악 - 앨범 정보 반환
     * @param size
     * @param page
     * @param sortOpt
     */
    void ProcBugsAPI::request_bugs_getList_albums_of_myMusic(const int size, const int page, const BugsMyMusic_Sort sortOpt){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        if(size > 0){
            params.addQueryItem("size", QString("%1").arg(size));
        }
        if(page > 0){
            params.addQueryItem("page", QString("%1").arg(page));
        }
        params.addQueryItem("sort", QVariant::fromValue(sortOpt).toString().toLower());
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Albums
                                 , QString("%1/%2/mymusic/like/album").arg(global.bugsAPI_url).arg(global.bugsAPI_version)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }




    // Album ---------------------------------------------------------------------------------------------------

    /**
     * @brief ProcBugsAPI::request_bugs_getList_artists_of_myMusic
     * @param size
     * @param page
     * @param sortOpt
     */
    void ProcBugsAPI::request_bugs_getList_artists_of_myMusic(const int size, const int page, const BugsMyMusic_Sort sortOpt){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        if(size > 0){
            params.addQueryItem("size", QString("%1").arg(size));
        }
        if(page > 0){
            params.addQueryItem("page", QString("%1").arg(page));
        }
        params.addQueryItem("sort", QVariant::fromValue(sortOpt).toString().toLower());
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Artists
                                 , QString("%1/%2/mymusic/like/artist").arg(global.bugsAPI_url).arg(global.bugsAPI_version)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }




    // Video ---------------------------------------------------------------------------------------------------

    /**
     * @brief ProcBugsAPI::request_tidal_getList_videos
     * @param api_subPath
     * @param size
     * @param page
     */
    void ProcBugsAPI::request_bugs_getList_videos(const QString api_subPath, const int size, const int page){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        if(size > 0){
            params.addQueryItem("size", QString("%1").arg(size));
        }
        if(page > 0){
            params.addQueryItem("page", QString("%1").arg(page));
        }
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Videos
                                 , QString("%1/%2/%3").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(api_subPath)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }






    // My Album ---------------------------------------------------------------------------------------------------

    /**
     * @brief ProcBugsAPI::request_bugs_getList_my_albums
     * @param size
     * @param page
     */
    void ProcBugsAPI::request_bugs_getList_my_albums(const int size, const int page){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        if(size > 0){
            params.addQueryItem("size", QString("%1").arg(size));
        }
        if(page > 0){
            params.addQueryItem("page", QString("%1").arg(page));
        }
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_My_Albums
                                 , QString("%1/%2/myalbum").arg(global.bugsAPI_url).arg(global.bugsAPI_version)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);


    }


    // PD_Album ---------------------------------------------------------------------------------------------------

    /**
     * @brief ProcBugsAPI::request_bugs_getList_pd_albums
     * @param size
     * @param page
     * @param sortOpt
     * @param tag_ids
     */
    void ProcBugsAPI::request_bugs_getList_pd_albums(const int size, const int page, const BugsPDAlbum_Sort sortOpt, const QString tag_ids){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        if(size > 0){
            params.addQueryItem("size", QString("%1").arg(size));
        }
        if(page > 0){
            params.addQueryItem("page", QString("%1").arg(page));
        }
        params.addQueryItem("sort", QVariant::fromValue(sortOpt).toString().toUpper());
        if(!tag_ids.isEmpty()){
            params.addQueryItem("tag_ids", tag_ids);
        }
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_PD_Albums
                                 , QString("%1/%2/musicpd/album").arg(global.bugsAPI_url).arg(global.bugsAPI_version)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }


    /**
     * @brief ProcBugsAPI::request_bugs_getList_pd_albums_of_myMusic
     * @param size
     * @param page
     * @param sortOpt
     */
    void ProcBugsAPI::request_bugs_getList_pd_albums_of_myMusic(const int size, const int page, const BugsMyMusic_Sort sortOpt){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        if(size > 0){
            params.addQueryItem("size", QString("%1").arg(size));
        }
        if(page > 0){
            params.addQueryItem("page", QString("%1").arg(page));
        }
        params.addQueryItem("sort", QVariant::fromValue(sortOpt).toString().toLower());
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_PD_Albums
                                 , QString("%1/%2/mymusic/like/musicpd/album").arg(global.bugsAPI_url).arg(global.bugsAPI_version)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }




    // Others ---------------------------------------------------------------------------------------------------

    /**
     * @brief album_id 로 track 목록 정보 요청 (General Album)
     * @param album_id
     */
    void ProcBugsAPI::request_bugs_getList_items_of_album(const int album_id){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Items_Album
                                 , QString("%1/%2/album/%3/track").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(album_id)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }


    /**
     * @brief es_album_id 로 track 목록 정보 요청 (PD's Album)
     * @param es_album_id
     */
    void ProcBugsAPI::request_bugs_getList_items_of_pd_album(const int es_album_id){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Items_PD_Album
                                 , QString("%1/%2/musicpd/album/%3/track").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(es_album_id)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }


    /**
     * @brief playlist_id 로 track 목록 정보 요청 (My Album)
     * @param playlist_id
     */
    void ProcBugsAPI::request_bugs_getList_items_of_my_album(const int playlist_id, const int page, const int size){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        params.addQueryItem("page", QString("%1").arg(page));
        params.addQueryItem("size", QString("%1").arg(size));
        network->request_forBugs(ProcBugsAPI::HttpRequestType::GetList_Items_MyAlbum
                                 , QString("%1/%2/myalbum/%3/track").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(playlist_id)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }

    /**
     * @brief album_id 로 전체적인 상세 정보를 요청
     * @param path
     * @param limit
     * @param offset
     */
    void ProcBugsAPI::request_bugs_get_album(const int album_id){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        network->request_forBugs(ProcBugsAPI::HttpRequestType::Get_AlbumInfo
                                 , QString("%1/%2/album/%3").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(album_id)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }


    /**
     * @brief es_album_id 로 PD의 Album에 대한 전체적인 상세 정보를 요청
     * @param es_album_id
     */
    void ProcBugsAPI::request_bugs_get_pd_album(const int es_album_id){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        network->request_forBugs(ProcBugsAPI::HttpRequestType::Get_PD_AlbumInfo
                                 , QString("%1/%2/musicpd/album/%3").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(es_album_id)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }


    /**
     * @brief ProcBugsAPI::request_bugs_get_artist
     * @param artist_id
     */
    void ProcBugsAPI::request_bugs_get_artist(const int artist_id){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        network->request_forBugs(ProcBugsAPI::HttpRequestType::Get_ArtistInfo
                                 , QString("%1/%2/artist/%3").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(artist_id)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }


    /**
     * @brief ProcBugsAPI::request_bugs_get_video
     * @param mv_id
     */
    void ProcBugsAPI::request_bugs_get_video(const int mv_id){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        network->request_forBugs(ProcBugsAPI::HttpRequestType::Get_VideoInfo
                                 , QString("%1/%2/mv/%3").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(mv_id)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : about Favorites
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // 즐겨찾기 추가 ---------------------------------------------------------------------------------------------

    /**
     * @brief Bugs API 요청 - 즐겨찾기 추가 (Album)
     * @param album_id
     */
    void ProcBugsAPI::request_bugs_addFavorite_album(const int album_id, const QJsonObject& jsonObj_opt){
        this->request_bugs_addFavorie(Favorite_Add_Album, album_id, "album", jsonObj_opt);
    }

    void ProcBugsAPI::request_bugs_addFavorite_pd_album(const int es_album_id, const QJsonObject& jsonObj_opt){
        this->request_bugs_addFavorie(Favorite_Add_PD_Album, es_album_id, "musicpd/album", jsonObj_opt);
    }

    /**
     * @brief Bugs API 요청 - 즐겨찾기 추가 (Aritst)
     * @param artist_id
     */
    void ProcBugsAPI::request_bugs_addFavorite_artist(const int artist_id, const QJsonObject& jsonObj_opt){
        this->request_bugs_addFavorie(Favorite_Add_Artist, artist_id, "artist", jsonObj_opt);
    }


    /**
     * @brief Bugs API 요청 - 즐겨찾기 추가 (Track)
     * @param artist_id
     */
    void ProcBugsAPI::request_bugs_addFavorite_track(const int track_id, const QJsonObject& jsonObj_opt){
        this->request_bugs_addFavorie(Favorite_Add_Track, track_id, "track", jsonObj_opt);
    }


    /**
     * @brief ProcCommon::request_tidal_addFavorie
     * @param requestType
     * @param id_key
     * @param id_val
     * @param subPath
     */
    void ProcBugsAPI::request_bugs_addFavorie(const HttpRequestType requestType, const QVariant id_val, const QString api_subPath, const QJsonObject& jsonObj_opt){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        network->request_forBugs(requestType
                                 , QString("%1/%2/%3/%4/like").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(api_subPath).arg(id_val.toString())
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Post
                                 , false
                                 , jsonObj_opt);
    }



    // 즐겨찾기 삭제 ---------------------------------------------------------------------------------------------

    /**
     * @brief Bugs API 요청 - 즐겨찾기 삭제 (Album)
     * @param album_id
     */
    void ProcBugsAPI::request_bugs_deleteFavorite_album(const int album_id, const QJsonObject& jsonObj_opt){
        this->request_bugs_deleteFavorie(Favorite_Delete_Album, album_id, "album", jsonObj_opt);
    }

    void ProcBugsAPI::request_bugs_deleteFavorite_pd_album(const int es_album_id, const QJsonObject& jsonObj_opt){
        this->request_bugs_deleteFavorie(Favorite_Delete_PD_Album, es_album_id, "musicpd/album", jsonObj_opt);
    }

    void ProcBugsAPI::request_bugs_deleteFavorite_artist(const int artist_id, const QJsonObject& jsonObj_opt){
        this->request_bugs_deleteFavorie(Favorite_Delete_Artist, artist_id, "artist", jsonObj_opt);
    }


    /**
     * @brief ProcBugsAPI::request_tidal_deleteFavorite_track
     * @param track_id
     * @param jsonObj_opt
     */
    void ProcBugsAPI::request_bugs_deleteFavorite_track(const int track_id, const QJsonObject& jsonObj_opt){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        params.addQueryItem("track_ids", QString("%1").arg(track_id));
        network->request_forBugs(Favorite_Delete_Track
                                 , QString("%1/%2/track/like").arg(global.bugsAPI_url).arg(global.bugsAPI_version)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Delete
                                 , false
                                 , jsonObj_opt);
    }


    /**
     * @brief ProcCommon::request_tidal_deleteFavorie
     * @param requestType
     * @param id_val
     * @param subPath
     */
    void ProcBugsAPI::request_bugs_deleteFavorie(const HttpRequestType requestType, const QVariant id_val, const QString api_subPath, const QJsonObject& jsonObj_opt){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        network->request_forBugs(requestType
                                 , QString("%1/%2/%3/%4/like").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(api_subPath).arg(id_val.toString())
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Delete
                                 , false
                                 , jsonObj_opt);
    }




    // 검색 ---------------------------------------------------------------------------------------------

    /**
     * @brief ProcBugsAPI::request_bugs_search_track
     * @param search_word
     * @param page
     * @param size
     * @param sortOpt - exact, popularity, recent, order 만 지원함 (Bugs API)
     */
    void ProcBugsAPI::request_bugs_search_track(const QString search_word, const int page, const int size, const BugsSearch_Sort sortOpt){
        QString str_sortOpt = QVariant::fromValue(sortOpt).toString().toLower();
        QList<BugsSearch_Sort> list_check{BugsSearch_Sort::exact, BugsSearch_Sort::popularity, BugsSearch_Sort::recent, BugsSearch_Sort::order};
        if(!list_check.contains(sortOpt)){
            str_sortOpt = QVariant::fromValue(BugsSearch_Sort::exact).toString().toLower();
        }

        this->request_bugs_search(search_word, page, size, str_sortOpt, "track", HttpRequestType::GetList_Tracks);
    }

    /**
     * @brief ProcBugsAPI::request_bugs_search_artist
     * @param search_word
     * @param page
     * @param size
     * @param sortOpt - exact, order 만 지원함 (Bugs API)
     */
    void ProcBugsAPI::request_bugs_search_artist(const QString search_word, const int page, const int size, const BugsSearch_Sort sortOpt){
        QString str_sortOpt = QVariant::fromValue(sortOpt).toString().toLower();
        QList<BugsSearch_Sort> list_check{BugsSearch_Sort::exact, BugsSearch_Sort::order};
        if(!list_check.contains(sortOpt)){
            str_sortOpt = QVariant::fromValue(BugsSearch_Sort::exact).toString().toLower();
        }

        this->request_bugs_search(search_word, page, size, str_sortOpt, "artist", HttpRequestType::GetList_Artists);
    }

    /**
     * @brief ProcBugsAPI::request_bugs_search_album
     * @param search_word
     * @param page
     * @param size
     * @param sortOpt - exact, popularity, recent, order 만 지원함 (Bugs API)
     */
    void ProcBugsAPI::request_bugs_search_album(const QString search_word, const int page, const int size, const BugsSearch_Sort sortOpt){
        QString str_sortOpt = QVariant::fromValue(sortOpt).toString().toLower();
        QList<BugsSearch_Sort> list_check{BugsSearch_Sort::exact, BugsSearch_Sort::popularity, BugsSearch_Sort::recent, BugsSearch_Sort::order};
        if(!list_check.contains(sortOpt)){
            str_sortOpt = QVariant::fromValue(BugsSearch_Sort::exact).toString().toLower();
        }

        this->request_bugs_search(search_word, page, size, str_sortOpt, "album", HttpRequestType::GetList_Albums);
    }

    /**
     * @brief ProcBugsAPI::request_bugs_search_pd_album
     * @param search_word
     * @param page
     * @param size
     * @param sortOpt - exact, popularity, order 만 지원함 (Bugs API)
     */
    void ProcBugsAPI::request_bugs_search_pd_album(const QString search_word, const int page, const int size, const BugsSearch_Sort sortOpt){
        QString str_sortOpt = QVariant::fromValue(sortOpt).toString().toLower();
        QList<BugsSearch_Sort> list_check{BugsSearch_Sort::exact, BugsSearch_Sort::popularity, BugsSearch_Sort::order};
        if(!list_check.contains(sortOpt)){
            str_sortOpt = QVariant::fromValue(BugsSearch_Sort::exact).toString().toLower();
        }

        this->request_bugs_search(search_word, page, size, str_sortOpt, "musicpd/album", HttpRequestType::GetList_PD_Albums);
    }

    /**
     * @brief ProcBugsAPI::request_bugs_search_video
     * @param search_word
     * @param page
     * @param size
     * @param sortOpt - exact, recent, order 만 지원함 (Bugs API)
     */
    void ProcBugsAPI::request_bugs_search_video(const QString search_word, const int page, const int size, const BugsSearch_Sort sortOpt){
        QString str_sortOpt = QVariant::fromValue(sortOpt).toString().toLower();
        QList<BugsSearch_Sort> list_check{BugsSearch_Sort::exact, BugsSearch_Sort::recent, BugsSearch_Sort::order};
        if(!list_check.contains(sortOpt)){
            str_sortOpt = QVariant::fromValue(BugsSearch_Sort::exact).toString().toLower();
        }

        this->request_bugs_search(search_word, page, size, str_sortOpt, "mv", HttpRequestType::GetList_Videos);
    }


    /**
     * @brief 검색 공통 처리
     * @param search_word
     * @param page
     * @param size
     * @param sortOpt - 각 파트별로 지원하는 Sort Option 달라서, string 으로 받음
     * @param api_subPath
     * @param httpReqtype
     */
    void ProcBugsAPI::request_bugs_search(const QString search_word, const int page, const int size, const QString sortOpt, const QString api_subPath, const HttpRequestType httpReqtype){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));      // 결과가 없으므로, Skip

        QUrlQuery params;
        params.addQueryItem("device_id", global.device.getDeviceRoseToken());
        params.addQueryItem("query", search_word);
        params.addQueryItem("size", QString("%1").arg(size));
        params.addQueryItem("page", QString("%1").arg(page));
        params.addQueryItem("sort", sortOpt);
        network->request_forBugs(httpReqtype
                                 , QString("%1/%2/search/%4").arg(global.bugsAPI_url).arg(global.bugsAPI_version).arg(api_subPath)
                                 , NetworkHttp::HeaderOption_forBugs::Bugs_AccessToken
                                 , params
                                 , NetworkHttp::RequestMethod::Get
                                 , false);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Http 요청 결과 처리
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [슬롯] Http 요청 결과 처리. p_id에 따라 분기 처리 진행된다.
     * @details
     *      @li 로그인 처리 : @see setResult_loginTidal
     *      @li 로그아웃 처리 : @see setResult_logoutTidal
     * @param p_id 요청 아이디
     * @param p_jsonObject HTTP 응답 결과
     */
    void ProcBugsAPI::slot_responseHttp(int p_id, QJsonObject p_jsonObj){

        NetworkHttp* sender_http = qobject_cast<NetworkHttp*>(sender());
        //print_debug();
        //qDebug() << "p_id=" << p_id;
        //qDebug() << "p_jsonObj=" << p_jsonObj;
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getString(p_jsonObj, "ret_code")=="202"){//c230421
            print_debug();
            global.user_forBugs.set_logoutState();
            emit linker->signal_clicked_movePage(global.user_forBugs.getPageData());
            sender()->deleteLater();
            if(global.abs_ani_dialog_wait->isHidden() != true){
                global.abs_ani_dialog_wait->hide(); //cheontidal
            }
            return;
        }

        switch (p_id) {
            case HttpRequestType::Get_Token:            this->setResult_getToken(p_jsonObj);            break;
            case HttpRequestType::Get_MemberInfo:       this->setResult_getMemberInfo(p_jsonObj);       break;

            case HttpRequestType::GetList_Genres:       this->setResult_list_genres(p_jsonObj);         break;
            case HttpRequestType::GetList_Theme:        this->setResult_list_themes(p_jsonObj);         break;

            case HttpRequestType::GetList_Tags_inTheme: this->setResult_list_tags_in_themes(p_jsonObj); break;

            case HttpRequestType::GetList_Tracks:
                this->setResult_list_tracks(p_jsonObj);
                break;

            case HttpRequestType::GetList_Albums:
                this->setResult_list_albums(p_jsonObj);
                break;

            case HttpRequestType::GetList_Videos:
                this->setResult_list_videos(p_jsonObj);
                break;

            case HttpRequestType::GetList_Artists:
                this->setResult_list_artists(p_jsonObj);
                break;

            case HttpRequestType::GetList_PD_Albums:
                this->setResult_list_pd_albums(p_jsonObj);
                break;

            case HttpRequestType::GetList_My_Albums:
                this->setResult_list_my_album(p_jsonObj);
                break;

            case HttpRequestType::Get_AlbumInfo :           this->setResult_album(p_jsonObj);                       break;
            case HttpRequestType::Get_PD_AlbumInfo :        this->setResult_pd_album(p_jsonObj);                    break;
            case HttpRequestType::Get_ArtistInfo :          this->setResult_artist(p_jsonObj);                      break;
            case HttpRequestType::Get_VideoInfo :           this->setResult_video(p_jsonObj);                       break;

            case HttpRequestType::GetList_Items_Album :     this->setResult_list_itmes_of_album(p_jsonObj);         break;
            case HttpRequestType::GetList_Items_PD_Album :  this->setResult_list_itmes_of_pd_album(p_jsonObj);      break;
            case HttpRequestType::GetList_Items_MyAlbum :   this->setResult_list_itmes_of_my_album(p_jsonObj);      break;

            case HttpRequestType::Favorite_Add_Track:
            case HttpRequestType::Favorite_Delete_Track:
                this->setResult_favorite_track(p_jsonObj, sender_http->get_userOptJson());
                break;

            case HttpRequestType::Favorite_Add_Album:
            case HttpRequestType::Favorite_Delete_Album:
                this->setResult_favorite_album(p_jsonObj, sender_http->get_userOptJson());
                break;

            case HttpRequestType::Favorite_Add_PD_Album:
            case HttpRequestType::Favorite_Delete_PD_Album:
                this->setResult_favorite_pd_album(p_jsonObj, sender_http->get_userOptJson());
                break;

            case HttpRequestType::Favorite_Add_Artist:
            case HttpRequestType::Favorite_Delete_Artist:
                this->setResult_favorite_artist(p_jsonObj, sender_http->get_userOptJson());
                break;

            case HttpRequestType::Favorite_Add_Video:
            case HttpRequestType::Favorite_Delete_Video:
                this->setResult_favorite_video(p_jsonObj, sender_http->get_userOptJson());
                break;

        }

        sender()->deleteLater();

    }



    /**
     * @brief ProcBugsAPI::setResult_getToken
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_getToken(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj) == false){
            //emit this->signal_completeReq_getToken(false, tr("Bugs 서비스를 이용할 수 없습니다."));
            QTimer::singleShot(3000,this,[=](){emit linker->signal_loginBugsAcount();});//c230426
            emit this->signal_completeReq_getToken(false, tr("Bugs Service is unavailable."));
        }
        else{
            // DB에 토큰정보 저장. (only DB)
            bugs::BugsTokenInfo tokenInfo_output;
            tokenInfo_output.access_token = ProcJsonEasy::getString(p_jsonObj, "access_token");
            tokenInfo_output.refresh_token = ProcJsonEasy::getString(p_jsonObj, "refresh_token");
            tokenInfo_output.expires_in = ProcJsonEasy::getInt(p_jsonObj, "expires_in");
            tokenInfo_output.scope = ProcJsonEasy::getString(p_jsonObj, "scope");
            tokenInfo_output.token_type = ProcJsonEasy::getString(p_jsonObj, "token_type");

            this->saveTokenInfo_bugsDB(tokenInfo_output);

            emit this->signal_completeReq_getToken(true, "");
        }
    }


    /**
     * @brief ProcBugsAPI::setResult_getMemberInfo
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_getMemberInfo(const QJsonObject &p_jsonObj){
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            // success
            QJsonObject jsonObj_result = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
            emit this->signal_completeReq_getMemberInfo("", ProcJsonEasy::getJsonObject(jsonObj_result, "member"), ProcJsonEasy::getJsonObject(jsonObj_result, "product"));
        }
        else{
            // occur error
            emit this->signal_completeReq_getMemberInfo(ProcJsonEasy::getString(p_jsonObj, "ret_msg"), QJsonObject(), QJsonObject());
        }
    }


    /**
     * @brief ProcBugsAPI::setResult_list_genres
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_genres(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){

            DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
            dataCommon_bugs.list_all.clear();
            dataCommon_bugs.list_kpop.clear();
            dataCommon_bugs.list_pop.clear();
            dataCommon_bugs.list_etc.clear();

            QJsonArray jsonArr_result = ProcJsonEasy::getJsonArray(p_jsonObj, "list");
            for(int i=0 ; i<jsonArr_result.size() ; i++){

                QJsonObject tmp_jsonObj = jsonArr_result.at(i).toObject();

                bugs::FilterOptionData tmp_bugsGenre;
                tmp_bugsGenre.opt_code = ProcJsonEasy::getInt(tmp_jsonObj, "genre_id");
                tmp_bugsGenre.opt_name = ProcJsonEasy::getString(tmp_jsonObj, "genre_nm");

                QString genre_category = ProcJsonEasy::getString(tmp_jsonObj, "genre_category");
                if(genre_category == "ALL"){
                    dataCommon_bugs.list_all.append(tmp_bugsGenre);
                }
                else if(genre_category == "KPOP"){
                    dataCommon_bugs.list_kpop.append(tmp_bugsGenre);
                }
                else if(genre_category == "POP"){
                    dataCommon_bugs.list_pop.append(tmp_bugsGenre);
                }
                else if(genre_category == "ETC"){
                    dataCommon_bugs.list_etc.append(tmp_bugsGenre);
                }
            }

            emit this->completeReq_list_genre_setDone();        // 필요한 경우 연결해서 사용

        }
        else{
            // error
        }
    }


    /**
     * @brief ProcBugsAPI::setResult_list_themes
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_themes(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){

            DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
            dataCommon_bugs.list_theme.clear();

            QJsonArray jsonArr_result = ProcJsonEasy::getJsonArray(p_jsonObj, "list");
            for(int i=0 ; i<jsonArr_result.size() ; i++){

                QJsonObject tmp_jsonObj = jsonArr_result.at(i).toObject();

                bugs::FilterOptionData tmp_bugsTheme;
                tmp_bugsTheme.opt_code = ProcJsonEasy::getString(tmp_jsonObj, "theme_code");
                tmp_bugsTheme.opt_name = ProcJsonEasy::getString(tmp_jsonObj, "theme_nm");

                dataCommon_bugs.list_theme.append(tmp_bugsTheme);
            }

            emit this->completeReq_list_theme_setDone();        // 필요한 경우 연결해서 사용
        }
        else{
            // error
        }
    }



    /**
     * @brief ProcBugsAPI::setResult_list_tags_in_themes
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_tags_in_themes(const QJsonObject &p_jsonObj){

        QList<bugs::BugsTag> list_output;

        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonArray jsonArr_list = ProcJsonEasy::getJsonArray(p_jsonObj, "list");
            for(int i=0 ; i<jsonArr_list.size() ; i++){
                QJsonObject tmp_jsonObj = jsonArr_list.at(i).toObject();

                bugs::BugsTag tmp_bugsTag;
                tmp_bugsTag.tag_id = ProcJsonEasy::getInt(tmp_jsonObj, "tag_id");
                tmp_bugsTag.tag_nm = ProcJsonEasy::getString(tmp_jsonObj, "tag_nm");

                list_output.append(tmp_bugsTag);
            }

        }

        emit this->completeReq_list_tags_in_theme(list_output);

    }






    /**
     * @brief ProcBugsAPI::setResult_list_tracks
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_tracks(const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << strJson;*/

        QList<bugs::TrackItemData> list_output;
        QJsonArray jsonArr_list = QJsonArray();
        bool flag_lastPage = true;

        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){

            // pager
            if(p_jsonObj.contains("pager")){
                jsonArr_list = ProcJsonEasy::getJsonArray(p_jsonObj, "list");
                QJsonObject jsonObj_pager = ProcJsonEasy::getJsonObject(p_jsonObj, "pager");

                for(int i = 0; i < jsonArr_list.count(); i++){
                    QJsonObject tmp_json = jsonArr_list.at(i).toObject();

                    // 정보 담을 struct
                    bugs::TrackItemData tmp_data = ConvertData_forBugs::make_trackData_fromBugsJsonObj(tmp_json);
                    tmp_data.total_count = ProcJsonEasy::getInt(jsonObj_pager, "total_count");
                    list_output.append(tmp_data);
                }

                flag_lastPage = ProcJsonEasy::getBool(jsonObj_pager, "last_yn");
            }
            else if(p_jsonObj.contains("result")){

                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
                jsonArr_list.append(tmp_json);

                // 정보 담을 struct
                bugs::TrackItemData tmp_data = ConvertData_forBugs::make_trackData_fromBugsJsonObj(tmp_json);
                tmp_data.total_count = 1;
                list_output.append(tmp_data);

                flag_lastPage = true;
            }

            emit this->completeReq_list_tracks(list_output, jsonArr_list, flag_lastPage);
        }
        else{
            // occur error
            emit this->completeReq_list_tracks(list_output, QJsonArray(), true);
        }
    }


    /**
     * @brief ProcBugsAPI::setResult_list_tracks
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_albums(const QJsonObject &p_jsonObj){

        QList<bugs::AlbumItemData> list_output;

        if(!ProcJsonEasy::get_flagOk(p_jsonObj)){//c220707
            ProcRoseAPI_withBugs *procRose = new ProcRoseAPI_withBugs();

            bugs::RoseSessionInfo_forBugs sessionInfo;
            sessionInfo.BUGS_ProductDisplayEndDt = "";
            sessionInfo.BUGS_StreamingYn = false;
            sessionInfo.BUGS_ExpiresIn = "";
            sessionInfo.BUGS_ProductName = "";
            sessionInfo.BUGS_Nickname = "";

            sessionInfo.BUGS_RefreshToken = "";
            sessionInfo.BUGS_TokenType = "";
            sessionInfo.BUGS_AccessToken = "";
            sessionInfo.BUGS_FlacPremiumYn = false;
            sessionInfo.BUGS_AdultYn = false;

            sessionInfo.BUGS_Scope = "";

            procRose->request_set_session_info(sessionInfo);
            //ToastMsg::show(this,"", tr("The subscription has expired. Please log in again."), 3000, 0);
            return;

        }

        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonArray jsonArr_list = ProcJsonEasy::getJsonArray(p_jsonObj, "list");

            // pager
            QJsonObject jsonObj_pager = ProcJsonEasy::getJsonObject(p_jsonObj, "pager");

            for(int i=0 ; i<jsonArr_list.count() ; i++){
                QJsonObject tmp_json = jsonArr_list.at(i).toObject();

                // 정보 담을 struct
                bugs::AlbumItemData tmp_data = ConvertData_forBugs::make_albumData_fromBugsJsonObj(tmp_json);
                tmp_data.total_count = ProcJsonEasy::getInt(jsonObj_pager, "total_count");
                list_output.append(tmp_data);
            }

            bool flag_lastPage = ProcJsonEasy::getBool(jsonObj_pager, "last_yn");

            emit this->completeReq_list_albums(list_output, flag_lastPage);
        }
        else{
            emit this->completeReq_list_albums(list_output, true);
        }


    }


    /**
     * @brief ProcBugsAPI::setResult_list_videos
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_videos(const QJsonObject &p_jsonObj){

        QList<bugs::VideoItemData> list_output;
        QJsonArray jsonArr_list = QJsonArray();
        bool flag_lastPage = true;


        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){

            // pager
            if(p_jsonObj.contains("pager")){
                jsonArr_list = ProcJsonEasy::getJsonArray(p_jsonObj, "list");
                QJsonObject jsonObj_pager = ProcJsonEasy::getJsonObject(p_jsonObj, "pager");

                for(int i = 0; i < jsonArr_list.count(); i++){
                    QJsonObject tmp_json = jsonArr_list.at(i).toObject();

                    // 정보 담을 struct
                    bugs::VideoItemData tmp_data = ConvertData_forBugs::make_videoData_fromBugsJsonObj(tmp_json);
                    tmp_data.total_count = ProcJsonEasy::getInt(jsonObj_pager, "total_count");
                    list_output.append(tmp_data);
                }

                flag_lastPage = ProcJsonEasy::getBool(jsonObj_pager, "last_yn");
            }
            else if(p_jsonObj.contains("result")){

                QJsonObject tmp_json = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
                jsonArr_list.append(tmp_json);

                // 정보 담을 struct
                bugs::VideoItemData tmp_data = ConvertData_forBugs::make_videoData_fromBugsJsonObj(tmp_json);
                tmp_data.total_count = 1;
                list_output.append(tmp_data);

                flag_lastPage = true;
            }

            emit this->completeReq_list_videos(list_output, jsonArr_list, flag_lastPage);
        }
        else{
            emit this->completeReq_list_videos(list_output, QJsonArray(), true);
        }

    }


    /**
     * @brief ProcBugsAPI::setResult_list_artists
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_artists(const QJsonObject &p_jsonObj){
        QList<bugs::ArtistItemData> list_output;
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "list");

            // pager
            QJsonObject jsonObj_pager = ProcJsonEasy::getJsonObject(p_jsonObj, "pager");

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                bugs::ArtistItemData tmp_data = ConvertData_forBugs::make_artistData_fromBugsJsonObj(tmp_json);
                tmp_data.total_count = ProcJsonEasy::getInt(jsonObj_pager, "total_count");
                list_output.append(tmp_data);
            }

            bool flag_lastPage = ProcJsonEasy::getBool(jsonObj_pager, "last_yn");

            emit this->completeReq_list_artists(list_output, flag_lastPage);
        }
        else{
            emit this->completeReq_list_artists(list_output, true);
        }
    }


    /**
     * @brief ProcBugsAPI::setResult_list_artists
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_pd_albums(const QJsonObject &p_jsonObj){
        QList<bugs::PD_AlbumItemData> list_output;
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "list");

            // pager
            QJsonObject jsonObj_pager = ProcJsonEasy::getJsonObject(p_jsonObj, "pager");

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                bugs::PD_AlbumItemData tmp_data = ConvertData_forBugs::make_pd_albumData_fromBugsJsonObj(tmp_json);
                tmp_data.total_count = ProcJsonEasy::getInt(jsonObj_pager, "total_count");
                list_output.append(tmp_data);
            }

            bool flag_lastPage = ProcJsonEasy::getBool(jsonObj_pager, "last_yn");

            emit this->completeReq_list_pd_albums(list_output, flag_lastPage);
        }
        else{
            emit this->completeReq_list_pd_albums(list_output, true);
        }
    }



    /**
     * @brief ProcBugsAPI::setResult_list_my_album
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_my_album(const QJsonObject &p_jsonObj){
        QList<bugs::MyAlbumItemData> list_output;
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "list");

            // pager
            QJsonObject jsonObj_pager = ProcJsonEasy::getJsonObject(p_jsonObj, "pager");

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                bugs::MyAlbumItemData tmp_data = ConvertData_forBugs::make_my_albumData_fromBugsJsonObj(tmp_json);
                tmp_data.total_count = ProcJsonEasy::getInt(jsonObj_pager, "total_count");
                list_output.append(tmp_data);
            }

            bool flag_lastPage = ProcJsonEasy::getBool(jsonObj_pager, "last_yn");

            emit this->completeReq_list_my_albums(list_output, flag_lastPage);
        }
        else{
            emit this->completeReq_list_my_albums(list_output, true);
        }
    }





    /**
     * @brief
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_itmes_of_album(const QJsonObject &p_jsonObj){

        QList<bugs::TrackItemData> list_output;
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "list");

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                bugs::TrackItemData tmp_data = ConvertData_forBugs::make_trackData_fromBugsJsonObj(tmp_json);
                tmp_data.total_count = jsonArr_item.count();
                list_output.append(tmp_data);
            }

            // pager 없음
            emit this->completeReq_list_items_of_album(list_output, jsonArr_item, true);
        }
        else{
            // occur error
            emit this->completeReq_list_items_of_album(list_output, QJsonArray(), true);
        }

    }



    /**
     * @brief ProcBugsAPI::setResult_list_itmes_of_pd_album
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_itmes_of_pd_album(const QJsonObject &p_jsonObj){

        QList<bugs::TrackItemData> list_output;
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1) == 0){
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "list");

            for(int i = 0; i < jsonArr_item.count(); i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                bugs::TrackItemData tmp_data = ConvertData_forBugs::make_trackData_fromBugsJsonObj(tmp_json);
                tmp_data.total_count = jsonArr_item.count();
                list_output.append(tmp_data);
            }

            // pager 없음
            emit this->completeReq_list_items_of_pd_album(list_output, jsonArr_item, true);
        }
        else{
            // occur error
            emit this->completeReq_list_items_of_pd_album(list_output, QJsonArray(), true);
        }

    }



    /**
     * @brief ProcBugsAPI::setResult_list_itmes_of_my_album
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_list_itmes_of_my_album(const QJsonObject &p_jsonObj){

        QList<bugs::TrackItemData> list_output;
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1) == 0){
            QJsonArray jsonArr_item = ProcJsonEasy::getJsonArray(p_jsonObj, "list");

            for(int i=0 ; i<jsonArr_item.count() ; i++){
                QJsonObject tmp_json = jsonArr_item.at(i).toObject();

                // 정보 담을 struct
                bugs::TrackItemData tmp_data = ConvertData_forBugs::make_trackData_fromBugsJsonObj(tmp_json);
                tmp_data.total_count = jsonArr_item.count();
                list_output.append(tmp_data);
            }

            // pager
            QJsonObject jsonObj_pager = ProcJsonEasy::getJsonObject(p_jsonObj, "pager");
            bool flag_lastPage = ProcJsonEasy::getBool(jsonObj_pager, "last_yn");

            emit this->completeReq_list_items_of_my_album(list_output, jsonArr_item, flag_lastPage);
        }
        else{
            // occur error
            emit this->completeReq_list_items_of_my_album(list_output, QJsonArray(), true);
        }

    }


    /**
     * @brief Album 상세정보 처리반환
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_album(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonObject jsonObj_item = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
            AlbumItemData data_album = ConvertData_forBugs::make_albumData_fromBugsJsonObj(jsonObj_item);
            emit this->completeReq_album(data_album);
        }
    }


    /**
     * @brief ProcBugsAPI::setResult_pd_album
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_pd_album(const QJsonObject &p_jsonObj){

        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonObject jsonObj_item = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
            PD_AlbumItemData data_album = ConvertData_forBugs::make_pd_albumData_fromBugsJsonObj(jsonObj_item);
            emit this->completeReq_pd_album(data_album);
        }
    }


    /**
     * @brief Artist 상세정보 처리반환
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_artist(const QJsonObject &p_jsonObj){
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonObject jsonObj_item = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
            ArtistItemData data_artist = ConvertData_forBugs::make_artistData_fromBugsJsonObj(jsonObj_item);
            emit this->completeReq_artist(data_artist);
        }
    }


    /**
     * @brief Video 상세정보 처리반환
     * @param p_jsonObj
     */
    void ProcBugsAPI::setResult_video(const QJsonObject &p_jsonObj){
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonObject jsonObj_item = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
            VideoItemData data_video = ConvertData_forBugs::make_videoData_fromBugsJsonObj(jsonObj_item);
            emit this->completeReq_video(data_video, jsonObj_item);
        }
    }




    // Favorite -------------------------------------------------------------------------

    /**
     * @brief Track에 대해서 Fav 처리 결과를 반영함 (Data & UI)
     * @param p_jsonObj
     * @param p_jsonObj_opt
     */
    void ProcBugsAPI::setResult_favorite_track(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt){
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonObject jsonObj_result = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
            p_jsonObj_opt.insert("likes_yn", ProcJsonEasy::getBool(jsonObj_result, "likes_yn"));

            emit this->completeReq_favarite_track(p_jsonObj_opt);
        }
        else{
            emit this->failedReq_favorite_track(p_jsonObj_opt);
        }
    }

    /**
     * @brief Album에 대해서 Fav 처리 결과를 반영함 (Data & UI)
     * @param p_jsonObj
     * @param p_jsonObj_opt
     */
    void ProcBugsAPI::setResult_favorite_album(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt){
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonObject jsonObj_result = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
            p_jsonObj_opt.insert("likes_yn", ProcJsonEasy::getBool(jsonObj_result, "likes_yn"));

            emit this->completeReq_favarite_album(p_jsonObj_opt);
        }
        else{
            emit this->failedReq_favorite_album(p_jsonObj_opt);
        }
    }


    /**
     * @brief PD Album에 대해서 Fav 처리 결과를 반영함 (Data & UI)
     * @param p_jsonObj
     * @param p_jsonObj_opt
     */
    void ProcBugsAPI::setResult_favorite_pd_album(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt){

        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonObject jsonObj_result = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
            p_jsonObj_opt.insert("likes_yn", ProcJsonEasy::getBool(jsonObj_result, "likes_yn"));

            emit this->completeReq_favarite_pd_album(p_jsonObj_opt);
        }
        else{
            emit this->failedReq_favorite_pd_album(p_jsonObj_opt);
        }
    }


    /**
     * @brief ProcBugsAPI::setResult_favorite_artist
     * @param p_jsonObj
     * @param p_jsonObj_opt
     */
    void ProcBugsAPI::setResult_favorite_artist(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt){
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonObject jsonObj_result = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
            p_jsonObj_opt.insert("likes_yn", ProcJsonEasy::getBool(jsonObj_result, "likes_yn"));

            emit this->completeReq_favarite_artist(p_jsonObj_opt);
        }
        else{
            emit this->failedReq_favorite_artist(p_jsonObj_opt);
        }
    }


    /**
     * @brief Video에 대해서 Fav 처리 결과를 반영함 (Data & UI)
     * @param p_jsonObj
     * @param p_jsonObj_opt
     */
    void ProcBugsAPI::setResult_favorite_video(const QJsonObject &p_jsonObj, QJsonObject p_jsonObj_opt){
        if(ProcJsonEasy::get_flagOk(p_jsonObj) && ProcJsonEasy::getInt(p_jsonObj, "ret_code", -1)==0){
            QJsonObject jsonObj_result = ProcJsonEasy::getJsonObject(p_jsonObj, "result");
            p_jsonObj_opt.insert("likes_yn", ProcJsonEasy::getBool(jsonObj_result, "likes_yn"));

            emit this->completeReq_favarite_video(p_jsonObj_opt);
        }
        else{
            emit this->failedReq_favorite_video(p_jsonObj_opt);
        }
    }

}
