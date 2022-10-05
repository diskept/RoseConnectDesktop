#ifndef ABSTRACTADDPLAYLIST_RHV_H
#define ABSTRACTADDPLAYLIST_RHV_H

#include "common/filedownloader.h"
#include "common/linker.h"

#include "home/topmenubar.h"

#include "tidal/Dialog_ChoosePlaylist_forTidal.h"
#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"

#include "qobuz/Dialog_ChoosePlaylist_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "widget/ElidedLabel.h"
#include "widget/AbstractPlaylistTrackDetailInfo_RHV.h"
#include "widget/listwidget_rosehome_playlist.h"
#include "widget/dialogconfirm.h"

#include <QLabel>
#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QCoreApplication>
#include <QListWidgetItem>
#include <QCheckBox>


class AbstractAddPlaylist_RHV : public QWidget
{
    Q_OBJECT

public:
    enum ContentsType{
        Music = 1
        , Video
        , RoseTube
        , Tidal
        , Bugs
        , Qobuz
    };
    Q_ENUM(ContentsType);

    enum ContentsUIType{
        Creating = 1
        , Adding
        , Editing
    };
    Q_ENUM(ContentsUIType);

    explicit AbstractAddPlaylist_RHV(ContentsType p_ContentsType, ContentsUIType p_ContentsUIType, QWidget *parent = nullptr);
    ~AbstractAddPlaylist_RHV();

    void setData_fromJsonObject(const QJsonObject&);
    void setActivePage();

signals:


private slots:

    void slot_clickedDelete();
    void slot_clickedSelected(int s);
    void slot_clickedSelected_exist(int s);
    void slot_dragAndDropLoop();
    void slot_listwidgetItemPressed(QListWidgetItem *item);

    void slot_fileDownload_loadImage();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData);

    void setChange_title_text(const QString &);
    void setChange_content_text();

    void slot_btnPublic_clicked();
    void slot_btnTags_pressed();
    void slot_btnTags_clicked();

    void slot_changedSubTabUI(const QJsonObject &p_data);

    void slot_clicked_btnStream();
    void slot_clicked_btnSelect();

    void slot_selected_playlist(QString playlist_id_choose, QString playlist_title, QString playlist_description);

    void slot_create_qobuzPlaylist(const int playlist_id);
    void slot_create_tidalPlaylist(const QString uuid);
    void slot_addTracks_qobuzPlaylist(const int tracks_count);
    void slot_addTracks_tidalPlaylist(const bool flag);

    void slot_create_rosePlaylist(const QJsonObject&);
    void slot_addTracks_rosePlaylist(const QJsonObject&);

    void slot_clicked_btnConfirm();
    void slot_clicked_btnCancel();

private:
    bool playList_selectDialogConfirm();
    bool playList_delDialogConfirm();
    void setUIControl_Creating();
    void setUIControl_Adding();
    void setUIControl_Editing();

    void setImage(QString imagePath);
    void paint_imageBig(QPixmap &pixmap);
    void setDataTrackInfo_RoseListEdit_Item(const QJsonObject &trackInfo, const int &p_indedx);
    void setDataTrackInfo_RoseListEdit_Item_exist(const QJsonObject &trackInfo, const int &p_indedx);
    void setImage_item(QString imagePath);
    void paint_image_item(QPixmap &pixmap);

private:

    QPixmap *pixmap_albumImg_default_track;
    QLabel * label_track_thumbnail_item;
    QLabel *label_track_type;
    QLabel *label_track_hires;
    QLabel *label_track_resolution;
    QLabel *label_title;
    QLabel *label_artist;
    QLabel *label_album;
    QLabel *label_duration;
    QString playlist_type;
    QWidget *widget_info_main_item;
    QList<QWidget *> widget_list_added, widget_list_exist;
    QPixmap album_PixmapBig;
    QList<int> intList_editPositon, intlist_selectPositon;
    QList<QCheckBox *> checkbox_Selected, checkbox_Selected_Exist;
    QPushButton *btn_del;

    int drop_cnt_flag = 0;
    Linker *linker;

    PlaylistTrackDetailInfo_RHV *added_playlist[9999];
    PlaylistTrackDetailInfo_RHV *existing_playlist[9999];

    ContentsType contents_type;
    ContentsUIType contents_ui_type;

    TopMenuBar *menubar;
    QString contentStep;

    QVBoxLayout *vl_addPlaylist;

    QWidget *wg_info;
    QWidget *wg_info_title;
    QWidget *wg_info_content;
    QWidget *wg_info_tag;

    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    FileDownloader *fileDownLoader_item;
    QPixmap *pixmap_albumImg;

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

    QWidget *wg_menubar;
    QPushButton *btn_withStream;
    QPushButton *btn_selectList;

    QLabel *lb_listCheck_icon;
    QLabel *lb_del_icon;
    QLabel *lb_listCheck;
    QLabel *lb_selectList_icon;
    QLabel *lb_selectList, *lb_del;

    QStackedWidget *stackedwidget;
    QWidget *wg_Added;
    QWidget *wg_Existing;

    QVBoxLayout *vl_Added;
    QVBoxLayout *vl_Existing;

    ListWidget_Rosehome_Playlist *listWidget_add, *listWidget_exist;

    QWidget *wg_button;
    QPushButton *btn_confirm;
    QPushButton *btn_cancel;

    QJsonObject playlist_info;
    QJsonArray added_array;
    QJsonArray existing_array;


    QString str_added = "";
    QString str_exsting = "";

    QString selected_playlist_id = "";
    QString selected_playlist_name = "";
    QString selected_playlist_description = "";
    QString selected_track_ids = "";

    QString shared_type = "";

    int playlist_id = 0;
    int pageCNT = 0;
    int track_count = 0;
    int create_count = 0;

    QVariant vtrack_id;
    QVariant valbum_id;
    QVariant vplaylist_id;

    bool flag_all_enable = false;
    bool flag_freind_enable = false;
    bool flag_private_enable = false;

    bool flag_stream_enable = false;

    bool flag_addTracks_tidal = false;
    bool flag_addTracks_qobuz = false;
    bool flag_addTracks_rose = false;
};

#endif // ABSTRACTADDPLAYLIST_RHV_H
