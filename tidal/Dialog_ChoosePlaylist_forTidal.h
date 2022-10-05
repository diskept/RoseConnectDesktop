#ifndef DIALOG_CHOOSEPLAYLIST_FORTIDAL_H
#define DIALOG_CHOOSEPLAYLIST_FORTIDAL_H

#include "tidal/tidal_struct.h"

#include "login/dialog_newplaylist.h"
#include "login/dialog_newplaylist_onRose.h"

#include "delegate/DlgChoosePlaylistDelegate.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>

#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal 전용 - 플레이리스트에 음원을 담을 때, 플레이리스트 목록 선택을 위한 다이얼로그 클래스
     */
    class Dialog_ChoosePlaylist_forTidal : public QDialog
    {
        Q_OBJECT
    public:

        enum class ItemType_forAddCollection {
            None
            , Track
            , Album
            , Playlist
        };
        Q_ENUM(ItemType_forAddCollection);


        explicit Dialog_ChoosePlaylist_forTidal(QWidget *parent = nullptr);
//        void set_list_playlist(const QList<tidal::PlaylistItemData> list_playlist);

        void set_contentData(QVariant content_id, ItemType_forAddCollection content_type);

        void showEvent(QShowEvent *) override;


    signals:
        void signal_choosePlaylist_forTrack(QString playlist_uuid_choose, int track_id);
        void signal_choosePlaylist_forAlbum(QString playlist_uuid_choose, int album_id);
        void signal_choosePlaylist_forPlaylist(QString playlist_uuid_choose, QString playlist_uuid);
        void signal_choosePlaylist_forTIDAL(QString playlist_id_choose, QString playlist_title, QString palylist_description);


    private slots:
        void slot_clickedItem_inList(const int index);
        void slot_thumbnailDownloaded_fotListWidget_playlist();

        void slot_applyResult_myCreatedPlaylists_forAddCollection(const QList<tidal::PlaylistItemData>&, const QJsonArray&, const bool, const QJsonObject&);
        void slot_completeItems();

    private:
        const int DLG_WIDTH = 600;
        const int DLG_HEIGHT = 500;

        QVariant m_content_id;
        ItemType_forAddCollection m_content_type;

        // 초기 UI화면 구성
        void setInit();
        void setUIControl();
        void setUIControl_topTitle();
        void setUIControl_body();

        void add_listWidgetItem(int start_index);

        // UI 구성
        QVBoxLayout *vBox_total;      ///< 화면 UI구성 전체 마지막 주관하는 Layout (다이얼로그의 Top, Body 등)

        DlgChoosePlaylistDelegate *delegate;
        QListWidget *listWidget_playlist;
        QList<tidal::PlaylistItemData> *list_playlist;

    };

}

#endif // DIALOG_CHOOSEPLAYLIST_FORTIDAL_H
