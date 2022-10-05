#ifndef QUEUEPLAYLIST_H
#define QUEUEPLAYLIST_H

#include "widget/clickablelabel.h"
#include "widget/dialogconfirm.h"
#include "widget/flowlayout.h"      //cheon211120-2
#include "widget/onoffwidget.h"     //cheon211120-2

#include "common/filedownloader.h"
#include "common/linker.h"

#include "data/datapopup.h"

#include "delegate/queueitemdelegate.h"
#include "widget/listwidget.h"

#include <QBuffer>
#include <QCheckBox>
#include <QCoreApplication>
#include <QJsonArray>
#include <QLabel>
#include <QListWidget>
#include <QMap>
#include <QMovie>
#include <QMutex>
#include <QProgressBar>
#include <QProgressDialog>
#include <QScrollBar>               //cheon211114-01
#include <QScrollArea>              //cheon211120-2
#include <QStackedWidget>
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>


#define  print_widget_func() qDebug() << "[WIDGET][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


/**
 * @brief The QueuePlayList class 재생목록(Queue List) 클래스
 */
class QueuePlayList : public QWidget
{
    Q_OBJECT
public:
    explicit QueuePlayList(QWidget *parent = nullptr);

    bool islistOpen = false;
    QScrollBar *scrollArea;//chwon211114-01

private:
    QPixmap pixmapCheckOn;  ///< 체크박스 On
    QPixmap pixmapCheckOff; ///< 체크박스 Off
    QPixmap pixmapDel;      ///< 휴지통
    QPixmap pixmapMore;     ///< 더보기 아이콘
    QPixmap pixmapNet;      ///< 음악의 네트워크 파일을 의미하는 아이콘
    QPixmap pixmapShare;      ///< 공유 모양 파일을 의미하는 아이콘
    QPixmap pixmapPlaying;  ///< 현재 재생중인 파일을 표현하는 아이콘
    QPixmap pixmapDefault;  ///< 현재 재생중인 파일을 표현하는 아이콘
    QPixmap pixmapFlac;
    QPixmap pixmapChange_list;

    /** * @brief The DataTrackforDelegate struct 데이터 구조체   */
    struct DataTrackforDelegate {
        enum Types_Data{
            Total
            , Music
            , RoseTube
            , Tidal
            , Bugs
            , Qobuz
            , AppleMusic
            , RecommLabel //cheon211122-01
            , Recomm //cheon211122-01
        };

        Types_Data type = Types_Data::Music;
        bool isDirect = false;      ///< 음악 네트워크 파일인지 유무
        bool isShare = false;      ///< 음악 공유 파일인지 유무 (친구 플레이리스트?)
        QString title = "";
        QString artist = "";
        int duration = 0;
        QString audioQuality = "";
        QString thumbnail = "";
        int index = -1;
        QString len ="";
        int no = -1;
        bool isPlay = false;        ///< 현재 재생중인 유무
        bool isDelChecked= false;
        quint64 addtimeResult = 0;
    };

    QThread *threadQueueOne;
    QTimer *TriggerQueuePlayPosition;

    int noTotal_currntPlay = -1;
    bool queue_init_flag = false;
    bool mouse_trigger_flag = false;
    QMutex mutex;
    //QLabel *lb_listBox;
    int press_btnType = -1;
    //QHBoxLayout *hl_listBoxlayout;
    QHBoxLayout *hl_listBoxlayout_recomm;
    QList<QLabel *> lb_queueListImg[10];
    QList<QLabel *> lb_queueListImg1;
    QList<QLabel *> lb_queueListImg_recomm[10];

    QLabel *lb_title;
    QString currMenu = "";          ///< 현재선택된 메뉴 값
    int indexCurrPlay[10] = {-1,};         ///< 현재 재생중인 곡의 index
    int indexPrePlay[10] = {-1,};
    bool isEditMode = false;        ///< 현재 편집모드 유무
    bool isMoveMode = false;
    bool isPlayMode = false;
    bool isAllSelected =false;      ///< 전체 체크선택 유무
    bool bugs_mv_enable = false;

    Linker *linker;//cheon211114-01
    QueueItemDelegate *delegate;
    //QListWidget *listWidget;
    QWidget *ww;
    QProgressDialog *progress;
    int numFiles =0;
    QueueItemDelegate *delegate_recomm;//cheon211114-01
    //QListWidget *listWidget_recomm;//cheon211114-01
    QWidget *widget_top_recomm;//cheon211120-2
    QLabel *lb_title_recomm;
    FlowLayout *fl_queueList;//cheon211120-2
    QWidget *widget_scrollArea;//cheon211120-2
    QScrollArea *scrollArea11;

    // 상단 버튼
    QPushButton *btn_ok, *btn_moveok, *btn_delok, *btn_del, *btn_PlayPosition;
    //QPushButton *btn_play;
    QPushButton *btn_edit, *btn_mov, *btn_play;
    QCheckBox *checkbox_allSelected;
    QList<QCheckBox *> checkbox_Selected[10];
    QWidget *widget_editMode, *widget_moveMode, *widget_playMode;

    //QLabel *lb_queueListImgCheckOff;
    QLabel *lb_queueListImgDel;

    //ClickableLabel *btn_edit;
    //ClickableLabel *btn_mov;
    // 메인메뉴
    ClickableLabel *clb_total;
    ClickableLabel *clb_music;
    ClickableLabel *clb_video;
    ClickableLabel *clb_fmtuner;
    ClickableLabel *clb_radio;
    ClickableLabel *clb_roseTube;
    ClickableLabel *clb_potcast;
    ClickableLabel *clb_tidal;
    ClickableLabel *clb_bugs;
    ClickableLabel *clb_qobuz;
    ClickableLabel *clb_apple;

    QList<DataTrackforDelegate> list_dataTrackForDelegate[10];
    QList<DataTrackforDelegate> list_dataTrackForDelegate_tmp;
    QList<DataTrackforDelegate> list_dataTrackForDelegate_total;

    QJsonArray jsonArray_backup_music;      ///< 백업용//cheon210916-total
    QJsonArray jsonArray_backup_youtube;        ///< 백업용
    QJsonArray jsonArray_backup_tidal;          ///< 백업용
    QJsonArray jsonArray_backup_bugs;           ///< 백업용
    QJsonArray jsonArray_backup_qobuz;          ///< 백업용
    QJsonArray jsonArray_backup_apple;          ///< 백업용
    QJsonArray jsonArray_backup_total[10];          ///< 백업용

    QJsonArray jsonArray_backup_music_recomm;      ///< 백업용//cheon210916-total
    QJsonArray jsonArray_backup_youtube_recomm;        ///< 백업용
    QJsonArray jsonArray_backup_tidal_recomm;          ///< 백업용
    QJsonArray jsonArray_backup_bugs_recomm;           ///< 백업용
    QJsonArray jsonArray_backup_qobuz_recomm;          ///< 백업용
    QJsonArray jsonArray_backup_apple_recomm;          ///< 백업용
    QJsonArray jsonArray_backup_total_recomm[10];          ///< 백업용
/*
    //    bool flagForcedStop = false;
    int current_queue_cnt = -1;//cheon211122-02
    int current_musicqueue_cnt = 0;//cheon211122-02
    int current_qobuzqueue_cnt = 0;//cheon211122-02
    int current_tidalqueue_cnt = 0;//cheon211122-02
    int current_bugsqueue_cnt = 0;//cheon211122-02
    int current_rosetubequeue_cnt = 0;//cheon211122-02
    int current_applequeue_cnt = 0;//c220304
*/
    int recomCurr_selIndex = -1;
    QString checkSum = "";
    int nextPlayNo = -1;
    int CurrPlayNo = -1;
    int checking_curr_play_index = -1;
    int local_index = 0;
    int local_max = 0;
    bool indexCurrPlay_reflash_flag = false;    //cheon211120-3
    int bar_pos = 0;
    bool recomm_flag = false;                   //cheon211114-01

    int indexStartSetThumb = 0;
    int cnt_setThumb = 0;                       ///< 현재까지 세팅완료한 썸네일 갯수
    int cnt_maxThumb = 0;                       ///< MAX만큼 요청할 썸네일 갯수

    int indexStartSetThumb2 = 0;
    int cnt_setThumb2 = 0;                       ///< 현재까지 세팅완료한 썸네일 갯수
    int cnt_maxThumb2 = 0;                       ///< MAX만큼 요청할 썸네일 갯수

    int indexStartSetListWidget = 0;
    int cnt_setListWidget = 0;                  ///< 현재까지 세팅완료한 썸네일 갯수
    int cnt_maxListWidget = 0;                  ///< MAX만큼 요청할 썸네일 갯수

    QList<QString> list_thumbPath[10];
    int current_queue_cnt = -1;

    /*
    int tidal_queue_list_count = 0;             //cheon210916-total
    int qobuz_queue_list_count = 0;             //cheon210916-total
    int bugs_queue_list_count = 0;              //cheon210916-total
    int music_queue_list_count = 0;             //cheon210916-total
    int rosetube_queue_list_count = 0;          //cheon210916-total
    int applemusic_queue_list_count = 0;
    int total_queue_list_count = 0;             //cheon210916-total
*/
    int tidal_queue_list_count_recomm = 0;      //cheon210916-total
    int qobuz_queue_list_count_recomm = 0;      //cheon210916-total
    int bugs_queue_list_count_recomm = 0;       //cheon210916-total
    int music_queue_list_count_recomm = 0;      //cheon210916-total
    int rosetube_queue_list_count_recomm = 0;   //cheon210916-total
    int applemusic_queue_list_count_recomm = 0;
    int total_queue_list_count_recomm = 0;      //cheon210916-total

    QJsonObject totalQueue_backup;              //cheon211102
    QJsonObject recomm_totalQueue_backup;       //cheon211102

    QJsonArray jsonArr_deletePositon;
    QJsonArray jsonArr_editPositon;
    bool showEvent_showFlag = false;
    QPushButton *lb_onOfftitle;                 //cheon211120-2
    OnOffWidget *onOff;

    QList<DataTrackforDelegate> list_dataTrackForDelegate_recomm[10];
    int indexStartSetThumb_recomm = 0;
    int cnt_setThumb_recomm = 0;           ///< 현재까지 세팅완료한 썸네일 갯수
    int cnt_maxThumb_recomm = 0;           ///< MAX만큼 요청할 썸네일 갯수
    QList<QString> list_thumbPath_recomm[10];

    QJsonArray jsonArray_backup_Total;
    QList<QString> list_thumbPath_Total;
    QList<DataTrackforDelegate> list_dataTrackForDelegate_Total;

    QHash<QString,QPixmap> queueHash;
    QHash<QString, QPixmap>::const_iterator it = queueHash.constBegin();
    QHash<QString, QPixmap>::const_iterator itEnd = queueHash.constEnd();

    //QMap <QString, QPixmap> que_map;

    QVBoxLayout *box_contents;
    QProgressBar *queProgressBar = new QProgressBar;
    //-----------------------------------------end
    QPixmap *pixmap_type_music;
    QPixmap *pixmap_type_video;
    QPixmap *pixmap_type_podcast;
    QPixmap *pixmap_type_rosetube;
    QPixmap *pixmap_type_tidal;
    QPixmap *pixmap_type_bugs;
    QPixmap *pixmap_type_qobuz;
    QPixmap *pixmap_type_apple;
    QPixmap *pixmap_type_cloud;
    QPixmap *pixmap_type_network;
    QPixmap *pixmap_type_local;
    QPixmap *pixmap_type_audioQuality;
    QPixmap *pixmap_type_mqa;
    QPixmap *pixmap_type_flac;

    QDialog *dialog_wait;

    //----------------------------------------------------------
    QList<QWidget *> widget[10];
    QList<QLabel *> Img_widget;
    QList<QWidget *> widget1;
    QList<QWidget *> widget2;
    QList<QWidget *> widget_recomm[10];
    QWidget *widget_tmp;
    QList<QPixmap > listPixmap;

    //QList<QPixmap > backupPixmapArrayTotal;
    QList<QPixmap > backupPixmapArrayBugs;
    QList<QPixmap > backupPixmapArrayTidal;
    QList<QPixmap > backupPixmapArrayQobuz;
    QList<QPixmap > backupPixmapArrayApple;
    QList<QPixmap > backupPixmapArrayRoseTube;
    QList<QPixmap > backupPixmapArrayMusic;

    QMap <QString, QString> que_map;
    //----------------------------
    int preSelectedMenu = -1;
    int currSelectedMenu = -1;
    QString preMenu = "";
    bool queMode = true;
    //----------------------------------------------------------

    //--------------------------------------------------
    bool force_reloadList_flag = false;
    int playAfterDeleteIndex = -1;
    QString preJsonStr = "", preJsonStr_recomm = "";
    QString currJsonStr = "";
    bool equelRecieveJsonFlag[10] = {false,}, recomm_equelRecieveJsonFlag = false;
    bool equelDelegateFlag = false;
    QString selected_menuSave = "";
    //--------------------------------------------------------
    QLabel *lb_listBox_title;
    QLabel *lb_listBox_artist;
    QLabel *lb_listBox_type;
    QLabel *lb_list_duration;
    QLabel *lb_list_addtimeResult;
    QLabel *lb_list_audioQuality;
    QLabel *lb_queueListImgPlaying;
    QLabel *lb_queueListImgDefault;
    QLabel *lb_Change_list;
    //----------------------------------------------------------
    QMetaObject::Connection disConSelectId[3000];
    QMetaObject::Connection disConId[3000];
    QMetaObject::Connection disConId_recomm[1000];
    QMetaObject::Connection dis_checking_currPlay_conn;
    QTimer *timer, *timer_reGetCeckSum;
    int disConId_cnt = 0;
    int disConIdRecomm_cnt = 0;
    bool forceReflashFlag = false;
    int pre_position = -1;
    bool mouse_trigger_menu_flag = true;
    bool mouse_trigger_menuClick_flag = false;
    //bool normal_reflash_call_flag = false;
    bool music_start_positionGet_flag = false;
    bool force_newLoad_flag = false;
    int drop_cnt_flag = 0;
    DialogConfirm *dlgConfirmQueue;
    int precurrisMode = 0;
    QLabel *lb_title_move, *lb_title_del, *lb_title_cnt;
    QLabel *lb_position_site, *lb_position_title, *lb_position_title_name, *lb_position_label;
    QWidget *widget_title, *widget_position_msg;
    int list_thumbPath_cnt = 0, list_thumbPath_cnt_2 = 0, list_thumbPath_cnt2 = 0;
    bool downloadThumbImage_ing_flag = false;
    bool playCurr_recomm_run_flag = false;
    bool playCurr_run_flag = false;
    bool equelLoadRecieveJsonFlag = false;

    bool isPlaying_flag = false;
    bool only_replace_flag = false;
    //bool delete_PixmapArray_flag = false;
    int indexCurrPlay_start_cnt = 0;
    int save_indexCurrPlay[10] = {-1,};
    int old_indexPlayNo[10] = {-1,};
    int pre_current_queue_cnt = 0;
    bool after_move_queuelist_flag = false;
    bool http_signal_flag = false;
    bool click_slot_clickedPlay_flag = false;
    bool click_slot_clickedDelete_flag = false;
    int tmp_Onecnt = 0;
    //bool only_curr_replace_flag = false;
    int CurrViewRow = 0, PreViewRow = 0;
    QString pre_checkSum = "";
    int tabSectionlist[7] = {0,1,4,6,7,8,9};
    int startIndex = -1;
    QMutex mtx;

    //QLabel *lb_listBox_title_tmp , *lb_listBox_title_tmp_true;
    //QHBoxLayout *hl_listBoxlayout;
    //--------------------------------------
    QVBoxLayout *vl_list_titlelayout_lineItem;
    QHBoxLayout *hl_listBoxlayout_lineItem;
    QVBoxLayout *vl_list_titlelayout2_lineItem;
    QWidget *widget_title_lineItem;
    QHBoxLayout *hl_listl3layout_lineItem;
    QWidget *widget_listl3layout_lineItem;

    QListWidget *listWidget_total[10];
    int pre_now_Section = -1;
    int curr_sliderPosition = 0;
    int pre_sliderPosition = 0;
    bool force_play_mode = false;

    QStackedWidget *stackWidget_queue;//c220625
    QWidget *temp_menuWidget;



signals:
    void signal_valueChanged(int);
    void signal_nornalQueuelistloaded(int);//cheon211120-2
    void signal_editmode(QListWidgetItem *);
    void signal_playCurr_recomm();
    void signal_downloadThumbImageMore();
    void signal_queuelist_GetcurrPlayIndex();
    void signal_assignImg();
    void signal_assignImg0();
    void signal_music_start();
    void signal_for_view_std_listWidget_one_lineItemUI();

private slots:

    void requestRecom_check(const int &p_id);
    void slot_downloadThumbImage();
    void slot_clickedDeleteOk();
    void slot_changedQueueSubMenu();
    void slot_changedQueueMenu();//----------------------------------
    void slot_queue_question_popup();
    void slot_showDlgOfQueueNotice();
    void slot_editModeClicked(QListWidgetItem *item);
    void slot_queuelistDelay();
    void slot_checking_currPlay();
    void slot_queuelistPositionGet_checking_currPlay();
    void slot_queuelistScrollbarMotify(const int &p_id);//--------------------------------------
    void slot_queuelistScrollbarMotify_menuChange(const int &p_id);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_responseHttp_checksum(const int &p_id, const QJsonObject &p_jsonObject);//cheon210916-total
    void slot_responseHttp_checksum_showEvent(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_responseHttp_get(const int &p_id, const QJsonObject &p_jsonObject);//cheon211104-01
    void slot_responseHttp_music(const int &p_id, const QJsonObject &p_jsonObject);//c1209
    void slot_responseHttp_motify(const int& p_id, const QJsonObject& p_jsonObj);
    void slot_responseHttp_playbar(const int& p_id, const QJsonObject& p_jsonObj);
    void slot_responseHttp_play(const int &p_id, const QJsonObject &p_jsonObject);//cheon210916-total
    void slot_responseHttp_delete(const int& p_id, const QJsonObject& p_jsonObj);//cheon210916-total
    //void slot_delegateClicked(const int &p_index,const int &p_btnType);

    void slot_thumbnailDownloaded();
    void slot_thumbnailDownloaded_playbarChanged();
    void slot_thumbnailDownloaded_playbarChanged_section();

    //void slot_thumbnailDownloadedOne();
    void slot_clickedEdit();
    void slot_clickedMove();
    void slot_clickedPlay();
    void slot_clickedDelete();
    void slot_clickedAllSelected();

    void slot_sliderMoved_notifySliderMove(int index);//cheon211120-2
    void slot_valueChanged_notifySliderMove(int index);//cheon211120-2
    void slot_modeOnOff(bool b);//cheon211120-02
    void slot_responseHttp_playRelateState(const int &p_id, const QJsonObject &p_jsonObject);//cheon211122-01

    void slot_listwidgetItemClicked(QListWidgetItem *item);
    //void slot_delegateClicked_recomm(const int &p_index,const int &p_btnType);
    void slot_thumbnailDownloaded_recomm();//cheon211114-01
    void slot_responseHttp_recomm(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_responseHttp_recomm_check(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_responseHttp_recomm_only(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_httpservermsg_queue_play(const QJsonObject &p_jsonObject);
    void slot_queueListImgCheckOff();
    void slot_queuelistMotify();
    void slot_queuelistPositionGet();
    void slot_queuelistPositionGet_check();
    void slot_queuelistPositionGet_http_signal();
    void slot_queuelistPositionGet_http_signal_singhot();//c220625
    void slot_queue_recent_track_add_http_signal();//c220625
    void slot_responseHttp_recomm_createlist(const int &p_id);
    void slot_responseHttp_recomm_createlist_only(const int &p_id);
    void slot_playCurr_recomm();


    void slot_httpservermsg(const QJsonObject &p_jsonObject);//c211213
    void slot_queuelistCurrPlayIndexGet();
    void slot_queuelistCurrPlayIndexGet_singleshot();//c220603

    void slot_listwidgetItemActivated(QListWidgetItem *item);
    void slot_listwidgetItemEntered(QListWidgetItem *item);
    void slot_listwidgetcurrentItemChanged(QListWidgetItem *item, QListWidgetItem *item2);
    void slot_listwidgetItemPressed(QListWidgetItem *item);
    void slot_sliderReleased();
    void slot_listwidgetcurrentRowChanged(int row);//c220620

    void slot_listwidgetcurrentRowChanged_start(int row);//c220620

    void slot_listwidgetcurrentRowChanged();//c220620
    void slot_dragAndDropEdit();
    void slot_dragAndDropLoop();
    void slot_dragAndDropEnter();

    void slot_clickedSelected(int s);
    void slot_assignImg();
    void slot_assignImg0();
    void slot_setWidgetColor(int i);
    void slot_setWidgetColor_recomm(int i);
    void slot_reGet_checkSum();
    void slot_for_view_std_listWidget_one_lineItemUI();

private:
    void responseHttp_flagOkFalseMsg();//c221004_1
    void listwidgetcurrentRow_append(const int &p_index);
    void listWidget_total_setShow(const int &no);//c220625
    ListWidget *setListWidget();
    void setChangMenuStyling(QString curr_nemu_play);
    void itemPressesBackground(int i);
    void itemPressesBackground_recomm(int i);
    void queuelist_change_replace(bool flag, int menu_count);//-----------------------
    void queuelist_change_replace_recovery(int no);

    QJsonDocument  loadJson(QString fileName);
    void saveJson(QJsonDocument document, QString fileName);
    void loadingGif();
    bool check_func(const int &p_id, const int &menu_count);
    void queue_setListWidget(int i);
    void menu_init();//c220629
    void pixmap_init();//c220629
    void menu_click_init();//c220629
    void cal_indexStartSetThumb_forSpeedLoop();
    void for_view_std_listWidget_one_lineItemUI(const int &p_id);
    void view_listWidget_setItemWidget(const int &p_id);//----------------------------------
    void view_listWidget_setItemWidget_forMenu(int queue_list_cnt);//-----------------

    inline void std_listWidget_one_lineItemUI(const int &p_index);//-------------

    void showEvent_after_reGet_checkSum();
    void RemoveLayout (QWidget* widget);
    void reGet_checkSum();
    bool queueList_delDialogConfirm();
    bool queueList_draDropEditDialogConfirm();
    void dlgOutputQueueQuestion();

    QPixmap* getUIPixmapImg(QString p_url);

    int get_fromNextIndexNo_toCurrIndexPlay();
    int get_fromABSNextIndexNo_toSUBIndex(const int &p_index);
    int get_IndexCurrPlayfromNextIndexNo(const int &p_index);
    void std_listWidget_one_line_replace(const int &p_index);
    void std_listWidget_one_line_replace_only(const int &p_index);
    void std_listWidget_one_line_replace_only(const int &section, const int &p_index);
    void view_listWidget_setItemWidget_replace(const int &p_index, const int &index_old);
    void view_listWidget_setItemWidget_replace_only(const int &p_index, const int &index_old);

    bool queueReflash_checking(int sel);//-------------------------------------------------------------------

    void mouse_all_clear();
    void mouse_all_lock();
    void mouse_all_semiLock();
    void mouse_clickable_state_print();
    void append_widget();

    void downloadThumbImageMore_playbarChanged(int);
    void downloadThumbImageMore_playbarChanged(int, int);

    void downloadThumbImageMore();//----------------------------------------------

    void downloadThumbImageMore_recomm();


    void playCurr(int p_index);//---------------------------------------------------------

    QWidget *getUIControlOption(QString p_title);//cheon211120-2
    void mythreadFunc();
    void reloadList();
    void force_reflash(int p_id);
    void force_reflashexceptRecomm(int p_id);
    int currMenutoPid();

    bool TotalQueueListCount_afterGetFile(const int &p_id, const QJsonObject &p_jsonObject);
    void TotalQueueListCount_afterGetFile_recomm(const int &p_id, const QJsonObject &p_jsonObject);

    void setTotalQueueList(int p_id, const QJsonObject &p_jsonObject);//c220609

    void setTotalQueueList_recomm(int p_id, const QJsonObject &p_jsonObject);//c220609

    void listWidget_recommTitledrawingUI();

    //void downloadThumbImageMore_recomm();
    void appendTrackUI_recomm(const int &p_index);

    void view_listWidget_setItemWidget_recomm(const int &p_id);
    void view_listWidget_setItemWidget_recomm_only(const int &p_id);

    void std_listWidget_one_line_recomm(const int &p_index);
    void playCurr_recomm(int p_index);

    void responseQueueList_checksum(const QJsonObject &p_jsonObject);//c220609
    void appendTrackUI(const int &p_index);

    void request_musicPlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj);//cheon210916-total
    void request_musicPlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr);//cheon210916-total
    void request_rosetubePlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj);//cheon210916-total
    void request_rosetubePlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr);//cheon210916-total
    void request_totalPlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj);//cheon210916-total
    void request_totalPlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr);//cheon210916-total
    void oneTrackDelete(int p_index);

    void myFunction(QModelIndex Sindex,int S,int E,QModelIndex Dindex ,int Drow);

    void changeEditMode(bool p_isEditMode, bool p_isMoveMode, bool p_isPlayMode);

    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

};

#endif // QUEUEPLAYLIST_H
