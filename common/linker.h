#ifndef LINKER_H
#define LINKER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

/**
 * @brief The Linker class 링커 @n
 * Object간 signal & slot 을 연결을 위한 링커 Object
 */
class Linker : public QObject
{
    Q_OBJECT

private:
    Q_DISABLE_COPY(Linker);
    explicit Linker(QObject *parent = nullptr);
    virtual ~Linker();
public:
    static Linker* getInstance(){
        static Linker instance;
        return &instance;
    }

signals:
    void signal_setMountShareFolder(QString, QString, QString, QString);//c220913_1
    void signal_goFolder(QString);//c220909_1
    void signal_searchFolder_clicked();//c220906_1
    void signal_sugestionbarDown(QString);//c220904_1
    void signal_sugestionbarUp(QString);//c220904_1

    void signal_dialog_share_link(QString);//c220902_1
    void signal_optMorePopup_menuClicked22(int);//c220818
    void signal_dragDropPlay(int, QJsonObject);//c220803
    void signal_dropEvent_hide_show(bool show);//c220727
    void signal_dragEnterEvent_hide_show(bool show);//c220727
    void signal_text_copy();//c220726
    void signal_text_clear();//c220726
    void signal_sugestionbarOld_sel(QString);//c220715
    void signal_clickListBtn();//c220628
    void signal_checknetwork();//c220624
    void signal_focusOutEvent();//c220624
    void signal_focusInEvent();//c220623
    void signal_playMusic_desktop(QJsonArray, bool);//c220615
    void signal_playlist_reflesh();//c220615
    void siganl_desktopModeStart();//c220609
    void signal_deviceStart();//c220609
    void signal_fileInfo_filePath_stop();//c220609
    void signal_localPlayMusicInfo(const QJsonObject &p_jsonData, const QPixmap &pixmap);//c220609
    void signal_clickedRemoteEXTE();//c220521
    void signal_profileImage_delete();//c220428
    void signal_window_show();//c220401
    void signal_window_hide();//c220401
    void signal_window_resize();//c220401
    void signal_sliderclick(int);//c220521
    void signal_checkQueue(int, QString);//c1209
    void signal_queuelist_reload();//c211213
    void signal_recommand_data_change();//c220626_2
    void signal_queuelist_musicStart();//c220418
    void signal_queuelist_mouse_trigger_menu_flag();//c220418

    void signal_clickedMovePageMusicSearch();//cheon211008-search
    void signal_clickedMovePageVideoSearch();//cheon211008-search
    void signal_clickedMovePageRadioSearch();//cheon211008-search
    void signal_clickedMovePageRoseRadioSearch();//cheon211008-search
    void signal_clickedMovePageRoseTubeSearch();//cheon211008-search
    void signal_clickedMovePageRoseTubeSearchCall();//c220728
    void signal_clickedMovePagePodCastSearch();//cheon211008-search
    void signal_clickedMovePageTidalSearch();//cheon211008-search
    void signal_clickedMovePageBugsSearch();//cheon211008-search
    void signal_clickedMovePageQobuzSearch();//cheon211008-search

    void signal_clickedMovePageSearch_share(QString);//c220818

    void signal_menumoveMusic();//cheon211008-search
    void signal_menumoveVideo();//cheon211008-search
    void signal_menumoveRadio();//cheon211008-search
    void signal_menumoveRoseTube();//cheon211008-search
    void signal_menumovePodCast();//cheon211008-search
    void signal_menumoveTidal();//cheon211008-search
    void signal_menumoveBugs();//cheon211008-search
    void signal_menumoveQobuz();//cheon211008-search

    void signal_music_start();//c220411
    void signal_device_connect_check();//c220521
    void signal_device_connect_check_error();//c220525
    void signal_leftmenuSearch(QString);//cheon211008-search

    // 기기 연결 완료
    void signal_connected();
    // 로그인 완료
    void signal_logined();
    void signal_logined_tidal();
    // 로그인 실패
    void signal_login_fail();
    // 기기 전원 상태 변경
    void signal_devicePowerChanged();
    // 로즈 DB View To Table 완료
    void signal_finishedViewToTable();

    // AbstrackMainContent와 연결되어있음.. (매개변수 p_code : more/play/fav/addPlayList)
    void signal_clickedHoverItem(QString p_code, QJsonObject p_jsonData);   // 이 함수는 const 하면 안됨 : 수정되는부분있음.
    void signal_changeRoseTubeBG(const QPixmap &p_pixmap);
    void signal_changeRoseTubeBGURL(QString p_url);

    // 팟캐스트 채널 BG Show 요청 시그널
    void signal_showPodcastBG(const QPixmap &pixmap);

    void signal_showPlayScreenBG(const QPixmap &pixmap);

    void signal_clickedMovePage(const QJsonObject &p_jsonData);
    void signal_loginchageBugs();//cheon210617-http
    void signal_changedRadioFavorChannel(const bool &flagChangedInFavor);
    void signal_changedRadioUserChannel();

    // 검색 요청 시그널
    void signal_search(const QString &p_text);
    void signal_searchCopy(const QString &p_text);//c220904_1

    /// 검색바 포커스
    void signal_searchBarFocusChanged(const bool &p_flagHasFocus);

    /// 검색 제안어 변경 시그널
    void signal_searchSuggestionsChanged(const QJsonObject &p_jsonObject);

    // My 플레이리스트 변경 시그널
    void signal_myPlaylistChanged();

    // My 플레이리스트 트랙 변경 시그널
    void signal_myPlaylistTrackChanged();

    // Favorite Track 변경 시그널
    void signal_favoriteTrackChanged();

    // 네트워크 폴더 추가 시그널
    void signal_networkFolderAdded();

    // 즐겨찾기 폴더 변경 시그널
    void signal_favoriteFolderChanged();

    // DB 다운로드 요청 시그널
    void signal_requestDownloadDB();

    // Device State change signal
    void signal_change_device_state(const QString &category);

    // Music Top label data chagne signal
    void signal_completeDownloadDB();

    // message show timeout signal
    void signal_message_show_timeout();

    void signal_addPlaylistFinish();
    void signal_rosetubeRecentChanged();
    void signal_musicRecentChanged();

    void signal_rosefmChannelChanged(const QJsonObject &p_jsonObject);

    void signal_Video_movePage(const QJsonObject &p_jsonObject);
    void signal_Artist_movePage(const QJsonObject &p_jsonObject);
    void signal_clicked_movePage(const QJsonObject &P_jsonObjet);

    void signal_RoseHome_movePage(const QString &p_menu);

    void signal_httpserverQueueMsg(const QJsonObject &a);//c220402
    void signal_httpservermsg(const QJsonObject &a);//cheon210617-http
    void signal_httpservermsg_music_play_type_change();//c220408
    void signal_dragDrop();
    void signal_dragDrop_playlist();
    void signal_dragEnter();
};

#endif // LINKER_H
