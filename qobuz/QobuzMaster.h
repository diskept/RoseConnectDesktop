#ifndef QOBUZMASTER_H
#define QOBUZMASTER_H

#include <QWidget>
#include "qobuz/AbstractQobuzSubWidget.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz > Master 화면 클래스
     */
    class QobuzMaster : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzMaster(QWidget *parent = nullptr);
        ~QobuzMaster();
        void setActivePage() override;

    protected slots:
        void slot_clickBtn_subTitle_side(const int) override;

        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_applyResult_playlists(const QList<qobuz::PlaylistItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool) override;

    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUIControl_album();


        QLabel *label_mainTitle;
        QHBoxLayout *hBox_playlist;     ///< layout of playlist
        QHBoxLayout *hBox_album;        ///< layout of album

        // Data
        bool flagInitDraw;
        QList<qobuz::PlaylistItemData> *list_playlist;
        QList<qobuz::AlbumItemData> *list_album;


    };

}

#endif // QOBUZMASTER_H
