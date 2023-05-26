#ifndef ABSTRACTADDPLAYLIST_RHV_H
#define ABSTRACTADDPLAYLIST_RHV_H

#include "common/linker.h"
#include "common/filedownloader.h"

#include "home/topmenubar.h"

#include "widget/listwidget_rosehome_playlist.h"

#include "roseHome/rosehome_struct.h"

#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>


class AbstractAddPlaylist_RHV : public QWidget
{
    Q_OBJECT

public:
    enum ContentsType{
        RosePlaylist = 1
        , Music
        , Video
        , RoseTube
        , Tidal
        , Bugs
        , Qobuz
        , Apple_Music
    };
    Q_ENUM(ContentsType);

    enum ContentsUIType{
        Creating = 1
        , Adding
        , Editing
        , Delete
    };
    Q_ENUM(ContentsUIType);

    explicit AbstractAddPlaylist_RHV(ContentsType p_ContentsType, ContentsUIType p_ContentsUIType, QWidget *parent = nullptr);
    ~AbstractAddPlaylist_RHV();

    void setData_fromJsonObject(const QJsonObject&);
    void setActivePage();

private slots:
    void slot_fileDownload_loadImage();
    void slot_thumbnailDownload_adding();
    void slot_thumbnailDownload_existing();
    void slot_thumbnailDownload_editing();

    void slot_changedSubTabUI(const QJsonObject &p_data);

    void slot_dragAndDropLoop();
    void slot_listwidgetItemPressed(QListWidgetItem *item);

    void slot_applyResult_playlist(const roseHome::PlaylistItemData&);
    void slot_applyResult_playlist_tracks(const QList<roseHome::TrackItemData>&, const QJsonArray&, const bool);

    void setChange_title_text(const QString &);
    void setChange_content_text();

    void slot_btnPublic_clicked();
    void slot_btnTags_pressed();
    void slot_btnTags_clicked();

    void slot_btnEditConfirm_clicked();

    void slot_clickedDelete();
    void slot_clickedSelected(int state);
    void slot_clicked_btnSelectAll();
    void slot_clicked_btnDelete();

    void slot_clicked_btnStream();
    void slot_clicked_btnSelect();

    void slot_selected_playlist(QString playlist_id_choose, QString playlist_title, QString playlist_description);

    void slot_create_qobuzPlaylist(const int playlist_id);
    void slot_create_tidalPlaylist(const QString uuid);

    void slot_addTracks_qobuzPlaylist(const int tracks_count);
    void slot_addTracks_tidalPlaylist(const bool flag);

    void slot_create_rosePlaylist(const QJsonObject&);
    void slot_addTracks_rosePlaylist(const QJsonObject&);
    void slot_editInfo_rosePlaylist(const QJsonObject&);
    void slot_editTracks_rosePlaylist(const QJsonObject&);

    void slot_clicked_btnConfirm();
    void slot_clicked_btnCancel();

private:
    void setUIControl_Creating();
    void setUIControl_Adding();
    void setUIControl_Editing();

    void request_more_trackData();

    inline void setDataTrackInfo_RoseListEdit_Item(const QJsonObject &trackInfo, const int &p_index);
    inline void setDataTrackInfo_RoseListEdit_Item_exist(const QJsonObject &trackInfo, const int &p_index);
    inline void setDataTrackInfo_RoseListEdit_Item_edit(const QJsonObject &trackInfo, const int &p_index);

    bool tracksDelete_DialogConfirm();

    void setImage(QString imagePath);
    void paint_imageBig(QPixmap &pixmap);

private:
    Linker *linker;

    ContentsType contents_type;
    ContentsUIType contents_ui_type;

    TopMenuBar *menubar;
    QString contentStep;

    QVBoxLayout *vl_addPlaylist;
    QVBoxLayout *vl_existPlaylist;
    QVBoxLayout *vl_editPlaylist;

    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;

    QWidget *widget_info;
    QWidget *widget_info_title;
    QWidget *widget_info_content;
    QWidget *widget_info_tag;

    QLabel *lb_image;

    QLabel *lb_title;
    QLabel *lb_content;

    QLineEdit *le_title;
    QTextEdit *te_content;
    QLineEdit *le_tag;

    QLabel *lb_public_set;
    QLabel *lb_public_all;
    QLabel *lb_public_freind;
    QLabel *lb_public_private;

    QPushButton *btn_all;
    QPushButton *btn_freind;
    QPushButton *btn_private;

    QStringList tagTypeList;
    QPushButton *btn_tag[20];

    QPushButton *btn_edit_confirm;

    QWidget *widget_button;
    QPushButton *btn_confirm;
    QPushButton *btn_cancel;

    QWidget *widget_menubar;
    QPushButton *btn_withStream;
    QPushButton *btn_selectList;
    QPushButton *btn_delete;

    QLabel *lb_listCheck_icon;
    QLabel *lb_listCheck;
    QLabel *lb_selectList_icon;
    QLabel *lb_selectList;
    QLabel *lb_delete_icon;
    QLabel *lb_delete;

    QStackedWidget *stackedwidget;
    QWidget *widget_Adding;
    QWidget *widget_Existing;
    QWidget *widget_Editing;

    QVBoxLayout *vl_Adding;
    QVBoxLayout *vl_Existing;
    QVBoxLayout *vl_Editing;

    ListWidget_Rosehome_Playlist *listWidget_adding;
    QListWidget *listWidget_existing;
    ListWidget_Rosehome_Playlist *listWidget_editing;

    QCheckBox *checkbox_selectAll;
    QCheckBox *checkbox_Selected[99999];

    QLabel *lb_trackImg_adding[99999];
    QLabel *lb_trackImg_existing[99999];
    QLabel *lb_trackImg_editing[99999];

    QJsonObject playlist_info;
    QJsonArray added_array;
    QJsonArray existing_array;
    QJsonArray editing_array;

    QJsonObject snd_playlist_info;
    QJsonArray snd_track_array;

    QString str_added = "";
    QString str_exsting = "";
    QString str_editing = "";

    QString selected_playlist_id = "";
    QString selected_playlist_name = "";
    QString selected_playlist_description = "";
    QString selected_track_ids = "";

    QString shared_type = "";
    QString playlist_type = "";

    int drop_cnt_flag = 0;

    int playlist_id = 0;
    int pageCNT = 0;
    int track_count = 0;
    int create_count = 0;
    int track_drawCnt = 0;

    QVariant vtrack_id;
    QVariant valbum_id;
    QVariant vplaylist_id;

    bool flag_playlist_info = false;

    bool flagReqMore_track = false;
    bool flag_lastPage_track = false;

    bool flag_all_enable = false;
    bool flag_freind_enable = false;
    bool flag_private_enable = false;

    bool flag_stream_enable = false;

    bool flag_addTracks_tidal = false;
    bool flag_addTracks_qobuz = false;
    bool flag_addTracks_rose = false;
};

#endif // ABSTRACTADDPLAYLIST_RHV_H
