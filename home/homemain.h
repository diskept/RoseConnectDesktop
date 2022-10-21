#ifndef HOMEMAIN_H
#define HOMEMAIN_H

#include "home/abstractmaincontent.h"
#include "home/remotewidget.h"
#include "home/sectionbottom.h"
#include "home/sectionleft.h"
#include "home/suggestionsbar.h"

#include "common/findip.h"
#include "common/httpServer.h"
#include "common/linker.h"

#include "data/datadevice.h"

#include "roseHome/rosehomeMain.h"
#include "music/musicmain.h"
#include "music_2/MusicMain_2.h"          // by sunnyfish
#include "video/videomain.h"
#include "radio/radiomain.h"
#include "roseRadio/roseradio_Main.h"
#include "rosefm/rosefm_main.h"
#include "rosetube/rosetubemain.h"
#include "podcast/podcastmain.h"
#include "cdplay/CDplay_Main.h"//c220721
#include "tidal/TidalMain.h"
#include "bugs/BugsMain.h"
#include "qobuz/QobuzMain.h"
#include "apple/AppleMain.h"
#include "spotify/spotifymain.h"
#include "setting/settingmain.h"
#include "setting/settingtrackoption.h"

#include "widget/queueplaylist.h"
#include "widget/searchremotebar.h"
#include "widget/downloadprogress.h"
#include "widget/volumncontrol.h"
#include "widget/dialogWakeUp.h"

#include "versionupdate/Updater.h"//c220411
#include "versionupdate/QSimpleUpdater.h"//c220411

#include <QMutex>
#include <QWidget>
#include <QHBoxLayout>
#include <QAudioOutput>//c220609


#define print_home_func() qDebug() << "[HOME][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


/**
 * @brief 홈 메인 클래스 @n
 *      @sa SectionLeft 대항목 메뉴, @sa SectionBottom 재생바, @sa AbstractMainContent 페이지 화면으로 구성
 * @note
 *      @sa RemoteWidget 리코콘위젯, @sa QueuePlayList 재생 큐, @sa VolumnControl 볼륨 @n
 *      @sa DownloadProgress DB 다운로드 프로그레스, @sa SuggestionsBar 검색제안어 등의 QWidget 관리 @n
 */
class HomeMain : public QWidget
{
    Q_OBJECT
public:
    explicit HomeMain(QWidget *parent = nullptr);
    ~HomeMain();

    void reSize(const int &p_width);

signals:

public slots:
    void changedLeftMainMenu(QString p_menuCode);
    void connectDevice(QString p_ip);
    void slot_httpservermsg_menu_flag();
    void slot_queue_hide();
    void slot_httpservermsg(const QJsonObject &jj);//cheon210617-http

private slots:
    void slot_showDlgOfDisconnect_hide();
    void slot_showDlgOfDisconnect_call();
    void printError(QNetworkReply::NetworkError code);//c220521
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_startDownloadDB();
    void downloadingDB(qint64, qint64);
    void downloadedDB();
    void slot_completedViewToTable();
    void clickedBottomQueueIcon(bool p_flagShow);
    void slot_clickedBottomVolumn(bool p_flagShow);

    void showRemote(bool p_flagShow);
    void slot_changedPageSetting_VU();
    void slot_changedPageSetting_InputOutput();
    void slot_changedPageSetting_HDMI();
    void slot_changedPageSetting_Timer();
    void slot_search5555(const QString &p_text);//c220904_1
    void slot_searchSuggestionsChanged(const QJsonObject &p_jsonObject);



    void slot_changedLoginInfo();
    void slot_changedDevicePower();
    void slot_changedSubMenu();

    void slot_changePagePlayList(const QJsonObject &p_jsonObject);
    void slot_changedMainMenuAndMovePage(int p_mainMenuCode, const QJsonObject &p_jsonObject);
    void slot_getCurrPlayData();

    void slot_clickedDBRefreshBtn();
    void slot_showPlayFullScreen(const QJsonObject &p_jsonData);
    void slot_changedCurrPlayTrack(const QJsonObject &p_jsonData);
    void slot_showDlgOfDisconnect();

    void slot_searchBarFocusChanged(bool);

    void slot_incompleteReq_get_tsession_info();
    void slot_completeReq_get_tsession_info(const tidal::RoseSessionInfo_forTidal& sessionInfo);
    void slot_completeReq_getTidalGenre(const bool flag, const QString errorMsg);
    void slot_completeReq_getTidalMoods(const bool flag, const QString errorMsg);
    void slot_completeReq_getRefreshToken(const bool flagSuccess, const QString errorMsg);
    void slot_completeReq_getSessionInfo(const QString errorMsg, const QJsonObject& jsonObj_session);
    void slot_completeReq_getSubscription(const QString errorMsg, const QJsonObject& jsonObj_subscription);

    void slot_incompleteReq_get_bsession_info();
    void slot_completeReq_get_bsession_info(const bugs::RoseSessionInfo_forBugs& sessionInfo);
    void slot_completeReq_getbLoginInfo();

    void slot_incompleteReq_get_qsession_info();
    void slot_completeReq_get_qsession_info(const qobuz::RoseSessionInfo_forQobuz& sessionInfo);
    void slot_completeReq_getqLoginInfo();
    void slot_completeReq_getQobuzGenre();

    void slot_completeReq_get_asession_info(const apple::RoseSessionInfo_forApple& sessionInfo);
    void slot_completeReq_getAppleGenre(const bool flag, const QString errorMsg);

    void slot_sectionleft_active();
    void showDlgOfNotice_Readme_show();
    //void slot_clickedMovePageMusicSearch(QString a);//cheon211008-search
    void slot_clickedMovePageMusicSearch();//cheon211008-search
    void slot_clickedMovePageVideoSearch();//cheon211008-search
    void slot_clickedMovePageRadioSearch();//cheon211008-search
    void slot_clickedMovePageRoseRadioSearch();//cheon211008-search
    void slot_clickedMoveRoseTubeSearch();//cheon211008-search
    void slot_clickedMovePagePodCastSearch();//cheon211008-search
    void slot_clickedMovePageTidalSearch();//cheon211008-search
    void slot_clickedMovePageBugsSearch();//cheon211008-search
    void slot_clickedMovePageQobuzSearch();//cheon211008-search


    void slot_clickedMovePageSearch_share(QString);//c220818
    void slot_msg();//c220818


    void slot_MovePageSearch();//cheon211008-search
    void slot_MovePageSearchMusic();//cheon211008-search
    void slot_MovePageSearchPodCast();//cheon211008-search
    void slot_MovePageSearchRadio();//cheon211008-search
    void slot_MovePageSearchVideo();//cheon211008-search

    void slot_MovePageRoseHomeTo(const QString leftmenu);
    void slot_connectDevice();//c220521
    void slotReadyRead_connect(QNetworkReply *reply);//c220526
    //void slot_player_valuestop();//c220705
    void slot_rebooting();//c220624
    void slot_searchBarClear();//c220628
    void slot_responseHttp_deviceIOState(const int &p_id, const QJsonObject &p_jsonObject);//c220708

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    //void nextSongPlay();//c220705
    void setInit();
    void setUIControl();
    void setDataLoginAfter();

    void checkUserLogin();
    void checkUserInfo();
    void checkTidalLogin();
    void checkBugsLogin();
    void checkQobuzLogin();//cheon2 code
    void checkAppleLogin();
    long getOldDbFileSize();

    void saveCurrentDeviceData(DataDevice &p_data);
    void requestAutoLogin();
    void requestSetRoseUserInfo();
    void requestGetRoseUserInfo();
    void requestGetShazamInfo();
    void requestGetRoseFMCountryInfo();
    void requestGetRoseFMChannelInfo();
    void requestGetRoseRadioGenreInfo();
    void requestGetRoseRadioRegionInfo();
    void requestGetRoseRadioLanguageInfo();

    void requestGetTimer();         //cheon01_src
    void requestGetHDMI();          //cheon01_src

    void clearLayout(QLayout *layout);

    void setResultOfCurrDevice(const QJsonObject &p_json);
    void setResultOfLogin(const QJsonObject &p_json);
    void setResultOfGetRoseUserInfo(const QJsonObject &p_json);
    void setResultOfGetShazamInfo(const QJsonObject &p_json);
    void setResultOfUserProfile(const QJsonObject &p_json);
    void setResultOfRoseFMCountryInfo(const QJsonObject &p_json);
    void setResultOfRoseFMChannelInfo(const QJsonObject &p_json);
    void setResultOfRoseRadioGenreInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage);
    void setResultOfRoseRadioRegionInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage);
    void setResultOfRoseRadioLanguageInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage);
    void unRaiseAllWidget(bool p_unRaiseRemote=true, bool p_unRaiseQueue=true, bool p_unRaiseVolumn=true);
    void resizeEvent(QResizeEvent *event) override;

    void showDlgOfNotice_Readme();
    void changedIndexNotice(int index);
    void dlgNotice_Readme();
    void read_noticeDBTable();
    void dialog_comfirmCD();//cheon210817-iso
    void changedLeftMainMenu_search(QString p_menuCode);

private:
    int p_preByte = 0;//c220801
    bool db_downloadCompleting_flag = false;//c220801
    //DesktopMediaPlayerThread *Dt_thread;//c220705
    QAudioOutput *audioOutput;
    Updater *m_updater_tmp;
    SettingTrackOption *settingTrackOption;
    MusicSearch *musicSearch=nullptr;           ///< 음악 > 검색//cheon211108
    //DialogConfirm *dlgConfirmCD;//cheon210817-iso
    Linker *linker;
    FindIP *findIp;
    SectionLeft *sectionLeft;
    SectionBottom *sectionBottom;
    QHBoxLayout *hlayout_content;
    DownloadProgress *progress;
    RemoteWidget *remoteWidget=nullptr;
    SuggestionsBar *suggestionBar=nullptr;
    QueuePlayList *queueplaylist=nullptr;
    QueuePlayList *queueplaylist_copy=nullptr;//cheon211120-3
    VolumnControl *volumnControl=nullptr;
    AbstractMainContent *curr_absMainContent = nullptr;

    bool db_downloadComplete_flag = true;//c220826_2
    bool flag_tget_session = false;
    bool flag_bget_session = false;
    bool flag_qget_session = false;
    bool flag_db_prograssbar = false;
    int widget_left_min = 0;

    QString currMainMenuCode;       ///< 현재 메인메뉴 코드 값
    QString oldDeviceID =  "";
    QString connectDeviceIp = "";

    DialogConfirm *dlgConfirm = new DialogConfirm;
    DialogConfirm *dlgConfirmReadmeNotice;
    QMutex mutex;
    bool showDlgOfDisconnect_flag = false;

    bool isToken = false;

    int genre_offset = 0;
    int region_offset = 0;
    int language_offset = 0;

    int genre_total_count = 0;
    int region_total_count = 0;
    int language_total_count = 0;

    QJsonArray jsonArr_genre;
    QJsonArray jsonArr_region;
    QJsonArray jsonArr_language;

    bool flagRoseRadio = false;
};

#endif // HOMEMAIN_H
