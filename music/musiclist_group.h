#ifndef MUSICLIST_GROUP_H
#define MUSICLIST_GROUP_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include <QSettings>
#include <QListWidget>
#include <QCoreApplication>


namespace music {
    /**
     * @brief 음악-프레임 위젯 목록(앨범,아티스트,플레이리스트) @n
     *      @sa FrameWidget 목록 화면
     */
    class MusicList_Group : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        enum GroupMode {
            /// 음악>분류>장르
            KIND,
            /// 음악>분류>아티스트
            ARTIST,
            /// 음악>분류>작곡가
            COMPOSER,
            /// 음악>홈>View All : 최근 재생 앨범
            VIEWALL_PLAYALBUM,
            /// 음악>홈>View All : 최근 재생 플레이리스트
            VIEWALL_PLAYPLAYLIST,
            /// 음악>홈>View All : 최근 추가 된 앨범
            VIEWALL_ADDEDALBUM,
            /// 음악>홈>View All : My 플레이리스트
            VIEWALL_MYPLAYLIST,
            /// 음악>홈>View All : 친구 플레이리스트
            VIEWALL_FRIENDPLAYLIST,
            /// 음악>홈>View All : 추천 아티스트
            VIEWALL_RECOMMENDARTIST,
            /// 음악>검색>View All : 앨범
            VIEWALL_SEARCHALBUM,
            /// 음악>검색>View All : 아티스트
            VIEWALL_SEARCHARTIST
        };

        explicit MusicList_Group(GroupMode p_mode,QWidget *parent = nullptr);
        explicit MusicList_Group(QString p_strArg, GroupMode p_mode,QWidget *parent = nullptr);
        ~MusicList_Group();

        void setDataFromDB(const QString &p_arg);
        void setDataFromDB();

    protected:
        void proc_wheelEvent_to_getMoreData() override;

        void resizeEvent(QResizeEvent *event) override;

    private:
        void setUIControl();

        int getDataFromDBforGenreTotalCount();
        int getDataFromDBforArtistTotalCount();
        int getDataFromDBforComposerTotalCount();
        int getDataFromDBforRecentlyAddTotalCount();

        void setDataGenreFromDB();
        void setDataArtistFromDB();
        void setDataComposerFromDB();
        void setDataAddedAlbumFromDB();

    private:
        GroupMode groupMode;
        QString strArg_1;

        FlowLayout *flowLayout_items;    ///< items

        QJsonArray jsonArr_Item;

        int widget_cnt = 0;

        int next_offset = 0;
        int total_cnt = 0;
        int draw_cnt = 0;

        bool flag_flow_draw = false;
        bool flag_item_draw = false;
    };
};



//#include <QScrollArea>
//#include <QVBoxLayout>
//#include <QWidget>
//#include <QSettings>

//#include <widget/flowlayout.h>

//#include <common/linker.h>
//#include "widget/customlineedit.h"//cheon210601
//#include <QLabel>//cheon210601
//#include <QElapsedTimer>//cheon210608-test
//#include <QTimer>//cheon210608-test
//#include <QTextEdit>//cheon210717-search
///**
// * @brief The MusicList_One class 앨범 리스트 클래스
// */

///**
// * @brief 음악-프레임 위젯 목록(앨범,아티스트,플레이리스트) @n
// *      @sa FrameWidget 목록 화면
// */
//class MusicList_Group : public QWidget
//{
//    Q_OBJECT
//public:
//    enum GroupMode {
//        /// 음악>분류>장르
//        KIND,
//        /// 음악>분류>아티스트
//        ARTIST,
//        /// 음악>분류>작곡가
//        COMPOSER,
//        /// 음악>홈>View All : 최근 재생 앨범
//        VIEWALL_PLAYALBUM,
//        /// 음악>홈>View All : 최근 재생 플레이리스트
//        VIEWALL_PLAYPLAYLIST,
//        /// 음악>홈>View All : 최근 추가 된 앨범
//        VIEWALL_ADDEDALBUM,
//        /// 음악>홈>View All : My 플레이리스트
//        VIEWALL_MYPLAYLIST,
//        /// 음악>홈>View All : 친구 플레이리스트
//        VIEWALL_FRIENDPLAYLIST,
//        /// 음악>홈>View All : 추천 아티스트
//        VIEWALL_RECOMMENDARTIST,
//        /// 음악>검색>View All : 앨범
//        VIEWALL_SEARCHALBUM,
//        /// 음악>검색>View All : 아티스트
//        VIEWALL_SEARCHARTIST
//    };

//    explicit MusicList_Group(GroupMode p_mode,QWidget *parent = nullptr);
//    explicit MusicList_Group(QString p_strArg, GroupMode p_mode,QWidget *parent = nullptr);
//    void setDataFromDB(const QString &p_arg);
//    void setDataFromDB();
//    int getAlbumDataFromDBforGenreTotalCount();//cheon210601
//    int getAlbumDataFromDBforArtistTotalCount();//cheon210601
//    int getAlbumDataFromDBforComposerTotalCount();//cheon210601
//    void goto_getGenreDataFromDB(int tmp_page);
//    void goto_getArtistDataFromDB(int tmp_page);
//    void goto_getComposerDataFromDB(int tmp_page);


//signals:
//private slots:
//    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
//    void slot_myPlaylistChanged();
//    //void slot_returnPressed(const QString p_text);//cheon210717-search
//    void slot_returnPressed();//cheon210717-search
//    //void slot_textChanged(QString);//cheon210717-search
//    void slot_clickedChange();//cheon210717-search
//    void slot_returnPressed_str();//cheon210717-search
//    //void slot_textChanged(QString);//cheon210717-search
//    void slot_clickedChange_str();//cheon210717-search

//    void OnTimerCallbackFunc2();//cheon210608-test


//private:
//    long kindEtime = 0;
//    QThread *threadartist;//cheon210608-test
//    QTimer *workerTriggerartist;//cheon210608-test
//    QElapsedTimer Etimerartist;//cheon210608-test
//    bool NreloadFlag = false;//cheon210608-test
//    bool PreloadFlag = false;//cheon210608-test

//    QThread *thread2;//cheon210608-test
//    QTimer *workerTrigger2;//cheon210608-test
//    QElapsedTimer Etimer2;//cheon210608-test
//    bool NreloadFlag2 = false;//cheon210608-test
//    bool PreloadFlag2 = false;//cheon210608-test

//    QThread *thread3;//cheon210608-test
//    QTimer *workerTrigger3;//cheon210608-test
//    QElapsedTimer Etimer3;//cheon210608-test
//    bool NreloadFlag3 = false;//cheon210608-test
//    bool PreloadFlag3 = false;//cheon210608-test

//    //---------------------------//cheon210601
//    //CustomLineEdit *le_lisearch;//cheon210717-search
//    QLineEdit *le_lisearch, *le_lisearch_str;//cheon210717-search
//    QLabel *albumtCnt;//cheon210601
//    int CNT = 0;


//    bool flagReqMore = false;//cheon210607-test
//    Linker *linker;

//    GroupMode groupMode;

//    QString strArg_1;

//    QWidget *widget_scrollArea;
//    QScrollArea *scrollArea;
//    FlowLayout *fl_albums;

//    void setInit();
//    void setUIControl();
//    void setDataGenreFromDB();
//    void setDataGenreFromDB_pre();
//    void setDataArtistFromDB();
//    void setDataArtistFromDB_pre();
//    void setDataComposerFromDB();
//    void setDataComposerFromDB_pre();
//    void setDataViewAllPlayAlbumFromAPI();
//    void setDataViewAllPlayPlaylistFromAPI();
//    void setDataViewAllAddedAlbumFromDB();
//    void setDataViewAllMyPlayListFromAPI();
//    void setDataViewAllFriendPlayListFromAPI();
//    void setDataViewAllArtistFromDB();
//    void setDataViewAllSearchAlbumFromDB();
//    void setDataViewAllSearchArtistFromDB();
//    void getDataFromDB();
//    void getDataFromDB_pre();

//    void setResultOfRecentAlbum(const QJsonObject &p_jsonObject);
//    void setResultOfRecentPlaylist(const QJsonObject &p_jsonObject);
//    void setResultOfMyPlayList(const QJsonObject &p_jsonObject);
//    void setResultOfFrienPlayList(const QJsonObject &p_jsonObject);

//    void wheelEvent(QWheelEvent *event) override;

//public:
//     QString go_page_text;//cheon210527

//private:
//    int OffsetPoint = 0;

//    int scroll_pos_currunt = 0;
//    int scroll_pos_before = 0;
//    //--------------------------------------cheon210601
//    bool flag_page_first = false;

//    int pre_lmt_cnt = 0;
//    //int OffsetPoint = 0;
//    int total_page_cnt = 0;
//    int curr_page_cnt = 0;
//    int total_db_cnt = 0, total_db_cnt_kind = 0, total_db_cnt_artist = 0, total_db_cnt_composer = 0;

//    //int scroll_pos_currunt = 0;
//    //int scroll_pos_before = 0;

//    int currunt_album_page = 0;
//    int before_album_page = 1;
//    //---------------------------------------------------

//    int pageCNT = 0;
//    int myPlaylist_cnt = 0;

//    int recent_pageCNT = 0;
//    int recent_album_count = 0;
//    int recent_total_album_count = 0;

//    int recent_album_beforeOffset = 0;
//    int recent_album_currentOffset = 0;

//    QSettings *settings;
//};

#endif // MUSICLIST_GROUP_H
