#ifndef DIALOG_CHOOSEPLAYLIST_FORQOBUZ_H
#define DIALOG_CHOOSEPLAYLIST_FORQOBUZ_H

#include "qobuz/qobuz_struct.h"

#include "login/dialog_newplaylist.h"
#include "login/dialog_newplaylist_onRose.h"

#include "delegate/DlgChoosePlaylistDelegate.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz 전용 - 플레이리스트에 음원을 담을 때, 플레이리스트 목록 선택을 위한 다이얼로그 클래스
     */
    class Dialog_ChoosePlaylist_forQobuz : public QDialog
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


        explicit Dialog_ChoosePlaylist_forQobuz(QWidget *parent = nullptr);
//        void set_list_playlist(const QList<qobuz::PlaylistItemData> list_playlist);

        void set_contentData(QVariant content_id, ItemType_forAddCollection content_type);

        void showEvent(QShowEvent *) override;


    signals:
        void signal_choosePlaylist_forTrack(QString playlist_id_choose, int track_id);
        void signal_choosePlaylist_forAlbum(QString playlist_id_choose, int album_id);
        void signal_choosePlaylist_forPlaylist(QString playlist_id_choose, QString playlist_uuid);
        void signal_choosePlaylist_forQOBUZ(QString playlist_id_choose, QString playlist_title, QString playlist_description);


    private slots:
        void slot_clickedItem_inList(const int index);
        void slot_thumbnailDownloaded_fotListWidget_playlist();

        void slot_applyResult_myCreatedPlaylists_forAddCollection(const QList<qobuz::PlaylistItemData>&, const QJsonArray&, const bool, const QJsonObject&);
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
        QList<qobuz::PlaylistItemData> *list_playlist;

    };

}

#endif // DIALOG_CHOOSEPLAYLIST_FORQOBUZ_H
