#ifndef USERPICKUSERPAGE_H
#define USERPICKUSERPAGE_H


#include <QWidget>
#include "AbstractRoseHomeSubWidget.h"
#include "tidal/AbstractTidalSubWidget.h"
#include <widget/customlistwidget.h>
#include <delegate/filedelegate.h>
#include "common/networkhttp.h"
#include "setting/setting_struct.h"
//#include "userpickUserpage.h"

#include "roseHome/ItemPlaylist_rosehome.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace roseHome {

    /**
     * @brief Tidal > Master 화면 클래스
     */
    class userpickUserpage : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit userpickUserpage(QWidget *parent = nullptr);
        ~userpickUserpage();
        void setJsonObject_forData(const QJsonObject &jsonObj)override;
        void setActivePage() override;
        //void setDataMyPage(int memberIdIndex);

    protected slots:

        //void slot_clickedItemAlbum(const tidal::ItemAlbum::ClickMode clickMode) override;
        void slot_clickedItemPlaylist(const tidal::ItemPlaylist::ClickMode) override;


        // about OptMorePopup
       // void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    private slots:
        //void slot_clickBtn_subTitle_side_friend();
        void slot_responseHttp_friendInfo(const int &p_id, const QJsonObject &p_jsonObject);
        //void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;;
        // void slot_applyResult_playlists(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool) override;
        //void slot_applyResult_albums(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool) override;
        //void slot_thumbnailDownloaded();
        //void slot_thumbnailDownloaded_All();
        //void slot_friendImg_clicked();//cheon211119-01
        void slot_loadImagePhotos();
        void slot_loadImageMainProfile();


    private:


        //void setDataMyPage(int m);
        void setResponseProfile(QJsonObject p_jsonObject);
        void setUIControl_FriendProfile();
        void setResultOfMemberPlaylist(const QJsonObject &p_jsonObject);
        void setUIControl_Append_MemberContents(const int type, const bool flag, const QString userpickName, const int item_cnt);
        void setUIControl_Member_subTitle_withSideBtn(const QString subTitle, const int i, const int item_cnt);
        FlowLayout *setUIControl_hBoxLayout_forAlbum_member(const int hbox_height, const int btnId, const int i, const int item_cnt);

        FlowLayout *FlowLayout_userPick;
        QString curr_api_subPath;
        QString curr_api_mainTitle;
        QString api_subPath;
        QString type;

        int member_id;
        int type_id;

        QVBoxLayout *vl_userPage;
        int memberIndex = -1;
        QLabel *lb_profile;
        QLabel *lb_profile_grade;
        QLabel *lb_nickName;
        QLabel *lb_comment;
        QHBoxLayout *hl_photo;

        QString tmp_password;
        QLabel *lb_profileEdit;
        QLabel *lb_email;
        QLineEdit *le_nickName;
        QLineEdit *le_comment;
        QLineEdit *le_pw;
        QLabel *lb_title_regPhoto;
        QWidget *widget_flow;
        FlowLayout *flowlayout_photos;
        setting::ProfileInfo data_profileInfo;  ///< 데이터 struct
        QVBoxLayout *vbox_member_contents;

        QLabel *lb_subTitle;
        bool flagInitDraw;
        int page = 0;
        QHBoxLayout *hbox_MemberPlaylist;
        QList<roseHome::PlaylistItemData> *list_MemberPlaylist;
        QVBoxLayout *vbox_last_contents;

        roseHome::ItemPlaylist_rosehome *user_playlist[99999];

    };

}


#endif // USERPICKUSERPAGE_H
