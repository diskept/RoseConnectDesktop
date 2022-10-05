#ifndef MUSICLIST_ONE_H
#define MUSICLIST_ONE_H

#include <QLabel>
#include <QListWidget>
#include <QWidget>
#include <QSettings>

#include <common/filedownloader.h>
#include <common/linker.h>
#include <widget/customlistwidget.h>
#include <data/datapopup.h>
#include <delegate/filedelegate.h>
#include <music/proc_addtackinplaylist.h>
#include "widget/customlineedit.h"//cheon210601
//#include <QLabel>//cheon210601
#include <QElapsedTimer>//cheon210608-test
#include <QTimer>//cheon210608-test

/**
 * @brief 음악-분류 의 전체 곡 트랙 목록 @n
 *      DB 조회로 목록 구성 후 트랙 Favorite Get,Set API 를 통해 하트 구성
 */
class MusicList_One : public QWidget
{
    Q_OBJECT
public:
    explicit MusicList_One(QWidget *parent = nullptr);

    void setDataFromDB(const QString &p_arg);
    void setDataFromDB_Alpa(const QString &p_arg);
    void setDataFromDB();

public:

     QString go_page_text;//cheon210527
     QDialog *gdialog_wait;

signals:

private slots:
    void slot_thumbnailDownloaded();
    void slot_delegateClicked(const int &p_index, const int &p_btnType);
    void slot_playlistClicked(const int &p_playlistno);
    void slot_newPlaylistNameEdited(QString);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    //void slot_returnPressed(const QString p_text);//cheon210717-search
    void slot_returnPressed();//cheon210717-search
    //void slot_textChanged(QString);//cheon210717-search
    void slot_clickedChange();//cheon210717-search
    void slot_returnPressed_str();//cheon210717-search
    //void slot_textChanged(QString);//cheon210717-search
    void slot_clickedChange_str();//cheon210717-search
    void slot_clickedChange_alpa();//c0204
    void OnTimerCallbackFunc();//cheon210608-test

private:

    //---------------------------//cheon210601
    //CustomLineEdit *le_lisearch;//cheon210717-search
    QLineEdit *le_lisearch, *le_lisearch_str;//cheon210717-search
    QLabel *albumtCnt, *albumNone;//c220402
    //---------------------------------
    Linker *linker;

    QString strArg_1;
    QList<DataPopup*> list_audioInfo;

    FileDelegate *delegate;
    CustomListWidget *listWidget;

    Proc_AddTackInPlayList *proc_addPlaylist;

    bool flagReqMore = false;
    int next_OffsetPoint = 0;


    void setInit();
    void setUIControl();
    void getTrackDataFromDB();
    void getTrackDataFromDB_pre();
    //void getTrackDataFromDB_Alpa();
    //void getTrackDataFromDB_pre_Alpa();
    int getAlbumDataFromDBforFullsongTotalCount();
    void ContentLoadingwaitingMsgShow(QString msg);
    void ContentLoadingwaitingMsgHide();

    void goto_getFullsongDataFromDB(int p_page);
    //void goto_getFullsongDataFromDB_str(int p_page);
    void getTrackDataFromDBForSearchPage();
    void setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor);
    void changeTrackFavoritesUI(const int &p_index);
    void requestPlayMusic(const int &p_index);
    void requestShowOptionPopup(const int &p_index);
    void requestGetTrackFavorites(const int &p_index);
    void requestSetTrackFavorites(const int &p_index);
    QJsonObject getJsonObject_OptionPopup(const int &p_index);
    QJsonArray getJsonArr_TrackData(const int &p_index);
    QJsonObject getJsonObject_TrackData(const int &p_index);
    void setResultOfGetFavorite(const int &p_index, const QJsonObject &p_jsonData);
    void setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData);

    void wheelEvent(QWheelEvent *event) override;


    //---------------------------------------------------------------
    //QString al_str[26];
    //= {"A", "B", "C", "D", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

    int CNT = 0;
    int curr_page_next = 1;
    bool NreloadFlag = false;//cheon210608-test
    bool PreloadFlag = false;//cheon210608-test
    QThread *threadone;//cheon210608-test
    QTimer *workerTriggerone;//cheon210608-test
    QElapsedTimer Etimerone;//cheon210608-test

    int OffsetPoint = 0;

    int scroll_pos_currunt = 0;
    int scroll_pos_before = 0;
    //--------------------------------------cheon210601
    bool flag_page_first = false;

    int pre_lmt_cnt = 0;
  //  int OffsetPoint = 0;
    int total_page_cnt = 0;
    int curr_page_cnt = 1;
    int total_db_cnt = 0;

   // int scroll_pos_currunt = 0;
  //  int scroll_pos_before = 0;

    int currunt_album_page = 0;
    int before_album_page = 1;
    //---------------------------------------------------

    QSettings *settings;

};

#endif // MUSICLIST_ONE_H
