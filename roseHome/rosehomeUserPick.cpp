#include "rosehomeUserPick.h"
#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/tidal_struct.h"
#include "tidal/PushBtn_withID.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScroller>
#include "common/gscommon.h"
#include "tidal/ItemPlaylist.h"
#include "ItemAlbum_rosehome.h"
#include "common/networkhttp.h"
#include <delegate/filedelegate.h>
#include "widget/toastmsg.h"
#include "ConvertData_rosehome.h"



namespace roseHome {


const int HTTP_USERPICK_PLAYLIST_FRIEND1 = 18;
const int HTTP_USERPICK_PLAYLIST_FRIEND2 = 28;

    const int HTTP_USERPICK_PLAYLIST_FRIEND = 8;
    const int HTTP_USERPICK_PLAYLIST_ALL = 9;
    const int HTTP_USERPICK_PLAYLIST_ALL_INIT = 10;
    const int BTN_IDX_SUBTITLE_playlists_Friend = 3;
    const int BTN_IDX_SUBTITLE_playlists_All = 4;
    //const int BTN_IDX_SUBTITLE_playlists= 1;
    //const int BTN_IDX_SUBTITLE_albums = 2;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 2;

    const int HTTP_USERPAGE = 110;
    //const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 15;

    const QString TIDAL_API_PATH_ALBUM = "master/recommended/albums";
    const QString TIDAL_API_PATH_PLAYLIST = "master/recommended/playlists";

    const QString TIDAL_MAIN_TITLE = "Master";
    const QString TIDAL_SUB_TITLE___album = "FrenzPeter";
    const QString TIDAL_SUB_TITLE___playlist = "pinpin";


    /**
     * @brief "TIDAL > 탐색 > NEW 메인" 화면의 생성자
     * @param parent
     */
    RoseHomeUserPick::RoseHomeUserPick(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll, parent){//cheon211115-01

        this->flagInitDraw = true;
/*
        delegate = new FileDelegate(this);
        listWidget = new CustomListWidget();
        listWidget->setItemDelegate(delegate);
        listWidget->setCursor(Qt::PointingHandCursor);
        listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setStyleSheet("border:none;");
   */
/*//cheon211115-01
        this->vbox_main_contents = new QVBoxLayout();
        this->vbox_main_contents->setSpacing(0);
        this->vbox_main_contents->setContentsMargins(0, 0, 0, 0);
*/
        //-----------------------Friend-------------------------------
        this->vbox1_main_contents = new QVBoxLayout();
        this->vbox1_main_contents->setSpacing(20);
        this->vbox1_main_contents->setContentsMargins(0, 0, 0, 0);

        this->vbox2_main_contents = new QVBoxLayout();
        this->vbox2_main_contents->setSpacing(20);
        this->vbox2_main_contents->setContentsMargins(0, 0, 0, 0);




        widget_title1 = new QWidget();
        widget_title1->setObjectName("widget_title1");
        widget_title1->setContentsMargins(0,0,0,0);
        widget_title1->setLayout(vbox1_main_contents);
       // widget_title1->setStyleSheet("background-color:#444444;");




        widget_title2 = new QWidget();
        widget_title2->setObjectName("widget_title2");
        widget_title2->setContentsMargins(0,0,0,0);
        widget_title2->setLayout(vbox2_main_contents);
        //widget_title2->setStyleSheet("background-color:#444444;");


        this->vbox_main_contents = new QVBoxLayout();
        this->vbox_main_contents->setSpacing(0);
        this->vbox_main_contents->setContentsMargins(0, 0, 0, 0);

        tmp_hBox_friend = new QHBoxLayout();
        tmp_hBox_friend->setSpacing(0);
        tmp_hBox_friend->setContentsMargins(0,0,0,0);
        tmp_hBox_friend->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0,5,33,5);
        widget_box_subTitle->setLayout(tmp_hBox_friend);
        widget_box_subTitle->setStyleSheet("font-size:22px; color:#FFFFFF; background-color:#444444; padding:5px;border-bottom:1px solid #303030;");


        //this->box_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);
        /*
        lb_title_type = new QLabel();
        lb_title_type->setObjectName("widget_title_type");
        lb_title_type->setContentsMargins(30,0,0,0);
        lb_title_type->setText(tr("Friend Playlist (loading...)"));
        lb_title_type->setStyleSheet("font-size:18px; color:#FFFFFF; background-color:#444444; padding:5px;border-bottom:1px solid #303030;");
*/
        this->hbox1_main_contents = new QHBoxLayout();
        this->hbox1_main_contents->setSpacing(0);
        this->hbox1_main_contents->setContentsMargins(30, 0, 0, 0);
        this->hbox1_main_contents->setSpacing(30);
        this->hbox1_main_contents->addWidget(widget_title1);
        this->hbox1_main_contents->addWidget(widget_title2);

        widget_title_main = new QWidget();
        widget_title_main->setContentsMargins(0,0,0,0);
        widget_title_main->setLayout(hbox1_main_contents);

//start----------------------------User-------------------------------------------------
        this->vbox1_main2_contents = new QVBoxLayout();
        this->vbox1_main2_contents->setSpacing(20);
        this->vbox1_main2_contents->setContentsMargins(0, 0, 0, 0);

        this->vbox2_main2_contents = new QVBoxLayout();
        this->vbox2_main2_contents->setSpacing(20);
        this->vbox2_main2_contents->setContentsMargins(0, 0, 0, 0);



        widget_title1_user = new QWidget();
        widget_title1_user->setObjectName("widget_title1_user");
        widget_title1_user->setContentsMargins(0,0,0,0);
        widget_title1_user->setLayout(vbox1_main2_contents);
        //widget_title1_user->setStyleSheet("background-color:#444444;");

        widget_title2_user = new QWidget();
        widget_title2_user->setObjectName("widget_title2_user");
        widget_title2_user->setContentsMargins(0,0,0,0);
        widget_title2_user->setLayout(vbox2_main2_contents);
        //widget_title2_user->setStyleSheet("background-color:#444444;");

        tmp_hBox_all = new QHBoxLayout();
        tmp_hBox_all->setSpacing(0);
        tmp_hBox_all->setContentsMargins(0,0,0,0);
        tmp_hBox_all->setAlignment(Qt::AlignTop);


        QWidget *widget_box_subTitle2 = new QWidget();
        widget_box_subTitle2->setContentsMargins(0,5,33,5);
        widget_box_subTitle2->setLayout(tmp_hBox_all);
        widget_box_subTitle2->setStyleSheet("font-size:22px; color:#FFFFFF; background-color:#444444; padding:5px;border-bottom:1px solid #303030;");


        //lb_title_user_type = new QLabel();
        //lb_title_user_type->setObjectName("widget_title_type");
        //lb_title_user_type->setContentsMargins(30,0,0,0);
        //lb_title_user_type->setText(tr("All Public Playlist  (loading...)"));
        //lb_title_user_type->setStyleSheet("font-size:18px; color:#FFFFFF; background-color:#444444; padding:5px;border-bottom:1px solid #303030;");




        this->hbox1_main2_contents = new QHBoxLayout();
        this->hbox1_main2_contents->setSpacing(0);
        this->hbox1_main2_contents->setContentsMargins(30, 0, 0, 0);
        this->hbox1_main2_contents->setSpacing(30);
        this->hbox1_main2_contents->addWidget(widget_title1_user);
        this->hbox1_main2_contents->addWidget(widget_title2_user);

        widget_title_main_user = new QWidget();
        widget_title_main_user->setContentsMargins(0,0,0,0);
        widget_title_main_user->setLayout(hbox1_main2_contents);

 //end--------------------------------------------------------------------------

        this->vbox_last_contents = new QVBoxLayout();//cheon211115-01
        this->vbox_last_contents->setSpacing(30);//cheon211115-01
        this->vbox_last_contents->setContentsMargins(0, 0, 0, 0);//cheon211115-01

        //this->vbox_last_contents->addWidget(lb_title_type);//cheon211115-01
        this->vbox_last_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);
        this->vbox_last_contents->addWidget(widget_title_main);//cheon211115-01

        //this->vbox_last_contents->addWidget(lb_title_user_type);//cheon211115-01
        this->vbox_last_contents->addWidget(widget_box_subTitle2, 0, Qt::AlignTop);
        this->vbox_last_contents->addWidget(widget_title_main_user);//cheon211115-01

        //---------------scroll

        //this->box_contents->addWidget(widget_background);//cheon211115-01
        //this->box_contents->setContentsMargins(0, 0, 20, 0);
        this->box_contents->addLayout(vbox_last_contents);//cheon211115-01
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        this->tmp_label_subTitle_friend_cnt = new QLabel();
        this->tmp_label_subTitle_friend_cnt->setText(QString(tr("Downloading...")));//cheon211115-01
        this->tmp_label_subTitle_friend_cnt->setStyleSheet("font-size:18px; color:#aaaaaa; background-color:#444444;");

        this->addUIControl_subTitle_withSideBtn_common_cnt(tr("Friend Playlist"), tr("View All"), BTN_IDX_SUBTITLE_playlists_Friend, this->tmp_label_subTitle_friend_cnt);

        this->tmp_label_subTitle_all_cnt = new QLabel();
        this->tmp_label_subTitle_all_cnt->setText(QString(tr("Downloading...")));//cheon211115-01
        this->tmp_label_subTitle_all_cnt->setStyleSheet("font-size:18px; color:#aaaaaa; background-color:#444444;");

        this->addUIControl_subTitle_withSideBtn_common_cnt_all(tr("All Public Playlist"), tr("View All"), BTN_IDX_SUBTITLE_playlists_All, this->tmp_label_subTitle_all_cnt);


        //this->box_contents->addLayout(this->box_main_contents);
        //print_debug();
        //this->list_UserPickPlaylist = new QList<roseHome::PlaylistItemData>();
        //this->list_UserPickPlaylist_All = new QList<roseHome::PlaylistItemData>();
        //print_debug();


    }


    /**
     * @brief 소멸자.
     */
    RoseHomeUserPick::~RoseHomeUserPick(){

        //print_debug();
        this->hbox_UserPickPlaylist.clear();
        //GSCommon::clearLayout(this->hbox_UserPickPlaylist);
        //print_debug();
        //this->list_UserPickPlaylist->clear();
        //this->hbox_UserPickPlaylist.clear();
        //GSCommon::clearLayout(this->hbox_UserPickPlaylist);
        //print_debug();
        //this->list_UserPickPlaylist_All->clear();
        //print_debug();
        this->deleteLater();
    }

    /**
     * @brief 타이달 > 탐색 > "장르" 1개 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void RoseHomeUserPick::setActivePage(){//cheon211018

        // 항상 부모클래스의 함수 먼저 호출
        AbstractRoseHomeSubWidget::setActivePage();

        if(this->flagInitDraw){
            this->flagInitDraw = false;
            //this->list_UserPickPlaylist->clear();
            //this->list_UserPickPlaylist_All->clear();
            page = 0;
            QJsonObject json;
            QUrlQuery params;
            NetworkHttp *net_new_playlist = new NetworkHttp(this);
            connect(net_new_playlist, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            net_new_playlist->request(HTTP_USERPICK_PLAYLIST_FRIEND, QString("%1/member/playlist/userpick/friend?page=0&size=20&sortType=PLAYLIST_RECENT").arg(global.legacy_v1), json, false, true);

            //print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//cheon21119-02

            QJsonObject json2;
            QUrlQuery params2;
            NetworkHttp *net_new_playlist2 = new NetworkHttp(this);
            connect(net_new_playlist2, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            net_new_playlist2->request(HTTP_USERPICK_PLAYLIST_ALL_INIT, QString("%1/member/playlist/userpick/all?page=%2&size=20&sortType=PLAYLIST_RECENT").arg(global.legacy_v1).arg(page2), json2, false, true);

            //print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//cheon21119-02



        }
    }


    void RoseHomeUserPick::slot_userAllImg_clicked(){//cheon211119-01
        print_debug();
        ToastMsg::show(this, "", tr("The detailed Friend/(Public User) view is currently being prepared for service.."));

        QPushButton *lb_userpickImg = qobject_cast<QPushButton*>(sender());
        int memberIndex = lb_userpickImg->property("memberIdIndex_all").toInt();
        //if(btnId == BTN_IDX_SUBTITLE_Recommends){
            PageInfo_UserPage data_page;
            data_page.pathTitle = "";
            data_page.api_subPath = QString("%1/member/member").arg(global.legacy_v1);
            data_page.type = "UserPsge";
            data_page.type_id = HTTP_USERPAGE;
            data_page.member_id = this->list_UserPickId_All[memberIndex];

            this->movePage_userPage(data_page);
    }

    void RoseHomeUserPick::slot_friendImg_clicked(){//cheon211119-01
        print_debug();
        ToastMsg::show(this, "", tr("The detailed Friend/(Public User) view is currently being prepared for service.."));

        QPushButton *lb_userpickImg = qobject_cast<QPushButton*>(sender());
        int memberIndex = lb_userpickImg->property("memberIdIndex").toInt();
        //if(btnId == BTN_IDX_SUBTITLE_Recommends){
            PageInfo_UserPage data_page;
            data_page.pathTitle = "";
            data_page.api_subPath = QString("%1/member/member").arg(global.legacy_v1);
            data_page.type = "UserPsge";
            data_page.type_id = HTTP_USERPAGE;
            data_page.member_id = this->list_UserPickId[memberIndex];

            this->movePage_userPage(data_page);
        //}
/*
        QPushButton *lb_userpickImg = qobject_cast<QPushButton*>(sender());
        int memberIndex = lb_userpickImg->property("memberIdIndex").toInt();
qDebug() << "memberIndex = " << memberIndex;
        int memberIdIndex = this->list_UserPickId[memberIndex];
qDebug() << "memberIdIndex = " << memberIdIndex;

        //if(Userpage==nullptr){
            print_debug();
        //    this->Userpage = new userpickUserpage();
            //GSCommon::clearLayout(this->vbox_last_contents);
            //this->vbox_last_contents->addWidget(this->Userpage);
            //this->stackedWidget_content->addWidget(this->userpickUserpage);
            //userpickUserpage->setMinimumWidth(PAGE_MIN_WIDTH);
            //connect(this->userpickUserpage, SIGNAL(signal_clickedViewAll_music(QString)), this, SLOT(slot_clickedViewAll(QString)));
            //connect(this->userpickUserpage, SIGNAL(signal_clickedViewAll_rosetube(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
            //connect(this->userpickUserpage, SIGNAL(signal_changeMyInfoData()), this, SIGNAL(signal_changeMyInfo()));
         //}
        print_debug();
        //this->flagInitDraw = true;
        userPick_wheel_flag = false;
        GSCommon::clearLayout(this->vbox_last_contents);
        print_debug();

        this->Userpage = new userpickUserpage();

         this->vbox_last_contents->addWidget(this->Userpage);
         this->Userpage->setDataMyPage(memberIdIndex);
         //--------------this->stackedWidget_content->setCurrentWidget(this->Userpage);
*/

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void RoseHomeUserPick::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        userPick_wheel_flag = true;
        int offset = 0;

        if(p_id == HTTP_USERPICK_PLAYLIST_FRIEND){
            //print_debug();
            //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "roseHomeMain::slot_responseHttp-HTTP_USERPICK_PLAYLIST_FRIEND-p_jsonObject= " << strJson;
            const QString jsonKey_flagOk = "flagOk";
            const QString jsonKey_status = "status";
            const QString jsonKey_offset = "offset";
            const QString jsonKey_totalCount = "totalCount";

            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                //if(p_jsonObject.contains(jsonKey_status)){
                //}
                totalCount_friend = p_jsonObject[jsonKey_totalCount].toInt();
                offset = p_jsonObject[jsonKey_offset].toInt();
                qDebug() << "totalCount_friend = " << totalCount_friend;
                qDebug() << "offset = " << offset;

                setResultOfUserPickPlaylist(p_jsonObject);
                ToastMsg::show(this, "", "Friend Playlist  " + QString(tr("(%1 / %2)")).arg(recursive_cnt_friend).arg(totalCount_friend),2000);
                this->tmp_label_subTitle_friend_cnt->setText(QString(tr("(%1 / %2)")).arg(recursive_cnt_friend).arg(totalCount_friend));//cheon211115-01

                //ContentLoadingwaitingMsgHide();//cheon21119-02


            }

        }else if(p_id == HTTP_USERPICK_PLAYLIST_ALL){
            //print_debug();
            const QString jsonKey_flagOk = "flagOk";
            const QString jsonKey_status = "status";
            const QString jsonKey_offset = "offset";
            const QString jsonKey_totalCount = "totalCount";

            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                totalCount_all = p_jsonObject[jsonKey_totalCount].toInt();
                offset = p_jsonObject[jsonKey_offset].toInt();
                qDebug() << "totalCount = " << totalCount_all;
                qDebug() << "offset = " << offset;

                setResultOfUserPickPlaylist_All(p_jsonObject);
                //print_debug();
                ToastMsg::show(this, "", "All Public Playlist  " + QString(tr("(%1 / %2)")).arg(recursive_cnt_all).arg(totalCount_all),2000);
                this->tmp_label_subTitle_all_cnt->setText(QString(tr("(%1 / %2)")).arg(recursive_cnt_all).arg(totalCount_all));//cheon211115-01



            }
            //print_debug();
            //ContentLoadingwaitingMsgHide();//cheon21119-02

        }else if(p_id == HTTP_USERPICK_PLAYLIST_ALL_INIT){
            //print_debug();
            const QString jsonKey_flagOk = "flagOk";
            const QString jsonKey_status = "status";
            const QString jsonKey_offset = "offset";
            const QString jsonKey_totalCount = "totalCount";

            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                totalCount_all = p_jsonObject[jsonKey_totalCount].toInt();
                //qDebug() << "totalCount = " << totalCount_all;
                if(totalCount_all != 0 && recursive_cnt_all == 0){
                    this->tmp_label_subTitle_all_cnt->setText(QString(tr("(%1 / %2)")).arg(recursive_cnt_all).arg(totalCount_all) + tr("           After all Friend Playlist is shown..."));//cheon211115-01

                }else{
                    this->tmp_label_subTitle_all_cnt->setText(QString(tr("(%1 / %2)")).arg(recursive_cnt_all).arg(totalCount_all));//cheon211115-01

                }
            }

        }

        sender()->deleteLater();
    }

    void RoseHomeUserPick::addUIControl_subTitle_withSideBtn_common_cnt(const QString subTitle, const QString btnText, const int btnId, QLabel *lbCnt){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다. lb_title_type

//print_debug();
        // lbCnt->setText(QString("%1").arg(global.listCnt1));
        //lbCnt->setStyleSheet("font-size:22px; color:#FFFFFF;");

        // subTitle label
        QLabel *tmp_label_subTitle = new QLabel();
        tmp_label_subTitle->setText(subTitle);
        tmp_label_subTitle->setStyleSheet("font-size:22px; color:#FFFFFF;");
        tmp_hBox_friend->addWidget(tmp_label_subTitle,0, Qt::AlignVCenter);
        tmp_hBox_friend->setSpacing(10);
        tmp_hBox_friend->addWidget(lbCnt,1, Qt::AlignVCenter);

//print_debug();


    }

    void RoseHomeUserPick::addUIControl_subTitle_withSideBtn_common_cnt_all(const QString subTitle, const QString btnText, const int btnId, QLabel *lbCnt){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다. lb_title_type

//print_debug();
        // lbCnt->setText(QString("%1").arg(global.listCnt1));
        //lbCnt->setStyleSheet("font-size:22px; color:#FFFFFF;");

        // subTitle label
        QLabel *tmp_label_subTitle = new QLabel();
        tmp_label_subTitle->setText(subTitle);
        tmp_label_subTitle->setStyleSheet("font-size:22px; color:#FFFFFF;");
        tmp_hBox_all->addWidget(tmp_label_subTitle,0, Qt::AlignVCenter);
        tmp_hBox_all->setSpacing(10);
        tmp_hBox_all->addWidget(lbCnt,1, Qt::AlignVCenter);

//print_debug();


    }

    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void RoseHomeUserPick::slot_clickBtn_subTitle_side_friend(){
        int btnId = sender()->property("btnNo").toInt();
        print_debug();
        if(btnId == BTN_IDX_SUBTITLE_playlists_Friend){
            print_debug();
        }
        else if(btnId == BTN_IDX_SUBTITLE_playlists_All){
            print_debug();
        }


    }


    void RoseHomeUserPick::setResultOfUserPickPlaylist(const QJsonObject &p_jsonObject){

        if(ProcJsonEasy::get_flagOk(p_jsonObject) == true){

            QJsonArray jsonArruserpick = ProcJsonEasy::getJsonArray(p_jsonObject, "userPicks");

            int userpick_cnt = jsonArruserpick.count();
            QVBoxLayout *vl_total = new QVBoxLayout();
            vl_total->setContentsMargins(0,0,0,0);
            vl_total->setSpacing(0);
            //vl_total->setAlignment(Qt::AlignLeft);//cheon211119-01

            //print_debug();
            //lb_title_type->setText(QString(tr("Friend Playlist (%1)")).arg(jsonArruserpick.size()));//cheon211115-01

            if(userpick_cnt > 0){

               // listWidget->clear();
                for(int k = 0; k < userpick_cnt; k++){
                //for(int k = 0; k < 2; k++){
                    //print_debug();
                    this->list_UserPickPlaylist[recursive_cnt_friend] = new QList<roseHome::PlaylistItemData>();
                    //print_debug();
                    //qDebug()<< "recursive_cnt_friend= "<< recursive_cnt_friend;
                    this->lb_userpickImg[recursive_cnt_friend] = new QPushButton(this);
                    this->lb_userpickImg[recursive_cnt_friend]->setProperty("memberIdIndex", recursive_cnt_friend);
                    //this->lb_userpickImg[i]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
                    //this->lb_userpickImg[i]->setStyleSheet("border-color:#FFFFFF;border:1px solid #5A5A5A;");
                    this->lb_userpickImg[recursive_cnt_friend]->setCursor(Qt::PointingHandCursor);
                    this->lb_userpickImg[recursive_cnt_friend]->setFixedSize(50,50);
                    this->lb_userpickImg[recursive_cnt_friend]->setLayout(vl_total);

                    QJsonObject tmp_jsonuserpick = jsonArruserpick.at(k).toObject();
                    //print_debug();
                    //QJsonDocument doc(tmp_jsonuserpick);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "roseHomeMain::slot_responseHttp-setResultOfUserPickPlaylist-p_jsonObject= " << strJson;

                    int  playlistCount = ProcJsonEasy::ProcJsonEasy::getInt(tmp_jsonuserpick, "playlistCount");

                    QString userpickName = ProcJsonEasy::getString(tmp_jsonuserpick["member"].toObject(), "name");
                    int userpickid = ProcJsonEasy::getInt(tmp_jsonuserpick["member"].toObject(), "id");
                    QString userpickUrlimage = ProcJsonEasy::getString(tmp_jsonuserpick["member"].toObject(), "profileImage");
                    //qDebug()<<"\nRoseHomeUserPick::setResultOfUserPickPlaylist---userpickName---"<<userpickName;
                    //qDebug()<<"\nRoseHomeUserPick::setResultOfUserPickPlaylist---userpickUrlimagfe---"<<userpickUrlimagfe;
                    //print_debug();
                    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nk = " << k << ", roseHomeMain::slot_responseHttp--tmp_jsonuserpick= " << tmp_jsonuserpick;
                    QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(tmp_jsonuserpick, "playlists");
                    //print_debug();
                    //qDebug()<<"\nRoseHomeUserPick::setResultOfUserPickPlaylist---jsonArrSub.count()---"<<jsonArrSub.count();

                    if(jsonArrSub.size() > 0){
                        //this->list_UserPickPlaylist->clear();
                        //for(int i = 0; i < 2; i++){
                        for(int i = 0; i < jsonArrSub.count(); i++){
                            QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                            //print_debug();
                            //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nk = " << k << ",i = " << i << ", roseHomeMain::slot_responseHttp--tmp_json= " << tmp_json;
                            PlaylistItemData tmp_data;

                            tmp_data.id = ProcJsonEasy::getInt(tmp_json, "id");
                            tmp_data.ownerId =ProcJsonEasy::getInt(tmp_json, "ownerId");
                            tmp_data.sort = ProcJsonEasy::getInt(tmp_json, "sort");
                            tmp_data.star = ProcJsonEasy::getInt(tmp_json, "star");
                            tmp_data.thumbupCount = ProcJsonEasy::getInt(tmp_json, "thumbupCount");
                            tmp_data.trackCount = ProcJsonEasy::getInt(tmp_json, "trackCount");

                            tmp_data.clientKey = ProcJsonEasy::getString(tmp_json, "clientKey");
                            tmp_data.comment = ProcJsonEasy::getString(tmp_json, "comment");
                            tmp_data.ownerName = ProcJsonEasy::getString(tmp_json, "ownerName");
                            tmp_data.registDateTime = ProcJsonEasy::getString(tmp_json, "registDateTime");
                            tmp_data.share = ProcJsonEasy::getString(tmp_json, "share");
                            tmp_data.thumbnail = ProcJsonEasy::getString(tmp_json, "thumbnail");
                            tmp_data.title = ProcJsonEasy::getString(tmp_json, "title");
                            //qDebug()<<"\nRoseHomeUserPick::setResultOfUserPickPlaylist---tmp_data.title---"<<tmp_data.title;
                            tmp_data.type = ProcJsonEasy::getString(tmp_json, "type");
                            tmp_data.playTime = ProcJsonEasy::getString(tmp_json, "playTime");

                            tmp_data.favorite = ProcJsonEasy::getBool(tmp_json, "favorite");
                            tmp_data.favoriteTrack = ProcJsonEasy::getBool(tmp_json, "favoriteTrack");
                            tmp_data.isRose = ProcJsonEasy::getBool(tmp_json, "isRose");
                            tmp_data.thumbup = ProcJsonEasy::getBool(tmp_json, "thumbup");

                            QJsonObject data_json = ProcJsonEasy::getJsonObject(tmp_json, "data");
                            tmp_data.trackId = ProcJsonEasy::getInt(data_json, "trackId");
                            tmp_data.macAddress = ProcJsonEasy::getString(data_json, "macAddress");

                            QJsonArray tags_jsonArr = ProcJsonEasy::getJsonArray(tmp_json, "tags");

                            for(int i = 0; i < tags_jsonArr.count(); i++){
                                tmp_data.tags.append(tags_jsonArr.at(i).toString());
                            }

                            tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObject, "totalCount");

                            this->list_UserPickPlaylist[recursive_cnt_friend]->append(tmp_data);
                        }
                    }
                    //qDebug() << "recursive_cnt_friend = " << recursive_cnt_friend;
                    //qDebug() << "list_UserPickId-userpickid = " << userpickid;
                    //qDebug() << "userpick_cnt = " << userpick_cnt;
                    this->list_UserPickId[recursive_cnt_friend]= userpickid;
                    this->setUIControl_Append_Contents(HTTP_USERPICK_PLAYLIST_FRIEND, true, userpickName , recursive_cnt_friend, userpick_cnt, playlistCount, userpickUrlimage);//cheon211115-01
                    recursive_cnt_friend++;
                }

            }

        }
        else{
            this->setUIControl_Append_Contents(HTTP_USERPICK_PLAYLIST_FRIEND, false, "unname..." , -1, -1, -1, "");//cheon211115-01
        }
    }

    void RoseHomeUserPick::setResultOfUserPickPlaylist_All(const QJsonObject &p_jsonObject){

        if(ProcJsonEasy::get_flagOk(p_jsonObject) == true){

            QJsonArray jsonArruserpick = ProcJsonEasy::getJsonArray(p_jsonObject, "userPicks");

            int userpick_cnt = jsonArruserpick.count();
            QVBoxLayout *vl_total = new QVBoxLayout();
            vl_total->setContentsMargins(0,0,0,0);
            vl_total->setSpacing(0);

            for(int k = 0 ; k < userpick_cnt; k++){//cheon211119-01
            //for(int k = 0 ; k < 2; k++){

                this->lb_userpickImg_All[recursive_cnt_all] = new QPushButton(this);
                this->lb_userpickImg_All[recursive_cnt_all]->setProperty("memberIdIndex_all", recursive_cnt_all);

                //this->lb_userpickImg[i]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
                //this->lb_userpickImg[i]->setStyleSheet("border-color:#FFFFFF;border:1px solid #5A5A5A;");
                this->lb_userpickImg_All[recursive_cnt_all]->setCursor(Qt::PointingHandCursor);
                this->lb_userpickImg_All[recursive_cnt_all]->setFixedSize(50,50);
                this->lb_userpickImg_All[recursive_cnt_all]->setLayout(vl_total);
            //}
            //lb_title_user_type->setText(QString(tr("All Public Playlist  (%1)")).arg(jsonArruserpick.size()));//cheon211115-01

            //if(jsonArruserpick.size() > 0){
                this->list_UserPickPlaylist_All[recursive_cnt_all] = new QList<roseHome::PlaylistItemData>();

                //listWidget->clear();
               // for(int k = 0; k < jsonArruserpick.count(); k++){
                    QJsonObject tmp_jsonuserpick = jsonArruserpick.at(k).toObject();
                    int  playlistCount = ProcJsonEasy::ProcJsonEasy::getInt(tmp_jsonuserpick, "playlistCount");

                    QString userpickName = ProcJsonEasy::getString(tmp_jsonuserpick["member"].toObject(), "name");
                    QString userpickUrlimage = ProcJsonEasy::getString(tmp_jsonuserpick["member"].toObject(), "profileImage");
                     int userpickid = ProcJsonEasy::getInt(tmp_jsonuserpick["member"].toObject(), "id");
                    //qDebug()<<"\nRoseHomeUserPick::setResultOfUserPickPlaylist_All---userpickName---"<<userpickName;
                    //qDebug()<<"\nRoseHomeUserPick::setResultOfUserPickPlaylist_All---userpickUrlimagfe---"<<userpickUrlimagfe;
                    //print_debug();
                    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nk = " << k << ", roseHomeMain::slot_responseHttp--tmp_jsonuserpick= " << tmp_jsonuserpick;
                    QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(tmp_jsonuserpick, "playlists");
                    //print_debug();
                    //qDebug()<<"\nRoseHomeUserPick::setResultOfUserPickPlaylist_All---jsonArrSub.count()---"<<jsonArrSub.count();

                    if(jsonArrSub.size() > 0){
                        //this->list_UserPickPlaylist_All->clear();
                        for(int i = 0; i < jsonArrSub.count(); i++){
                            QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                            //print_debug();
                            //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nk = " << k << ",i = " << i << ", roseHomeMain::slot_responseHttp--tmp_json= " << tmp_json;
                            PlaylistItemData tmp_data;

                            tmp_data.id = ProcJsonEasy::getInt(tmp_json, "id");
                            tmp_data.ownerId =ProcJsonEasy::getInt(tmp_json, "ownerId");
                            tmp_data.sort = ProcJsonEasy::getInt(tmp_json, "sort");
                            tmp_data.star = ProcJsonEasy::getInt(tmp_json, "star");
                            tmp_data.thumbupCount = ProcJsonEasy::getInt(tmp_json, "thumbupCount");
                            tmp_data.trackCount = ProcJsonEasy::getInt(tmp_json, "trackCount");

                            tmp_data.clientKey = ProcJsonEasy::getString(tmp_json, "clientKey");
                            tmp_data.comment = ProcJsonEasy::getString(tmp_json, "comment");
                            tmp_data.ownerName = ProcJsonEasy::getString(tmp_json, "ownerName");
                            tmp_data.registDateTime = ProcJsonEasy::getString(tmp_json, "registDateTime");
                            tmp_data.share = ProcJsonEasy::getString(tmp_json, "share");
                            tmp_data.thumbnail = ProcJsonEasy::getString(tmp_json, "thumbnail");
                            tmp_data.title = ProcJsonEasy::getString(tmp_json, "title");
                            //qDebug()<<"\nRoseHomeUserPick::setResultOfUserPickPlaylist_All---tmp_data.title---"<<tmp_data.title;
                            tmp_data.type = ProcJsonEasy::getString(tmp_json, "type");
                            tmp_data.playTime = ProcJsonEasy::getString(tmp_json, "playTime");

                            tmp_data.favorite = ProcJsonEasy::getBool(tmp_json, "favorite");
                            tmp_data.favoriteTrack = ProcJsonEasy::getBool(tmp_json, "favoriteTrack");
                            tmp_data.isRose = ProcJsonEasy::getBool(tmp_json, "isRose");
                            tmp_data.thumbup = ProcJsonEasy::getBool(tmp_json, "thumbup");

                            QJsonObject data_json = ProcJsonEasy::getJsonObject(tmp_json, "data");
                            tmp_data.trackId = ProcJsonEasy::getInt(data_json, "trackId");
                            tmp_data.macAddress = ProcJsonEasy::getString(data_json, "macAddress");

                            QJsonArray tags_jsonArr = ProcJsonEasy::getJsonArray(tmp_json, "tags");

                            for(int i = 0; i < tags_jsonArr.count(); i++){
                                tmp_data.tags.append(tags_jsonArr.at(i).toString());
                            }

                            tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObject, "totalCount");

                            this->list_UserPickPlaylist_All[recursive_cnt_all]->append(tmp_data);
                        }
                    }//if
                    //print_debug();
                    this->list_UserPickId_All[recursive_cnt_all]= userpickid;
                    this->setUIControl_Append_Contents_All(HTTP_USERPICK_PLAYLIST_ALL, true, userpickName , recursive_cnt_all, userpick_cnt,playlistCount, userpickUrlimage);//cheon211115-01
                    recursive_cnt_all++;
               // }//for
            }//for
//print_debug();
        }//if
        else{
            this->setUIControl_Append_Contents_All(HTTP_USERPICK_PLAYLIST_ALL, false, "unname..." , -1, -1, -1, "");//cheon211115-01
        }
    }


    //json.insert("isFile", true);
    //json.insert("album_art", tmp_albumImg);

    void RoseHomeUserPick::setUIControl_Append_Contents(const int type, const bool flag, const QString userpickName, const int i, const int userpick_cnt, const int item_cnt, const QString userpickUrlimagfe){//cheon211115-01
        //int p_index = i;
        //print_debug()
        //qDebug() << "i = " << i;
        if(userpick_cnt <= 0) return;
        FileDownloader *fileDownloader = new FileDownloader(this);
        fileDownloader->setProperty("index", i);
        fileDownloader->setImageURL( QString("%1").arg(userpickUrlimagfe));
        connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));


        if(type == HTTP_USERPICK_PLAYLIST_FRIEND){
            this->flag_userpickPlaylist[0] = true;
            this->flag_userpickPlaylist[1] = flag;
        }


        if(this->flag_userpickPlaylist[0] == true ){



            if(this->flag_userpickPlaylist[1] == true){
                //this->flagInitDraw = false;
                //print_debug();

                this->setUIControl_subTitle_withSideBtn(userpickName, i, item_cnt);
                this->hbox_UserPickPlaylist.append(this->setUIControl_hBoxLayout_forAlbum(311, HTTP_USERPICK_PLAYLIST_FRIEND, i, item_cnt));
                this->createList_itemPlaylsit_applyingWithData(*this->list_UserPickPlaylist[i], tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hbox_UserPickPlaylist.at(i), 0, 1000+i);

                if(totalCount_friend % 2 != 0 && totalCount_friend == (i+1)){
                    //print_debug();
                    this->setUIControl_subTitle_withSideBtn("", i+1, item_cnt);
                    //print_debug();
                    this->hbox_UserPickPlaylist.append(this->setUIControl_hBoxLayout_forAlbum(311, HTTP_USERPICK_PLAYLIST_FRIEND, i+1, item_cnt));
                    //print_debug();
                    //this->createList_itemPlaylsit_applyingWithData(*this->list_UserPickPlaylist[i], tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hbox_UserPickPlaylist.at(i), 0, 1000+totalCount_friend);
                    //print_debug();

                }
                this->flag_userpickPlaylist[0] = false;
            }

        }
    }

    void RoseHomeUserPick::setUIControl_Append_Contents_All(const int type, const bool flag, const QString userpickName, const int i, const int userpick_cnt, const int item_cnt, const QString userpickUrlimagfe){//cheon211115-01

        if(userpick_cnt <= 0) return;
        FileDownloader *fileDownloader = new FileDownloader(this);
        fileDownloader->setProperty("index", i);
        fileDownloader->setImageURL( QString("%1").arg(userpickUrlimagfe));
        connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded_All()));


        if(type == HTTP_USERPICK_PLAYLIST_ALL){
            this->flag_userpickPlaylistAll[0] = true;
            this->flag_userpickPlaylistAll[1] = flag;
        }


        //if(this->flag_recommends[0] == true && this->flag_recentAlbum[0] == true && this->flag_recentPlaylist[0] == true && this->flag_recentRosetube[0] == true &&
                //this->flag_recentTrack[0] == true && this->flag_myPlaylist[0] == true && this->flag_newPlaylist[0] == true && this->flag_popPlaylist[0] == true){
        if(this->flag_userpickPlaylistAll[0] == true ){



            if(this->flag_userpickPlaylistAll[1] == true){

                if(this->flag_userpickPlaylistAll[1] == true){
                    //this->flagInitDraw = false;
                    //print_debug();

                    this->setUIControl_subTitle_withSideBtn_All(userpickName, i, item_cnt);
                    //print_debug();
                    this->hbox_UserPickPlaylist_All.append(this->setUIControl_hBoxLayout_forAlbum_All(311, HTTP_USERPICK_PLAYLIST_ALL, i, item_cnt));
                    //print_debug();
                    this->createList_itemPlaylsit_applyingWithData(*this->list_UserPickPlaylist_All[i], tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hbox_UserPickPlaylist_All.at(i), 0, 2000+i);
                    //print_debug();

                    if(totalCount_all % 2 != 0 && totalCount_all == (i+1)){
                        //print_debug();
                        this->setUIControl_subTitle_withSideBtn_All("", i+1, item_cnt);
                        this->hbox_UserPickPlaylist_All.append(this->setUIControl_hBoxLayout_forAlbum_All(311, HTTP_USERPICK_PLAYLIST_ALL, i+1, item_cnt));
                        //this->createList_itemPlaylsit_applyingWithData(*this->list_UserPickPlaylist_All[i], tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hbox_UserPickPlaylist_All.at(i), 0, 2000+totalCount_all);

                    }
                    this->flag_userpickPlaylistAll[0] = false;
                }


                this->flag_userpickPlaylistAll[0] = false;
            }

        }
    }


    void RoseHomeUserPick::setUIControl_subTitle_withSideBtn(const QString subTitle, const int i, const int item_cnt){//cheon211115-01

        //QListWidgetItem *item = listWidget->item(i);
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        //tmp_hBox->setSpacing(10);
        tmp_hBox->setContentsMargins(0,0,0,0);
        //tmp_hBox->setAlignment(Qt::AlignTop);

        this->lb_subTitle = new QLabel();
        //this->lb_subTitle->setText(subTitle);
        if(totalCount_friend != (i)){
            this->lb_subTitle->setText(QString(subTitle+"  (%1)").arg(item_cnt));

            this->lb_subTitle->setStyleSheet("font-size:18px;font-weight:bold;color:#FFFFFF;");


            // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.


            tmp_hBox->addWidget(lb_userpickImg[i]);

            tmp_hBox->addWidget(this->lb_subTitle);

            connect(lb_userpickImg[i], SIGNAL(clicked()), this, SLOT(slot_friendImg_clicked()));
        }else{
        }

        QWidget *widget_box_subTitle = new QWidget();
        //widget_box_subTitle->setContentsMargins(10,5,5,0);
        widget_box_subTitle->setFixedHeight(50);
        widget_box_subTitle->setLayout(tmp_hBox);

        if( (i+1)%2 != 0){
            //print_debug();
            this->vbox1_main_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);

        }
        else {
            //print_debug();
            this->vbox2_main_contents->addWidget(widget_box_subTitle, 1, Qt::AlignTop);
        }



    }


    void RoseHomeUserPick::setUIControl_subTitle_withSideBtn_All(const QString subTitle, const int i, const int item_cnt){//cheon211115-01

        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        //tmp_hBox->setSpacing(10);
        tmp_hBox->setContentsMargins(0,0,0,0);
        //tmp_hBox->setAlignment(Qt::AlignTop);

        //QListWidgetItem *item = listWidget->item(i);
        this->lb_subTitle_all = new QLabel();
        //this->lb_subTitle->setText(subTitle);
        if(totalCount_all != (i)){
            this->lb_subTitle_all->setText(QString(subTitle+"  (%1)").arg(item_cnt));
            this->lb_subTitle_all->setStyleSheet("font-size:18px;font-weight:bold;color:#FFFFFF;");

            // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.


            tmp_hBox->addWidget(lb_userpickImg_All[i]);
            tmp_hBox->addWidget(this->lb_subTitle_all);
            connect(lb_userpickImg_All[i], SIGNAL(clicked()), this, SLOT(slot_userAllImg_clicked()));
        }else{
        }
        QWidget *widget_box_subTitle = new QWidget();
        // widget_box_subTitle->setContentsMargins(10,5,5,0);
        widget_box_subTitle->setFixedHeight(50);
        widget_box_subTitle->setLayout(tmp_hBox);

        if( (i+1)%2 != 0){
            //print_debug();
            this->vbox1_main2_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);
        }
        else {
            //print_debug();
            this->vbox2_main2_contents->addWidget(widget_box_subTitle, 1, Qt::AlignTop);
        }
    }


    QHBoxLayout* RoseHomeUserPick::setUIControl_hBoxLayout_forAlbum(const int hbox_height, const int btnId, const int i, const int item_cnt){//cheon211115-01
        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0,0,0,0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0,0,0,0);



        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(hbox_height);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END
        QHBoxLayout *hBox_background = new QHBoxLayout();
        hBox_background->addWidget(tmp_scrollArea);

        QWidget *line_bottom_1 = new QWidget();
        line_bottom_1->setFixedHeight(5);
        line_bottom_1->setStyleSheet("border-bottom: 1px solid #222222;background-color:#333333;");


        QWidget *widget_background = new QWidget();
        widget_background->setLayout(hBox_background);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");
        if( (i+1)%2 != 0){
            //print_debug();
            this->vbox1_main_contents->addWidget(widget_background, 0, Qt::AlignTop);
            this->vbox1_main_contents->addWidget(line_bottom_1, 0, Qt::AlignTop);
        }else{
            //print_debug();
            this->vbox2_main_contents->addWidget(widget_background, 1, Qt::AlignTop);
            this->vbox2_main_contents->addWidget(line_bottom_1, 1, Qt::AlignTop);
        }



        // return
        return hBox_album;
        //return hBox_background;
    }

    QHBoxLayout* RoseHomeUserPick::setUIControl_hBoxLayout_forAlbum_All(const int hbox_height, const int btnId, const int i, const int item_cnt){//cheon211115-01
        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0,0,0,0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0,0,0,0);



        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(hbox_height);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END
        QHBoxLayout *hBox_background = new QHBoxLayout();
        hBox_background->addWidget(tmp_scrollArea);

        QWidget *line_bottom_1 = new QWidget();
        line_bottom_1->setFixedHeight(5);
        line_bottom_1->setStyleSheet("border-bottom: 1px solid #222222;background-color:#333333;");


        QWidget *widget_background = new QWidget();
        widget_background->setLayout(hBox_background);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");
        if( (i+1)%2 != 0){
            //print_debug();
            this->vbox1_main2_contents->addWidget(widget_background, 0, Qt::AlignTop);
            this->vbox1_main2_contents->addWidget(line_bottom_1, 0, Qt::AlignTop);
        }else{
            //print_debug();
            this->vbox2_main2_contents->addWidget(widget_background, 1, Qt::AlignTop);
            this->vbox2_main2_contents->addWidget(line_bottom_1, 1, Qt::AlignTop);
        }



        // return
        return hBox_album;
    }



    void RoseHomeUserPick::slot_thumbnailDownloaded(){

        int userpickIMG_SIZE = 50;
        FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
        int tmp_index = fileDownloader->property("index").toInt();
        //qDebug() << "RoseHomeUserPick::slot_thumbnailDownloaded----tmp_index = " << tmp_index;
        //qDebug() << "RoseHomeUserPick::slot_thumbnailDownloaded----listWidget->count() = " << listWidget->count();
        //if(tmp_index < listWidget->count()){
            //print_debug();
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());

            if(flagLoaded){
                //print_debug();
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(userpickIMG_SIZE,userpickIMG_SIZE, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

               // print_debug();
                //----------------------------------------------

                QPixmap pixmap_painter = QPixmap(QSize(userpickIMG_SIZE,userpickIMG_SIZE));
                pixmap_painter.fill(Qt::transparent);

                QPainter painter (&pixmap_painter);
                painter.setRenderHint(QPainter::Antialiasing, true);
                QBrush brush = QBrush(tmp_pixmap);
                painter.setBrush(brush);
                painter.drawRoundedRect(0, 0, userpickIMG_SIZE, userpickIMG_SIZE, 50, 50);    // border-radius:4px
                //this->lb_albumImg->setPixmap(pixmap_painter);
                //-----------------------------------

                //lb_userpickImg[tmp_index]->setPixmap(pixmap_painter);//cheon211119-01
                QPixmap imagedisplay(pixmap_painter);//cheon211119-01
                QIcon icon(imagedisplay);//cheon211119-01
                lb_userpickImg[tmp_index]->setIconSize(QSize(100,100));//cheon211119-01
                //lb_userpickImg[tmp_index]->setGeometry(70,10,100,160);//cheon211119-01
                lb_userpickImg[tmp_index]->setIcon(icon);//cheon211119-01


            }else{
                //this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_300.png");
                int width = 100;
                int height = 100;
                QImage image = QPixmap(width, height).toImage();
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(userpickIMG_SIZE,userpickIMG_SIZE, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

                //----------------------------------------------

                QPixmap pixmap_painter = QPixmap(QSize(userpickIMG_SIZE,userpickIMG_SIZE));
                pixmap_painter.fill(Qt::transparent);

                QPainter painter (&pixmap_painter);
                painter.setRenderHint(QPainter::Antialiasing, true);
                QBrush brush = QBrush(tmp_pixmap);
                painter.setBrush(brush);
                painter.drawRoundedRect(0, 0, userpickIMG_SIZE, userpickIMG_SIZE, 50, 50);    // border-radius:4px
                //this->lb_albumImg->setPixmap(pixmap_painter);
                //-----------------------------------
                //lb_userpickImg[tmp_index]->setPixmap(pixmap_painter);//cheon211119-01

                QPixmap imagedisplay(pixmap_painter);//cheon211119-01
                QIcon icon(imagedisplay);//cheon211119-01
                lb_userpickImg[tmp_index]->setIconSize(QSize(100,100));//cheon211119-01
                //lb_userpickImg[tmp_index]->setGeometry(70,10,100,160);//cheon211119-01
                lb_userpickImg[tmp_index]->setIcon(icon);//cheon211119-01

            }
        //}


        fileDownloader->deleteLater();

    }

    void RoseHomeUserPick::slot_thumbnailDownloaded_All(){
        int userpickIMG_SIZE = 50;
        FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
        int tmp_index = fileDownloader->property("index").toInt();
        //qDebug() << "RoseHomeUserPick::slot_thumbnailDownloaded----tmp_index = " << tmp_index;
        //qDebug() << "RoseHomeUserPick::slot_thumbnailDownloaded----listWidget->count() = " << listWidget->count();
        //if(tmp_index < listWidget->count()){
            //print_debug();
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());

            if(flagLoaded){
                //print_debug();
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(userpickIMG_SIZE,userpickIMG_SIZE, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

               // print_debug();
                //----------------------------------------------

                QPixmap pixmap_painter = QPixmap(QSize(userpickIMG_SIZE,userpickIMG_SIZE));
                pixmap_painter.fill(Qt::transparent);

                QPainter painter (&pixmap_painter);
                painter.setRenderHint(QPainter::Antialiasing, true);
                QBrush brush = QBrush(tmp_pixmap);
                painter.setBrush(brush);
                painter.drawRoundedRect(0, 0, userpickIMG_SIZE, userpickIMG_SIZE, 50, 50);    // border-radius:4px
                //this->lb_albumImg->setPixmap(pixmap_painter);
                //-----------------------------------

                //lb_userpickImg_All[tmp_index]->setPixmap(pixmap_painter);//cheon211119-01
                QPixmap imagedisplay(pixmap_painter);//cheon211119-01
                QIcon icon(imagedisplay);//cheon211119-01
                lb_userpickImg_All[tmp_index]->setIconSize(QSize(100,100));//cheon211119-01
                //lb_userpickImg[tmp_index]->setGeometry(70,10,100,160);//cheon211119-01
                lb_userpickImg_All[tmp_index]->setIcon(icon);//cheon211119-01


            }else{
                //this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_300.png");
                int width = 100;
                int height = 100;
                QImage image = QPixmap(width, height).toImage();
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(userpickIMG_SIZE,userpickIMG_SIZE, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

                //----------------------------------------------

                QPixmap pixmap_painter = QPixmap(QSize(userpickIMG_SIZE,userpickIMG_SIZE));
                pixmap_painter.fill(Qt::transparent);

                QPainter painter (&pixmap_painter);
                painter.setRenderHint(QPainter::Antialiasing, true);
                QBrush brush = QBrush(tmp_pixmap);
                painter.setBrush(brush);
                painter.drawRoundedRect(0, 0, userpickIMG_SIZE, userpickIMG_SIZE, 50, 50);    // border-radius:4px
                //this->lb_albumImg->setPixmap(pixmap_painter);
                //-----------------------------------
                //lb_userpickImg_All[tmp_index]->setPixmap(pixmap_painter);//cheon211119-01
                QPixmap imagedisplay(pixmap_painter);//cheon211119-01
                QIcon icon(imagedisplay);//cheon211119-01
                lb_userpickImg_All[tmp_index]->setIconSize(QSize(100,100));//cheon211119-01
                //lb_userpickImg[tmp_index]->setGeometry(70,10,100,160);//cheon211119-01
                lb_userpickImg_All[tmp_index]->setIcon(icon);//cheon211119-01
            }
        //}


        fileDownloader->deleteLater();

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief [slot] 앨범 데이터를 받아서 처리함
     * @param list_data
     */
    void RoseHomeUserPick::slot_applyResult_albums(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);
        this->list_album->append(list_data);
        this->createList_itemAlbum_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_album, 0, SECTION_FOR_MORE_POPUP___albums);

        if(list_data.size() > 0){
            tmp_label_subTitle_Album->setText(QString("(%1)").arg(list_data.at(0).totalCount));
        }
    }


    /**
     * @brief [slot] playlist 데이터를 받아서 처리함
     * @param list_data
     * @param jsonArr_dataToPlay
     * @param flag_lastPage
     */
    void RoseHomeUserPick::slot_applyResult_playlists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        Q_UNUSED(jsonArr_dataToPlay);
        this->list_playlist->append(list_data);
        this->createList_itemPlaylsit_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_playlist, 0, SECTION_FOR_MORE_POPUP___playlists);

        if(list_data.size() > 0){
            tmp_label_subTitle_Playlist->setText(QString("(%1)").arg(list_data.at(0).totalCount));
        }
    }




    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */


    void RoseHomeUserPick::slot_clickedItemAlbum(const tidal::ItemAlbum::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();
        print_debug();
        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }



    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */

    void RoseHomeUserPick::slot_clickedItemPlaylist(const tidal::ItemPlaylist::ClickMode clickMode){//c220511
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();
//print_debug();
        // ClickMode 별로 처리
        print_debug();
        qDebug() << "index = " << index;
        qDebug() << "section = " << section;
        if(section >= 2000){
            this->proc_clicked_itemPlaylist(this->list_UserPickPlaylist_All[section-2000], clickMode, index, section);

        }else{
            this->proc_clicked_itemPlaylist(this->list_UserPickPlaylist[section-1000], clickMode, index, section);

        }

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] OptMorePopup (더보기팝업에서 발생한 signal 처리)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    /*
    void RoseHomeUserPick::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }
    */


    /**
     * @brief 메인화면의 초기 UI 설정 > 앨범 파트
     */
    void RoseHomeUserPick::setUIControl_album(){

    this->tmp_label_subTitle_Album = new QLabel();
    this->addUIControl_subTitle_common(TIDAL_SUB_TITLE___album);
    this->hBox_album = this->get_addUIControl_hBoxLayout_forAlbum();
        /*
    print_debug();
        //cheon211008--start
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0,0,0,0);
        tmp_hBox->setAlignment(Qt::AlignTop);
    print_debug();
        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0,5,33,5);
        widget_box_subTitle->setLayout(tmp_hBox);
    print_debug();
        box_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);
        // 상단 필터
        QPushButton *btn_filter = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
        btn_filter->setFixedSize(50,70);
        btn_filter->setCursor(Qt::PointingHandCursor);
    print_debug();
        QLabel *lb_title = new QLabel(tr("Filter"));
        lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;font:bold;");
    print_debug();
        QHBoxLayout *hl_lb = new QHBoxLayout();
        hl_lb->setContentsMargins(0,0,0,0);
        hl_lb->setSpacing(0);
        hl_lb->addWidget(lb_title);
    print_debug();
        QWidget *widget_title = new QWidget();
        widget_title->setObjectName("widget_title");
        widget_title->setContentsMargins(0,0,0,0);
        widget_title->setLayout(hl_lb);
        widget_title->setStyleSheet("#widget_title { background-color:transparent; } QLabel { background-color:transparent; }");
    print_debug();
        QHBoxLayout *hl_filter = new QHBoxLayout();
        hl_filter->setContentsMargins(0,0,90,0);
        hl_filter->setSpacing(0);
        hl_filter->setAlignment(Qt::AlignRight);
        hl_filter->addWidget(btn_filter);
        hl_filter->addWidget(widget_title);
    print_debug();

        QWidget *widget_btnFilter = new QWidget();//cheon211008
        widget_btnFilter->setObjectName("widget_btnFilter");
        widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#212121; }");
        widget_btnFilter->setFixedHeight(70);

        widget_btnFilter->setLayout(hl_filter);
    print_debug();
        QWidget *widget_line = new QWidget();
        widget_line->setFixedHeight(1);
        widget_line->setStyleSheet("background-color:#171717;");
        widget_line->setContentsMargins(0,0,0,0);

    print_debug();
        QWidget *line_bottom_1 = new QWidget();
        line_bottom_1->setFixedHeight(1);
        line_bottom_1->setStyleSheet("background-color:#FFFFFF;");
        QWidget *line_bottom_2 = new QWidget();
        line_bottom_2->setFixedHeight(1);
        line_bottom_2->setStyleSheet("background-color:#282828;");
    print_debug();
        //cheon211008--end
        QVBoxLayout *vl_total = new QVBoxLayout();
        vl_total->setContentsMargins(0,0,0,0);
        vl_total->setSpacing(0);
        vl_total->addWidget(widget_btnFilter);//cheon211008
        vl_total->addWidget(line_bottom_1);
        vl_total->addWidget(line_bottom_2);
    print_debug();
        tmp_hBox->addLayout(vl_total);
        this->hBox_album->addLayout(tmp_hBox);
    print_debug();
    */

    }




    /**
     * @brief 메인화면의 초기 UI 설정 > Playlist 파트
     */
    void RoseHomeUserPick::setUIControl_playlist(){
        this->tmp_label_subTitle_Playlist = new QLabel();
        //this->addUIControl_subTitle_withSideBtn_common(TIDAL_SUB_TITLE___playlist, tr("View All"), BTN_IDX_SUBTITLE_playlists);
        //this->addUIControl_subTitle_withSideBtn_common_cnt(TIDAL_SUB_TITLE___playlist, tr("View All"), BTN_IDX_SUBTITLE_playlists, this->tmp_label_subTitle_Playlist);
        this->addUIControl_subTitle_common(TIDAL_SUB_TITLE___playlist);
        this->hBox_playlist = this->get_addUIControl_hBoxLayout_forPlaylist();

        QWidget *widget_box_sub1 = new QWidget();
        widget_box_sub1->setContentsMargins(0,5,33,5);
        widget_box_sub1->setLayout(this->hBox_playlist );

        GSCommon::clearLayout(this->hBox_playlist);

        this->addUIControl_subTitle_common(TIDAL_SUB_TITLE___playlist);
        this->hBox_playlist = this->get_addUIControl_hBoxLayout_forPlaylist();

        QWidget *widget_box_sub2 = new QWidget();
        widget_box_sub2->setContentsMargins(0,5,33,5);
        widget_box_sub2->setLayout(this->hBox_playlist );

        QVBoxLayout *Vl_box1 = new QVBoxLayout();
        Vl_box1->setContentsMargins(0,0,0,0);
        Vl_box1->setSpacing(0);
        Vl_box1->addWidget(widget_box_sub1);
        Vl_box1->addWidget(widget_box_sub2);

    }

    void RoseHomeUserPick::wheelEvent(QWheelEvent *event){

    #if defined(Q_OS_WINDOWS)

        if(event->angleDelta().ry() <= -120){//cheon210607-test
    #endif

    #if defined(Q_OS_MAC)

        if(event->angleDelta().ry() <= -120 ){//cheon210607-test
    #endif
            print_debug();
            if(userPick_wheel_flag == true ){
print_debug();
                userPick_wheel_flag = false;
                if(totalCount_friend > recursive_cnt_friend+1){
print_debug();
                    ToastMsg::show(this, "", tr("next."),500);
                    page++;
                    QJsonObject json;
                    QUrlQuery params;
                    NetworkHttp *net_new_playlist = new NetworkHttp(this);
                    connect(net_new_playlist, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                    net_new_playlist->request(HTTP_USERPICK_PLAYLIST_FRIEND, QString("%1/member/playlist/userpick/friend?page=%2&size=20&sortType=PLAYLIST_RECENT").arg(global.legacy_v1).arg(page), json, false, true);
                }else{
                    if(recursive_cnt_all == 0){
print_debug();
                        page2++;
                        QJsonObject json2;
                        QUrlQuery params2;
                        NetworkHttp *net_new_playlist2 = new NetworkHttp(this);
                        connect(net_new_playlist2, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                        net_new_playlist2->request(HTTP_USERPICK_PLAYLIST_ALL, QString("%1/member/playlist/userpick/all?page=%2&size=20&sortType=PLAYLIST_RECENT").arg(global.legacy_v1).arg(page2), json2, false, true);

                    }
print_debug();
                    if(totalCount_all > recursive_cnt_all+1){
print_debug();
                        ToastMsg::show(this, "", tr("next."),500);
                        page2++;
                        QJsonObject json2;
                        QUrlQuery params2;
                        NetworkHttp *net_new_playlist2 = new NetworkHttp(this);
                        connect(net_new_playlist2, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                        net_new_playlist2->request(HTTP_USERPICK_PLAYLIST_ALL, QString("%1/member/playlist/userpick/all?page=%2&size=20&sortType=PLAYLIST_RECENT").arg(global.legacy_v1).arg(page2), json2, false, true);

                    }

                }

        }



        }
    #if defined(Q_OS_WINDOWS)

        //qDebug() << "event->angleDelta()" << event->angleDelta();
        if(event->angleDelta().ry() >= 120 ){//cheon210607-test
    #endif

    #if defined(Q_OS_MAC)

        if(event->angleDelta().ry() >= 120 ){//cheon210607-test
    #endif

            //getTrackDataFromDB_pre();//cheon210608-test
            ToastMsg::show(this, "", tr("pre."),500);
        }

        //qDebug() << "event->angleDelta()" << event->angleDelta();

    }

}





