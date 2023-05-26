#ifndef MUSICLIST_ONE_H
#define MUSICLIST_ONE_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include <QSettings>
#include <QListWidget>
#include <QCoreApplication>


namespace music {

    /**
     * @brief 음악-분류 의 전체 곡 트랙 목록 @n
     *      DB 조회로 목록 구성 후 트랙 Favorite Get,Set API 를 통해 하트 구성
     */
    class MusicList_One : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit MusicList_One(QWidget *parent = nullptr);
        ~MusicList_One();

        void setDataFromDB();
        void setDataFromDB(const QString &p_arg);
        void setDataFromDB_Alpa(const QString &p_arg);

    public:
        QString go_page_text;//cheon210527
        QDialog *gdialog_wait;

    protected slots:
        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_thumbnailDownload_tracks();

        void slot_listwidgetItemPressed(QListWidgetItem *item);

        void slot_clicked_btnPlaylistAdd();
        void slot_clicked_btnFavorite();
        void slot_clicked_btnMenu();

        void slot_applyResult_checkRating_track(const QJsonObject&);
        void slot_applyResult_addRating_track(const QJsonObject&);
        void slot_applyResult_getRating_track(const QJsonArray&);

        void slot_add_rosePlaylist_withMusic(const int&, const QJsonObject&);

    private:
        void setUIControl();
        void getTrackDataFromDB();

        int getAlbumDataFromDBforFullsongTotalCount();
        inline void setDataTrackInfo_Music_Item(const QJsonObject &trackInfo, const int &idx);

    private:
        Linker *linker;
        QSettings *settings;

        // UI
        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QVBoxLayout *vbox_trackList;
        QListWidget *listWidget_tracks;

        QLabel *lb_trackImg[9999999];
        QPushButton *btn_list_fav[9999999];

        QJsonArray jsonArr_tracks_toPlay;

        QString strArg = "";

        int totalCnt = 0;
        int offsetCnt = 0;
        int scroll_maxium = 0;

        int track_star_count[9999999];

        int fav_start_idx = 0;
        int track_idx_fav = 0;

        bool flag_track_fav = false;

        bool flag_draw = false;
        bool flag_check_track = false;
    };
};
#endif // MUSICLIST_ONE_H
