#ifndef TIDALMASTER_H
#define TIDALMASTER_H

#include <QWidget>
#include "AbstractTidalSubWidget.h"


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
        void setActivePage() override;

    protected slots:
        void slot_clickBtn_subTitle_side(const int) override;

        void slot_clickedItemAlbum(const tidal::ItemAlbum::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::ItemPlaylist::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_playlists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool) override;

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUIControl_album();


        QLabel *label_mainTitle;
        QHBoxLayout *hBox_playlist;     ///< layout of playlist
        QHBoxLayout *hBox_album;        ///< layout of album

        // Data
        bool flagInitDraw;
        bool flagLoginCheck = false;
        bool flagLogoutCheck = false;
        QList<tidal::PlaylistItemData> *list_playlist;
        QList<tidal::AlbumItemData> *list_album;

        QLabel *tmp_label_subTitle_Album;
        QLabel *tmp_label_subTitle_Playlist;
    };

}

#endif // TIDALMASTER_H
