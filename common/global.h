#ifndef GLOBAL_H
#define GLOBAL_H

#include "common/rosesettings.h"

#include "data/datadevice.h"
#include "data/datauser.h"
#include "data/datauser_forRoseFm.h"
#include "data/datauser_forRoseRadio.h"
#include "data/datauser_fortidal.h"
#include "data/DataUser_forBugs.h"
#include "data/datauser_forqobuz.h"
#include "data/datauser_forApple.h"

#include <QString>
#include <QDebug>
#include <QDialog>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";

/**
 * @brief The Global class : Global 변수 관리 @n
 *      API Path 및 Global 변수 선언 @n
 *      장치 데이터 @sa DataDevice, 사용자 데이터(로즈) @sa DataUser @n
 *      사용자 데이터(타이달) @sa DataUser_forTIDAL, 사용자 데이터(벅스) @sa DataUser_forBugs 를 갖고있다.
 */

class Global
{

public :
    DataDevice device;
    DataUser user;

    DataUser_forRoseFM user_forRoseFM;
    DataUser_forRoseRadio user_forRoseRadio;

    DataUser_forTIDAL user_forTidal;
    DataUser_forBugs user_forBugs;
    DataUser_forQOBUZ user_forQobuz;//cheon2 code
    DataUser_forApple user_forApple;

    //Search button
    QPushButton *btn_Music;//cheon211008-search
    QPushButton *btn_Video;//cheon211008-search
    QPushButton *btn_Radio;//cheon211008-search
    QPushButton *btn_RoseRadio;//cheon211008-search
    QPushButton *btn_PodCast;//cheon211008-search
    QPushButton *btn_RoseTube;//cheon211008-search
    QPushButton *btn_Tidal;//cheon211008-search
    QPushButton *btn_Bugs;//cheon211008-search
    QPushButton *btn_Qobuz;//cheon211008-search
    //QPushButton *btn_Apple;//cheon211008-search
    bool searchMenuFlag = false;//cheon211008-search
    QString dragDrop_pathStr;//c220726
    QString smbParamPath_cmd;//c220907_1
    QString pretoastmsg = "";//c221021_1

    /// 기본 포트 : 9283
    QString port = "9283";
    /// Android 이미지 다운용 포트 : 8192 -> 8080 변경 12.20.2021
    QString port_img = "8080";
    QString legacy_v3 = "http://beta.api.roseaudio.kr/legacy/v3";
    QString legacy_v3_api = "https://api.roseaudio.kr/legacy/v3";
    QString legacy_mod = "http://beta.api.roseaudio.kr/legacy/mod";
    QString legacy_mod_api = "https://api.roseaudio.kr/legacy/mod";
    //QString hifimp = "https://www.hifimp.com:18081/v2/rest/rose";
    QString release_roseaudio = "https://release.roseaudio.kr:18081/v2/rest/rose";
    QString itunes_search = "https://itunes.apple.com/search";
    QString newpipe_api = "http://beta.api.roseaudio.kr/newpipe/v1";
    QString rosefm_member = "https://api.roseaudio.kr/v1/radio/member";
    QString legacy_dev_v1 = "https://dev.api.roseaudio.kr/v1";
    QString legacy_radio = "";
    //QString legacy_v1 = (debugCHK > 3) ? "https://dev.api.roseaudio.kr/v1" : "https://api.roseaudio.kr/v1";
    QString legacy_v1 = "";
    QString newpipe_v1 = "";
    int debugCHK = 0;

    //QString rosehome_userpick = "https://api.roseaudio.kr/v1/member/playlist/userpick/all";//cheon211015
    QString rosehome_userpick = "";

    int device_outputMode = 0;
    int indexCurrPlay[10] = {-1,0};//c220625
    int total_queue_list_count = 0;//c220528
    int autoRecommCount = 0;//c220528
    int Queue_track_count = 0;
    int ThumImageRoadCount = 0;//c220528
    bool power_sleepMode_flag = false;//c220402
    bool signal_selectedDevice_flag = false;//c220319
    bool isPlaying_flag = false;
    bool isPlaying_stop_flag = false;
    int currentExitCode = 0;//cheon210812-iso
    bool isoflag = false;//cheon210812-iso
    bool bugsloginChange = false;//cheon210617-http
    bool tidalloginChange = false;//cheon210617-http
    bool qobuzloginChange = false;//cheon210617-http
    bool userloginChage = false;//cheon210617-http
    bool bugsloginOk = false;//cheon210617-login
    bool updateCheckFlag = false;//cheon220519-network
    bool signal_device_connect_flag = false;

    //--------------------------------------------------//cheon210702-list
    int qobuz_playlist_cnt = 0;//cheon210703-list
    int qobuz_albumsuggested_cnt = 0;//cheon210703-list
    int tidal_suggestedalbum_cnt = 0;//cheon210703-list
    //----------------------------------------------------
    bool queue_position_startFlag = false;//cheon21120101
    bool queue_recent_track_addFlag = true;//cheon211206
    bool queue_music_startFlag = false;//c220418
    bool queue_music_menu_startFlag = false;//c220418
    bool queue_playFlag = false;//c220418
    bool queue_queuePlayFlag = false;//c220418
    bool queue_recommPlayFlag = false;//c220418
    bool onOffModeFlag = false;
    //QDialog *dialog_wait;//cheontidal
    //QDialog *dialog_wait;//cheontidal
    int queulistLeftPosion = 0;

    // sectionleft
    bool enable_section_left = false;
    bool queueTimerFlag = false;
    bool show_flag = false;
    int changeEditMode_isPlayMode = true;//c220430

    bool enable_message_flag = false;
    int enable_message_count = 0;
    bool window_activate_flag = false;//c220724

    // Music
    QString p_rootName = "";//cheon210831-network
    QString p_path = "";//cheon210831-network
    long q_wakeupTerm = 10;
    long u_wakeupTerm = 0;
    int LmtCnt = 0;
    int LmtCnt_H = 0;
    int request_total_count=500;//cheon210704-list
    int mouse_time_term = 2300;//cheon210704-mouse
    QString search_text = "";//searchtextcopy
    QString checkSum = "";//cheon210916-total
    int queue_position = 0;//cheon211104-01
    int current_position = 0;//cheon211104-01
    int current_duration = 0;
    bool isPlaying = false, device_playingState = false;//c220609
    int repeatMode = 0;
    int shuffleMode = 0;
    int current_totalqueue_cnt = 0;//cheon211120-03
    bool islistOpenFlag = false;
    bool queuelist_mouse_trigger_flag = true;//c220418
    bool queuelist_mouse_trigger_menu_flag = true;//c220418

    // Tidal
    const QString tidalAPI_url = "https://api.tidal.com/v1";      ///< TIDAL API URL
    const QString tidalToken = "8kNOZsDJNl38Apfo";                ///< TIDAL 요청 시 사용되는 Token 정보.

    const QString tidal_client_id = "JttPIQMxj7cidXuV";
    const QString tidal_auth_url_base = "https://login.tidal.com";
    const QString tidal_api_rest_auth_url = "https://auth.tidal.com/v1";

    const QString tidal_api_rest_url = "https://api.tidal.com/v1";
    const QString tidal_image_url = "https://resources.wimpmusic.com/images";

    //const QString tidal_auth_res_redirectUrl = "https://beta.roseaudio.kr/service/application/login?app=bugs/callback";
    const QString tidal_auth_res_redirectUrl = "https://beta.roseaudio.kr/service/application/callback";

    QString tidalAccessToken = "";

    // Qobuz
    const QString qobuzAPI_url = "https://www.qobuz.com/api.json/0.2";      ///< QOBUZ API URL

    //const QString X_App_Id = "680786274";     //cheon_debug1220           ///< QOBUZ 요청 시 사용되는 Token 정보.
    const QString X_App_Id = "398721064";
    const QString X_App_secret = "812c9faa644f0cb64b9a90df19a98641";

    // Bugs ---------------------------------------------------------------------- START

    // 인증 시 사용하는 URL관련
    //    const QString bugs_auth_url = "https://qa-secure.bugs.co.kr";      ///< Bugs API URL  (QA)
    QString bugs_auth_url = "https://secure.bugs.co.kr";      ///< Bugs API URL  (배포용)
    const QString bugs_auth_res_redirectUrl = "https://beta.roseaudio.kr/service/application/login?app=bugs/callback";          ///< 인증 요청결과를 받기위한 페이지

    // 인증 후 일반 API 요청 관련
    //    const QString bugsAPI_url = "https://beta-mapi.bugs.co.kr/open";
    const QString bugsAPI_url = "https://mapi.bugs.co.kr/open";
    const QString bugsAPI_version = "5";


    const QString bugs_client_id = "citech";
    const QString bugs_client_secret = "glu!LK5d";

    const QString BUGS_API_HEADER___client = "CLIENT";
    const QString BUGS_API_HEADER___appName = "ROSE";
    const QString BUGS_API_HEADER___versionPC = "1.0.0";
    const QString BUGS_API_HEADER___platform_name__PREFIX = "ROSE_";        ///< 뒤에 장비명 붙여야함

    // Bugs ---------------------------------------------------------------------- END


    // RoseTUBE ------------------------------------------------------------------ START
    int ROSETUBE_API_TRACK[20];
    int ROSETUBE_API_PLAYLIST[10];

    int ROSETUBE_GENRE_TRACK[50];
    int ROSETUBE_EXPLORE_TRACK[20];


    // RoseTUBE ------------------------------------------------------------------ END


    int width_mainwindow = 0;
    int height_mainwindow = 0;
    int left_mainwindow = 0;        ///< MainWindow의 left 위치 (added by sunnyfish)
    int top_mainwindow = 0;         ///< MainWindow의 top 위치 (added by sunnyfish)
    bool flagConnected = false;

    /// 앨범 좋아요 조회
    QString api_album_favorite_get = "/album/isfavorite";
    QString api_album_favorite_set = "/playlist/create";
    QString api_album_favorite_delete = "/album/favorite/delete";
    QString api_album_favorite_add = "/album/favorite/add";

    /// 트랙 좋아요 조회
    QString api_track_favorite_get = "/track/isfavorite";
    /// 트랙 좋아요 해제
    QString api_track_favorite_delete = "/track/favorite/delete";
    /// 트랙 좋아요 설정
    QString api_track_favorite_add = "/track/favorite/add";

    /// (트랙/앨범) 별 정보 GET
    QString api_music_rate_get = "/music.rate.get";                 // ---------> ProcStarData 클래스에서만 사용하도록 해야할 듯함. by sunnyfish (2020-07-31).
    /// (트랙/앨범) 별 정보 SET
    QString api_music_rate_set = "/music.rate.set";

    /// (플레이리스트) 좋아요 설정
    QString api_playlist_favorite_add = "/playlist/favorites/add";
    /// (플레이리스트) 좋아요 해제
    QString api_playlist_favorite_delete = "/playlist/favorites/delete";


    int img_width = 0;//c220909_2
    QStringList standardOutput;//c220913_1
    QStringList shareFolderInfoSel;//c220913_1
    QString userPassword;//c220913_1
    // Now section Menu
    int now_Section = 0;

    // Select Language
    int lang = 0;
    int notice = 0;
    bool isMoveMode = false;
    bool isPlayMode = false;
    bool isEditMode = false;

    bool isMoveModePlaylist = false;

    bool powerDialogShowFlag = false;
    QString music_folder_topdir;//c220609
    int setTextVolumn = 0;//c220609
    QMediaPlayer *music_player;//c220609
    QMediaPlaylist *music_playlist;//220609
    bool  desktopPlayerMode = false;//c220609
    //bool device_playingState = false;
    QList<QMediaContent> musicPlaylistMediaContent;
    QDialog *abs_ani_dialog_wait;
    QString savetypyingStr;//c220904_1

    Global() = default;
    Global(const Global&) = delete;
    Global(Global&&) = delete;

    static Global& Instance(){
        static Global global;
        return global;
    }
};

namespace  {
Global& global = Global::Instance();
}

#endif // GLOBAL_H
