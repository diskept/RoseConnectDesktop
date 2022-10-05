#ifndef TIDALRISINGMAIN_H
#define TIDALRISINGMAIN_H

#include "AbstractTidalSubWidget.h"
#include "tidal_struct.h"
#include "delegate/TidalTrackDelegate.h"

#include <QJsonArray>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal > 탐색 > Rising 선택한 화면
     */
    class TidalRisingMain : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalRisingMain(QWidget *parent = nullptr);
        ~TidalRisingMain();
        void setActivePage() override;

    protected slots:
        void slot_clickBtn_subTitle_side(const int) override;

        void slot_applyResult_tracks(const QList<tidal::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<tidal::AlbumItemData>&, const QJsonArray&, const bool) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;
        void slot_thumbnailDownloaded_fotListWidget_track() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    private :
        void setUIControl_track();
        void setUIControl_album();

        TidalTrackDelegate *delegate;

        QListWidget *listWidget_track;
        QHBoxLayout *hBox_album;        ///< layout of album

        // Data
        bool flagInitDraw;              ///< 최초 한번만 Drawing 하기 위함
        QList<tidal::AlbumItemData> *list_album;
        QList<tidal::TrackItemData> *list_track;

        QJsonArray jsonArr_tracks_toPlay;

    };

};

#endif // TIDALRISINGMAIN_H
