#ifndef DIALOG_PLAYLIST_ONROSE_H
#define DIALOG_PLAYLIST_ONROSE_H

#include "common/AbstractCommonSubWidget.h"

#include "roseHome/rosehome_struct.h"

#include <QDialog>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>


namespace Dialog {

    class Dialog_Playlist_onRose : public QDialog
    {
        Q_OBJECT
    public:

        enum Playlist_type{
            MUSIC,
            VIDEO,
            ROSETUBE,
            TIDAL,
            BUGS,
            QOBUZ
        };

        explicit Dialog_Playlist_onRose(Playlist_type playlistType, QJsonObject&, QWidget *parent = nullptr);
        void request_playlist_fetch();

        void wheelEvent(QWheelEvent*) override;

    signals:
        void signal_clicked_playlist(const int&, const QJsonObject&);

    private slots:
        void slot_clickedPlaylistDelete();//c02
        void slot_clicked_playlist();

        void slot_responseHttp(const int&, const QJsonObject&);
        void setResult_of_fetch(const QJsonObject&);        

    private:
        void setInit();
        void setUIControl();
        void setInit_playlist();

        void appendPlayListUI(int p_playlistNo, QString p_name, QString p_thumbPath, int p_numContents);

        void request_more_playlistData();
        bool playList_delDialogConfirm();

    private:
        const int DLG_WIDTH = 600;
        const int DLG_HEIGHT = 580;

        QPushButton *btn_playlistdel;
        QLabel *lb_playlistdel_icon;
        Playlist_type playlist_type;
        QJsonObject *jsonData_obj;
        QJsonArray *jsonData_arr;

        QVBoxLayout *dialog_layout;

        QVBoxLayout *dialog_creat;
        QVBoxLayout *dialog_playlist;
        QVBoxLayout *dialog_total;

        QWidget *widget_total;
        QWidget *widget_title;
        QWidget *widget_boby;
        QScrollArea *scrollArea;

        QLabel *label_title;
        QPushButton *btn_close;

        int pageCNT = 0;
        int playlist_count = 0;
        int total_playlist_count = 0;

        int playlist_beforeOffset = 0;
        int playlist_currentOffset = 0;

        QList<roseHome::PlaylistItemData> *list_playlist;
    };
};

#endif // DIALOG_PLAYLIST_ONROSE_H
