#ifndef ROSEHOMEUSERPICK_H
#define ROSEHOMEUSERPICK_H


#include <QWidget>
#include "AbstractRoseHomeSubWidget.h"
#include "tidal/AbstractTidalSubWidget.h"
#include <widget/customlistwidget.h>
#include <delegate/filedelegate.h>
#include "userpickUserpage.h"
#include "home/abstractmaincontent.h"

#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace roseHome {

    /**
     * @brief Tidal > Master 화면 클래스
     */
    class RoseHomeUserPick : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomeUserPick(QWidget *parent = nullptr);
        ~RoseHomeUserPick();
        void setActivePage() override;

    protected slots:

        void slot_clickedItemAlbum(const tidal::ItemAlbum::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::ItemPlaylist::ClickMode) override;


        // about OptMorePopup
       // void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        void slot_clickBtn_subTitle_side_friend();
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;;
         void slot_applyResult_playlists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool) override;
        void slot_applyResult_albums(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_thumbnailDownloaded();
        void slot_thumbnailDownloaded_All();
        void slot_friendImg_clicked();//cheon211119-01
        void slot_userAllImg_clicked();//cheon211119-01



    private:
        // 초기 UI화면 구성
        void setUIControl_playlist();
        void setUIControl_album();
        void setResultOfUserPickPlaylist(const QJsonObject &p_jsonObject);
        void setResultOfUserPickPlaylist_All(const QJsonObject &p_jsonObject);//cheon211115-01
        void setUIControl_Append_Contents(const int type, const bool flag, const QString userpickName, const int i, const int userpick_cnt, const int item_cnt, const QString userpickUrlimagfe);//cheon211115-01
        void setUIControl_Append_Contents_All(const int type, const bool flag, const QString userpickName, const int i, const int userpick_cnt, const int item_cnt, const QString userpickUrlimagfe);//cheon211115-01
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum(const int hbox_height, const int btnId, const int i, const int userpick_cnt);
        QHBoxLayout* setUIControl_hBoxLayout_forAlbum_All(const int hbox_height, const int btnId, const int i, const int userpick_cnt);
        void setUIControl_subTitle_withSideBtn(const QString subTitle, const int i, const int userpick_cnt);
        void setUIControl_subTitle_withSideBtn_All(const QString subTitle, const int i, const int userpick_cnt);
        void addUIControl_subTitle_withSideBtn_common_cnt(const QString subTitle, const QString btnText, const int btnId, QLabel *lbCnt);
        void addUIControl_subTitle_withSideBtn_common_cnt_all(const QString subTitle, const QString btnText, const int btnId, QLabel *lbCnt);
        //QHBoxLayout* widget_1111(QHBoxLayout *t1, QHBoxLayout *t2);

        void wheelEvent(QWheelEvent *event) override;

        //QString curr_api_subPath;
        //QString curr_api_mainTitle;

        userpickUserpage *Userpage;//cheon211119-01
        QPushButton *tmp_btnView_all_friend;
        QHBoxLayout *tmp_hBox_friend;
        QLabel *tmp_label_subTitle_friend_cnt;
        //QLabel *lb_userpickImg[3000];//cheon211119-01
        //QLabel *lb_userpickImg_All[3000];//cheon211119-01
        QPushButton *lb_userpickImg[3000];//cheon211119-01
        QPushButton *lb_userpickImg_All[3000];//cheon211119-01

        FileDelegate *delegate;
        CustomListWidget *listWidget;
        QGridLayout *grid_userpick;
        QLabel *lb_subTitle;
        QLabel *label_mainTitle;
        QHBoxLayout *hBox_playlist;     ///< layout of playlist
        QHBoxLayout *hBox_album;        ///< layout of album
        QList<QHBoxLayout *>hbox_UserPickPlaylist, *hbox_UserPickPlaylist1, *hbox_UserPickPlaylist2;
        QVBoxLayout *box_main_contents;
        QVBoxLayout *vbox_main_contents;
        QVBoxLayout *vbox_last_contents;
        QVBoxLayout *vbox1_main_contents;
        QVBoxLayout *vbox2_main_contents;
        QHBoxLayout *hbox1_main_contents;
        QHBoxLayout *hbox2_main_contents;



        QLabel *lb_title_type;//cheon211115-01
        QWidget *widget_title_main;//cheon211115-01
        QWidget *widget_title1;
        QWidget *widget_title2;
        QGridLayout *grid_box;

        //-------------------user---////cheon211115-01
        QPushButton *tmp_btnView_all_all;
        QHBoxLayout *tmp_hBox_all;
        QLabel *tmp_label_subTitle_all_cnt;
        QLabel *lb_subTitle_all;
        QList<QHBoxLayout *>hbox_UserPickPlaylist_All, *hbox_UserPickPlaylist1_All, *hbox_UserPickPlaylist2_All;
        QVBoxLayout *vbox1_main2_contents;
        QVBoxLayout *vbox2_main2_contents;
        QHBoxLayout *hbox1_main2_contents;
        QHBoxLayout *hbox2_main2_contents;
        QWidget *widget_title1_user;
        QWidget *widget_title2_user;
        QLabel *lb_title_user_type;
        QWidget *widget_title_main_user;
        //-----------------------------------

        //QGridLayout *box_main_contents;
        // Data
        int list_UserPickId[3000]={0,};
        int list_UserPickId_All[3000]={0,};
        int recursive_cnt_friend = 0, recursive_cnt_all = 0;

        int page = 0, page2= 0;
        bool flag_userpickPlaylist[2] = {false, false};
        bool flag_userpickPlaylistAll[2] = {false, false};
        bool flagInitDraw;
        bool flagLoginCheck = false;
        bool flagLogoutCheck = false;
        QList<roseHome::PlaylistItemData> *list_playlist, *list_UserPickPlaylist[3000];
        QList<roseHome::PlaylistItemData>  *list_UserPickPlaylist_All[3000];
        QList<roseHome::AlbumItemData> *list_album;

        QLabel *tmp_label_subTitle_Album;
        QLabel *tmp_label_subTitle_Playlist;
        QLabel *tmp_label_subTitle_PlaylistAll;

        bool userPick_wheel_flag = false;
        int totalCount_friend = 0, totalCount_all = 0;
    };

}


#endif // ROSEHOMEUSERPICK_H
