#ifndef SECTIONBOTTOM_H
#define SECTIONBOTTOM_H

#include "home/sectionbottomSlider.h"

#include "common/linker.h"
#include "common/filedownloader.h"

#include "widget/clickablelabel.h"
#include "widget/dialogconfirm.h"
#include "widget/dialogWakeUp.h"

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>
#include "versionupdate/Updater.h"//c220411
#include "versionupdate/QSimpleUpdater.h"//c220411
#include "thread/desktopmediaplayerthread.h"//c220705



#define print_home_func() qDebug() << "[HOME][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


/**
 * @brief The SectionBottom class 하단 재생바 @n
 * @note
 *      전체 영역 클릭시 @sa PlayFullScreenRelation 전체 재생페이지로 이동 @n
 *      음원 타입에 따라 shuffle 아이콘 비활성
 */
class SectionBottom : public QWidget
{
    Q_OBJECT
public:
    explicit SectionBottom(QWidget *parent = nullptr);
    ~SectionBottom();//c220705

    void setDataSectionBottom(QJsonObject p_jsonObject);
    void setCurrPlayValue(int p_curr_playValue);
    void setInitUIQueueIcon();
    void setInitUIVolumnIcon();
    void emitSinalCurrPlayDataInfo();

    QJsonObject getData();

signals:
    //void clickedBtn(QString p_btnCode);
    void clickedQueuePlayList(bool p_flagShow);
    void clickedVolumn(bool p_flagShow);
    //void signal_finishPlay();

    void signal_showPlayFullScreen(const QJsonObject &p_jsonData);
    void signal_changedCurrPlayTrack(const QJsonObject &p_jsonData);
    void signal_changedCurrHDMI(const bool flag);

private slots:
    void slot_Update_call();//c221013_1
    void slot_clickedYoutubeQuestion();//c220803
    void slot_player_valuestop();//c220705
    //void slot_playMusic_desktop(QJsonArray jsonArrMusic, bool flagPlayShuffle);//c220704
    void slot_desktopModeStart();//c220704
    void slot_deviceModeStart();//c220609
    void slot_checkWindow_hide();//c220401
    void slot_checkWindow_show();//c220401
    void slot_sliderClick(int p_value);
    void slot_loadImage();
    void clickedBtn();
    void slot_deviceModetimePlayValue();
    void slot_responseHttp_state(const int &p_id, const QJsonObject &p_jsonObject);//c220609
    void slot_responseHttp_localDriverPlayer(const QJsonObject &p_jsonObject, QPixmap pixmap);//c220609
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_showPlayFullScreen();
    void changedSliderBar();
    void changedSliderBar(int p_value);

    void slot_redirectUrl(const QString url);   //j220903 twitter
    void slot_checkQueueMsg(int,QString);//c211213

    void slot_volume_change(int&);
    void slot_muteState_change(int&);
    void slot_volume_widget_hide();
    void dialog_comfirmCD();

private:

    QLabel *lb_msg_show;//c230303_2
    QDialog *msg_dialog_wait;//c230303_2

    long latestCheckTimeSecond=0;
    long latestCheckTimeSecond_connect=0;
    long latestCheckTimeSecond_connect5sec=0;
    long latestCheckTimeSecond_connect10sec=0;//c230522
    long latestCheckTimeSecond_connect60sec=0;//c230315_1
    long latestCheckTimeSecond_connectloginsec=0;//c230422

    void ContentLoadingwaitingMsg_init();//c230303_2
    void ContentLoadingwaitingAni_init();//c230303_2

    void ContentLoadingwaitingAniShow(QString msg, int flag);
    void ContentLoadingwaitingMsgShow(QString msg, int flag);//c211213
    void setJsonData(QJsonObject p_jsonObject);
    void setUIDataBasicInfo();
    void setUIDataPlayIcon();
    void nextSongPlay();//c220705


    bool tmp_window_activate_flag = false;
    bool flag_response_proc = false;//c220709
    DesktopMediaPlayerThread *Dt_thread;//c220705
    Updater *m_updater_tmp;
    bool slot_sliderClick_flag = false;
    // bool section_bottom_mouse_trigger_menu_flag = false;//c220625
    bool showHideCheckFlag = false;
    QString thumbnail="";              ///< 앨범 이미지 경로
    bool isIsoOpen = false;

    int curPosition = 0;            ///< 현재 재생 시점 value
    int duration = -1;               ///< 재생 길이
    QString playType = "";
    bool isHdmiOn = false;
    bool isPlaying = false, device_playingState = false;
    bool isServer = false;          ///< 네트워크 파일인지 유무 인듯.. (마이/친구/폴더 파일인경우에만 true)
    bool isShareFile = false;       ///< 정확히 뭔지 모르겠지만,,어디선가 공유했다는 의미의 bool 인데.. 무조건 친구플레이리스트라 해서 true 도 아니라 정확한 의미는 모르겠음.
    int repeatMode = 0;
    int shuffleMode = 0;
    QString audio_id = "";          ///< 오디오 PK
    int album_id = 0;
    QString artist_id = "";
    QString iconPath = "";
    QJsonObject data_jsonObject;

    QTimer *timer;
    QPixmap *pixmap_albumImg;





    //QWidget *widget_barValue;
    //sectionbottomSlider *slider_playbar;
    QSlider *slider_playbar;
    QWidget *wg_playbar_cur;
    FileDownloader *filedownloader;
    QWidget *widget_total;
    QPushButton* getUIBtn(QString p_url, QString p_property);
    void resizeEvent(QResizeEvent* event);
    void setBackgroundAlbumImg();

    QLabel *lb_albumbg;
    ClickableLabel *lb_bgOpacity;

    ClickableLabel *lb_albumThumb;
    QLabel *lb_albumThumbText;

    ClickableLabel *lb_icon_playType;
    ClickableLabel *lb_title;
    ClickableLabel *lb_artistName;
    ClickableLabel *lb_trackinfo;


    QPushButton *btn_rep;
    QPushButton *btn_back;
    QPushButton *btn_pause;
    QPushButton *btn_next;
    QPushButton *btn_shu;
    QPushButton *btn_list;
    QPushButton *btn_sound;

    QLabel *lb_rep_img;
    QLabel *lb_back_img;
    QLabel *lb_pause_img;
    QLabel *lb_next_img;
    QLabel *lb_shu_img;
    QLabel *lb_list_img;
    QLabel *lb_sound_img;
    QLabel *lb_sound_text;

    QString thumbnail_realHttp="";      ///< 실제 세팅할 시점에만 실제 세팅한 이미지 풀경로를 저장하고 있는다 (동일한 앨범이미지를 계속 세팅할 필요 없게 하기위해 변수 추가)
    QString titleName="";               ///< 트랙 타이들
    QString albumName="";               ///< 앨범 타이틀
    QString artistName="";              ///< 아티스트 네임
    QString trackinfo="";               ///< 트랙 해상도

    bool flagSelectedQueue = false;     ///< 큐 목록 버튼 클릭 유무
    bool flagSelectedVolumn = false;    ///< 볼륨 버튼 클릭 유무
    bool flagMuteState = false;         ///< Mute 상태 유무

    QDialog *dialog_wait;//c211213
    //QMovie *mov;
    bool bottomInfo_flag = false;

    // ------------------------------
    //  Sleep Check Dlg
    // ------------------------------
    DialogConfirm *dlg;
    DialogWakeUp *dlgWakeUp;

    QLabel *lb_playbar_cur, *lb_playbar_duration;

    int ui_state = 0;                   //c230327



    //------------------------------------------------------------------------------------
    // for Tidal, Bugs
    //------------------------------------------------------------------------------------
private:
    Linker *linker;
    QJsonObject jsonObj_subAppCurrentData;                      // Tidal, Bugs는 이 정보가 필요함. 멤버 변수로 저장함
    bool tidal_flag = false;

    QString cd_img_before = "";        //j220903 twitter
    QString cd_img_change = "";        //j220903 twitter
};

#endif // SECTIONBOTTOM_H
