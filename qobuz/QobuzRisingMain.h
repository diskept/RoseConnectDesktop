#ifndef QOBUZRISINGMAIN_H
#define QOBUZRISINGMAIN_H

#include "AbstractQobuzSubWidget.h"
#include "qobuz_struct.h"
#include "delegate/QobuzTrackDelegate.h"

#include <QJsonArray>


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz > 탐색 > Rising 선택한 화면
     */
    class QobuzRisingMain : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzRisingMain(QWidget *parent = nullptr);
        ~QobuzRisingMain();
        void setActivePage() override;

    protected slots:
        void slot_clickBtn_subTitle_side(const int) override;

        void slot_applyResult_tracks(const QList<qobuz::TrackItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemTrack_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;
        void slot_thumbnailDownloaded_fotListWidget_track() override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    private :
        void setUIControl_track();
        void setUIControl_album();

        QobuzTrackDelegate *delegate;

        QListWidget *listWidget_track;
        QHBoxLayout *hBox_album;        ///< layout of album

        // Data
        bool flagInitDraw;              ///< 최초 한번만 Drawing 하기 위함
        QList<qobuz::AlbumItemData> *list_album;
        QList<qobuz::TrackItemData> *list_track;

        QJsonArray jsonArr_tracks_toPlay;

    };

};

#endif // QOBUZRISINGMAIN_H
