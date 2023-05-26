#ifndef TIDALMASTER_H
#define TIDALMASTER_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemAlbum.h"
#include "tidal/ItemPlaylist.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal > Master 화면 클래스
     */
    class TidalMaster : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalMaster(QWidget *parent = nullptr);
        ~TidalMaster();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:

        // about Item
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

        void slot_applyResult_albums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_playlists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool) override;

    private slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

    private:
        // 초기 UI화면 구성
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout);

        QString page = "";

        tidal::ItemAlbum *master_album[15];
        tidal::ItemPlaylist *master_playlist[15];

        // UI
        QLabel *label_mainTitle;        ///< main titel for specific genre's name

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        QVBoxLayout *box_main_contents;
        QWidget *widget_main_contents;

        QWidget *widget_album;
        QWidget *widget_playlist;

        QVBoxLayout *vBox_album;
        QVBoxLayout *vBox_playlist;

        QHBoxLayout *hBox_album;
        QHBoxLayout *hBox_playlist;

        QList<tidal::AlbumItemData> *list_album;
        QList<tidal::PlaylistItemData> *list_playlist;

        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        bool flag_album[2] = {false, false};
        bool flag_playlist[2] = {false, false};

        bool flag_check_track = false;
        bool flag_track_fav = false;
        bool flag_send_track = false;

        int track_id_fav = 0;
        int track_idx_fav = 0;
        int track_star_fav = 0;
    };

}

#endif // TIDALMASTER_H
