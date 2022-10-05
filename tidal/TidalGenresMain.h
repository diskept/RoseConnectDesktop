#ifndef TIDALGENRESMAIN_H
#define TIDALGENRESMAIN_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "widget/flowlayout.h"

#include "tidal/tidal_struct.h"
#include "AbstractTidalSubWidget.h"
#include "delegate/TidalTrackDelegate.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {


    /**
     * @brief Tidal > 탐색 > Genres > 장르선택 (장르 하나 선택했을 때의 메인 화면) 클래스
     */
    class TidalGenresMain : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalGenresMain(QWidget *parent = nullptr);
        ~TidalGenresMain();
        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_clickBtn_subTitle_side(const int) override;

        void slot_applyResult_playlists(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;
        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;
        void slot_thumbnailDownloaded_fotListWidget_track() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUIControl_track();
        void setUIControl_album();

//        void resizeEvent(QResizeEvent *event) override;

        QLabel *label_mainTitle;        ///< main titel for specific genre's name
        QHBoxLayout *hBox_playlist;     ///< layout of playlist
        QHBoxLayout *hBox_album;        ///< layout of album

        TidalTrackDelegate *delegate;
        QListWidget *listWidget_track;

        // Data
        QString curr_genre_path;        ///< 현재 선택된 genre path
        QString genre_api_playlist;     ///< 데이터 요청을 위한 장르 playlist path (Tidal API 요청 시 필요)
        QString genre_api_track;        ///< 데이터 요청을 위한 장르 track path (Tidal API 요청 시 필요)
        QString genre_api_album;        ///< 데이터 요청을 위한 장르 album path (Tidal API 요청 시 필요)

        QList<tidal::PlaylistItemData> *list_playlist;
        QList<tidal::AlbumItemData> *list_album;
        QList<tidal::TrackItemData> *list_track;

        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

    };

};

#endif // TIDALGENRESMAIN_H
