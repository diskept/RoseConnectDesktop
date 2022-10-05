#include "userpickUserpage.h"
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
//#include "common/networkhttp.h"
#include <delegate/filedelegate.h>
#include "widget/toastmsg.h"
#include "ConvertData_rosehome.h"




namespace roseHome {


    const int HTTP_USERPICK_PLAYLIST_FRIEND = 8;
    const int HTTP_USERPICK_PLAYLIST_ALL = 9;
    const int BTN_IDX_SUBTITLE_playlists_Friend = 3;
    const int BTN_IDX_SUBTITLE_playlists_All = 4;
    //const int BTN_IDX_SUBTITLE_playlists= 1;
    //const int BTN_IDX_SUBTITLE_albums = 2;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 2;

    //const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 15;

    const QString TIDAL_API_PATH_ALBUM = "master/recommended/albums";
    const QString TIDAL_API_PATH_PLAYLIST = "master/recommended/playlists";

    const QString TIDAL_MAIN_TITLE = "Master";
    const QString TIDAL_SUB_TITLE___album = "FrenzPeter";
    const QString TIDAL_SUB_TITLE___playlist = "pinpin";


    const int PROFILEIMG_SIZE = 270;
    const int PHOTOS_SIZE = 120;
    const int HTTP_GET_USERINFO = 99;

    /**
     * @brief "TIDAL > 탐색 > NEW 메인" 화면의 생성자
     * @param parent
     */
    userpickUserpage::userpickUserpage(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll, parent){//cheon211115-01
       // print_debug();
        //GSCommon::clearLayout(this->vl_userPage);
        this->flagInitDraw = false;
        lb_profile_grade = new QLabel();
        lb_profile = new QLabel();
        lb_profile_grade->setFixedSize(PROFILEIMG_SIZE,PROFILEIMG_SIZE);
        lb_profile->setFixedSize(PROFILEIMG_SIZE,PROFILEIMG_SIZE);
        lb_profile_grade->raise();

        lb_nickName = new QLabel();
        lb_nickName->setStyleSheet("color:#FFFFFF;font-size:36px;");
        //lb_nickName->setText("PPPPPPPPPPPPPPPPPPPPPPPPPP");
        lb_comment = new QLabel();
        lb_comment->setWordWrap(true);
        lb_comment->setStyleSheet("color:#CCCCCC;font-size:20px;");
        lb_nickName->setContentsMargins(25,0,0,0);
        lb_comment->setContentsMargins(25,0,0,0);

        QPushButton *btn_edit = new QPushButton();
        btn_edit->setProperty("goPageIndex", 1);
        btn_edit->setText(tr("Edit"));
        //btn_edit->setText(tr("편집"));
        btn_edit->setFixedSize(100,40);
        btn_edit->setStyleSheet("color:#E6E6E6;font-size:18px;border:1px solid #666666;border-radius:20px;");
        btn_edit->setCursor(Qt::PointingHandCursor);
        QPushButton *btn_logout = new QPushButton();
        btn_logout->setText(tr("Log out"));
        //btn_logout->setText(tr("로그아웃"));
        btn_logout->setFixedSize(100,40);
        btn_logout->setStyleSheet("color:#E6E6E6;font-size:18px;border:1px solid #666666;border-radius:20px;");
        btn_logout->setCursor(Qt::PointingHandCursor);

        QHBoxLayout *hl_btn = new QHBoxLayout();
        hl_btn->addWidget(btn_edit);
        hl_btn->addSpacing(25);
        hl_btn->addWidget(btn_logout);
        hl_btn->addSpacing(47);

        lb_email = new QLabel();
        lb_email->setContentsMargins(25,0,0,0);
        //lb_email->setAlignment(Qt::AlignLeft);
        lb_email->setStyleSheet("color:#FFFFFF;font-size:20px;");
        lb_email->setText("888888888888888888888888");

        QGridLayout *gl_myInfo = new QGridLayout();
        gl_myInfo->setContentsMargins(0,0,0,0);
        gl_myInfo->setSpacing(0);
        gl_myInfo->addWidget(lb_profile, 0, 0, 4, 1);
        gl_myInfo->addWidget(lb_profile_grade, 0, 0, 4, 1);
        gl_myInfo->addWidget(lb_nickName, 1, 1);
        gl_myInfo->addWidget(lb_comment, 2, 1, 1, 2);
        gl_myInfo->addWidget(lb_email, 3, 1 ,1,1);
       // gl_myInfo->addLayout(hl_btn, 0, 1, 1, 2, Qt::AlignRight);



        // 내 사진 리스트
        hl_photo = new QHBoxLayout();
        hl_photo->setContentsMargins(0,0,0,0);
        hl_photo->setSpacing(18);
        hl_photo->setAlignment(Qt::AlignLeft);

        QVBoxLayout *vl_myInfo = new QVBoxLayout();
        vl_myInfo->setContentsMargins(0,0,0,0);
        vl_myInfo->setSpacing(0);
        vl_myInfo->addLayout(gl_myInfo);
        vl_myInfo->addSpacing(37);
        vl_myInfo->addLayout(hl_photo);

        QWidget *widget_myInfoView = new QWidget();
        widget_myInfoView->setContentsMargins(40,30,30,40);
        widget_myInfoView->setObjectName("widget_myInfoView");
        widget_myInfoView->setStyleSheet("#widget_myInfoView { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");
        widget_myInfoView->setLayout(vl_myInfo);
        //widget_myInfoView->setFixedHeight(490);

        //this->box_contents->addWidget(widget_myInfoView);
        this->vbox_member_contents = new QVBoxLayout();//cheon211115-01
        this->vbox_member_contents->setSpacing(30);//cheon211115-01
        this->vbox_member_contents->setContentsMargins(0, 0, 0, 0);//cheon211115-01
        //-------------------------------------
        //this->vbox_member_contents->addWidget(lb_email);
        //----------------------------------------

        vl_userPage = new QVBoxLayout();
        vl_userPage->setContentsMargins(0,0,0,0);
        vl_userPage->setSpacing(0);
        vl_userPage->setAlignment(Qt::AlignTop);
        vl_userPage->addWidget(widget_myInfoView);         // 내 정보
        vl_userPage->addSpacing(40);
        vl_userPage->addLayout(vbox_member_contents);//cheon211115-01


        //this->vbox_last_contents->addWidget(widget_box_subTitle2, 0, Qt::AlignTop);
        //this->vbox_last_contents->addWidget(widget_title_main_user);//cheon211115-01

        //this->box_contents->addLayout(vbox_last_contents);//cheon211115-01

        this->box_contents->addLayout(vl_userPage);
    }


    /**
     * @brief 소멸자.
     */
    userpickUserpage::~userpickUserpage(){

        //print_debug();
        //GSCommon::clearLayout(this->hbox_UserPickPlaylist);
        //print_debug();
        this->list_MemberPlaylist->clear();
        //print_debug();

    }
    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void userpickUserpage::setJsonObject_forData(const QJsonObject &jsonObj){
//print_debug();
        PageInfo_UserPage data_pageInfo = ConvertData::getObjectJson_pageInfo_UserPage(jsonObj);


        //if(this->curr_api_subPath != data_pageInfo.api_subPath){
            print_debug();
            //this->flagNeedReload = true;
            //this->label_mainTitle->setText(data_pageInfo.pathTitle);
            this->api_subPath = data_pageInfo.api_subPath;
            this->type = data_pageInfo.type;
            this->member_id = data_pageInfo.member_id;

            this->type_id = data_pageInfo.type_id;
            this->curr_api_subPath = this->api_subPath;


            //this->playlist_count = 0;
            //this->total_playlist_count = 0;
            //this->pageCNT = 0;
            //this->playlist_beforeOffset = 0;
            //this->playlist_currentOffset = 0;
        //}
    }


    /**
     * @brief 타이달 > 탐색 > "장르" 1개 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void userpickUserpage::setActivePage(){//cheon211018

        // 항상 부모클래스의 함수 먼저 호출
        AbstractRoseHomeSubWidget::setActivePage();
        print_debug();
        qDebug() << "this->curr_api_subPath = " << this->curr_api_subPath;
        qDebug() << "this->member_id = " << this->member_id;
        qDebug() << "this->flagInitDraw = " << this->flagInitDraw;

        if(this->flagInitDraw){
            print_debug();
            this->list_MemberPlaylist->clear();
            //GSCommon::clearLayout(this->hbox_MemberPlaylist);
            //GSCommon::clearLayout(this->vl_userPage);
            GSCommon::clearLayout(this->vbox_member_contents);
            //GSCommon::clearLayout(this->vl_userPage);
            //this->list_MemberPlaylist->clear();
            //this->flagNeedReload =false;
            //this->list_UserPickPlaylist->clear();
            //this->list_UserPickPlaylist_All->clear();



        }else{
            print_debug();
            this->flagInitDraw = true;
        }
        page = 0;
        QJsonObject json;
        QUrlQuery params;
        NetworkHttp *net_userPage = new NetworkHttp(this);
        connect(net_userPage, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friendInfo(int, QJsonObject)));
        net_userPage->request(HTTP_GET_USERINFO, QString("%1/%2?page=%3&size=100").arg(this->curr_api_subPath).arg(this->member_id).arg(page), json, false, true);

    }



    void userpickUserpage::setUIControl_FriendProfile(){

    }

    void userpickUserpage::slot_responseHttp_friendInfo(const int &p_id, const QJsonObject &p_jsonObject){
        //print_debug();
        //qDebug() << "********** p_id== "<< p_id;
        NetworkHttp *network2 = new NetworkHttp(this);
        connect(network2, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_friendInfo(int, QJsonObject)));

        QString userpickName;

        if(p_id == HTTP_GET_USERINFO){
            print_debug();
            //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "userpickUserpage::slot_responseHttp---" << strJson;//
            this->setResponseProfile(p_jsonObject);

            //print_debug();

            int item_cnt = 0;
            userpickName = "Friend Playlist Album";

            this->setUIControl_Member_subTitle_withSideBtn(userpickName, 0, ProcJsonEasy::getInt(p_jsonObject, "totalCount"));

            this->FlowLayout_userPick = this->setUIControl_hBoxLayout_forAlbum_member(296, HTTP_USERPICK_PLAYLIST_FRIEND, 0, item_cnt);
            this->FlowLayout_userPick = new FlowLayout(0, 20, 0);

            this->list_MemberPlaylist = new QList<roseHome::PlaylistItemData>();

            GSCommon::clearLayout(this->FlowLayout_userPick);

            QWidget *widget_background = new QWidget();
            widget_background->setLayout(this->FlowLayout_userPick);
            widget_background->setContentsMargins(0,0,0,0);
            widget_background->setStyleSheet("background-color:#212121; border:0px;");

            this->vbox_member_contents->addWidget(widget_background, 1, Qt::AlignTop);

            //GSCommon::clearLayout(this->FlowLayout_userPick);

            QJsonObject json2;
            network2->request(555, QString("%1/member/playlist/member/%2?page=0&pageSize=50&sortType=PLAYLIST_RECENT")
                             .arg(global.legacy_v1).arg(this->member_id), json2, false, true);


        }else if(p_id == 555){
            //print_debug();
            userpickName = "Friend Playlist Album";

            this->lb_subTitle->setText(QString("%1  (%2)").arg(userpickName).arg(ProcJsonEasy::getInt(p_jsonObject, "totalCount")));
            if(page > ProcJsonEasy::getInt(p_jsonObject, "totalCount")/20+1) {

                this->createList_itemPlaylsit_applyingWithData(*this->list_MemberPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->FlowLayout_userPick, 0, 1004);
                return;
            }
            page++;
            this->setResultOfMemberPlaylist(p_jsonObject);

            //print_debug();
            QJsonObject json2;
            network2->request(555, QString("%1/member/playlist/member/%2?page=%3&pageSize=50&sortType=PLAYLIST_RECENT")
                              .arg(global.legacy_v1).arg(this->member_id).arg(page),json2, false, true);

        }

    }



    FlowLayout* userpickUserpage::setUIControl_hBoxLayout_forAlbum_member(const int hbox_height, const int btnId, const int i, const int item_cnt){//cheon211115-01
        //----------------------------------------------------------------------------------------------------  BODY : START

        FlowLayout *flowLayout = new FlowLayout(this,0,18,20);

        QWidget *widget_background = new QWidget();
        //widget_background->setLayout(vl_total);
        widget_background->setLayout(flowLayout);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");

        this->vbox_member_contents->addWidget(widget_background, 1, Qt::AlignTop);

        return flowLayout;
    }


    void userpickUserpage::setResponseProfile(QJsonObject p_jsonObject){
        //QJsonDocument doc(p_jsonObject);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << "RoseHomeUserPick::setResponseProfile : " << strJson;

        bool flagOk = false;
        if(p_jsonObject.contains("flagOk")){
            flagOk = p_jsonObject["flagOk"].toBool();
        }

        if(flagOk == false){
            // 다른 기기 로그인 하였거나 다른 문제로 인해 .. 다시 로그인 시도하게 해야한다...
            //ToastMsg::show(this, "", tr("Account Information has been changed."));
            //ToastMsg::show(this, "", tr("계정정보가 변경되었습니다."));

        }else{
            global.user.setUserInfo(p_jsonObject);

            // 뷰어화면 초기화
            this->lb_comment->clear();
            this->lb_profile->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/LV_VIP_image.png"));
            this->lb_profile_grade->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/LV_VIP_outline.png"));

            // 편집화면 초기화
            GSCommon::clearLayout(this->hl_photo);
           // GSCommon::clearLayout(this->flowlayout_photos);
    //        this->le_pw->setText("");
    //        this->le_comment->clear();
            //flowlayout_photos->addWidget(getUIAddPhotoWidget()); // TEST : 기능 구현 안되어있어서 hide 함
            //this->list_myPhotoEdit.clear();
        //    this->lb_title_regPhoto->setVisible(false);
      //     this->widget_flow->setVisible(false);
            QString tmp_imgURL_main = "";

            if(p_jsonObject.contains("member")){
                QJsonObject tmp_jsonObject_member = p_jsonObject["member"].toObject();

                // 기본 5등급부터 시작.. output에 등급 key 없음..
                // 그래서, 현재 항상 무조건 5등급의 기본이미지 세팅
                this->lb_profile_grade->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/LV_VIP_outline.png"));

                if(tmp_jsonObject_member.contains("name")){
                    this->lb_nickName->setText(tmp_jsonObject_member["name"].toString());
              //      this->le_nickName->setText(tmp_jsonObject_member["name"].toString());
                    this->data_profileInfo.username = tmp_jsonObject_member["name"].toString();
                }
                if(tmp_jsonObject_member.contains("comment")){
                    this->lb_comment->setText(tmp_jsonObject_member["comment"].toString());
                //    this->le_comment->setText(tmp_jsonObject_member["comment"].toString());
                    this->data_profileInfo.bio = tmp_jsonObject_member["comment"].toString();
                }
                else{
                    this->lb_comment->setText(tr("소개글이 없습니다."));
                  //  this->le_comment->setText(tr("소개글이 없습니다."));
                }
                if(tmp_jsonObject_member.contains("email")){
                    this->lb_email->setText(tmp_jsonObject_member["email"].toString());
                    this->data_profileInfo.email = tmp_jsonObject_member["email"].toString();
                }
                if(tmp_jsonObject_member.contains("profileImage")){
                    tmp_imgURL_main = tmp_jsonObject_member["profileImage"].toString();
                    if(tmp_imgURL_main!=""){

                        FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
                        connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImageMainProfile()));
                        tmp_fileDownLoader->setImageURL(QUrl(tmp_imgURL_main));
                    }
                }
                if(tmp_jsonObject_member.contains("images")){
                    QJsonArray tmp_arr = tmp_jsonObject_member["images"].toArray();
                    for(int i=0; i<tmp_arr.count(); i++){
                        // 뷰어화면의 사진목록
                        QLabel *lb_photo = new QLabel();
                        lb_photo->setFixedSize(PHOTOS_SIZE,PHOTOS_SIZE);
                        this->hl_photo->addWidget(lb_photo);

                        // 편집화면의 사진 목록
                   //     MyPhotoEdit *myPhotoEdit = new MyPhotoEdit();
                   //     myPhotoEdit->setDataMyPhotoEdit(tmp_arr[i].toString());
                        //this->list_myPhotoEdit.append(myPhotoEdit);
                        //connect(myPhotoEdit, &MyPhotoEdit::signal_deletePhoto, this, &userpickUserpage::slot_deletePhoto);
                  //      this->flowlayout_photos->addWidget(myPhotoEdit);

                        FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
                        tmp_fileDownLoader->setProperty("index", i);
                        connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImagePhotos()));
                        tmp_fileDownLoader->setImageURL(QUrl(tmp_arr[i].toString()));
                    }

                    // TEST : 기능 구현 안되어있어서 hide/show 함
                   // if(tmp_arr.count()>0){
                   //     this->lb_title_regPhoto->setVisible(true);
                   //     this->widget_flow->setVisible(true);
                   // }
                }
            }
        }
    }

    /**
     * @brief SettingMyPage::slot_loadImageMainProfile [SLOT] 프로필 메인 이미지 세팅
     */
    void userpickUserpage::slot_loadImageMainProfile(){

        FileDownloader *tmp_fileDownloader = qobject_cast<FileDownloader*>(sender());
        QPixmap *pixmap_albumImg = new QPixmap();
        bool flagLoad = pixmap_albumImg->loadFromData(tmp_fileDownloader->downloadedData());
        if(flagLoad){
            QPixmap tmp_pixmap = pixmap_albumImg->scaled(PROFILEIMG_SIZE, PROFILEIMG_SIZE, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation); // KeepAspectRatio

            // 빈 Pixmap
            QPixmap pixmap_painter = QPixmap(QSize(PROFILEIMG_SIZE,PROFILEIMG_SIZE));
            pixmap_painter.fill(Qt::transparent);

            QPainter painter (&pixmap_painter);
            painter.setRenderHint(QPainter::Antialiasing, true);
            QBrush brush = QBrush(tmp_pixmap);
            painter.setBrush(brush);
            painter.drawRoundedRect(0, 0, PROFILEIMG_SIZE, PROFILEIMG_SIZE, PROFILEIMG_SIZE/2, PROFILEIMG_SIZE/2);    // border-radius:4px
            this->lb_profile->setPixmap(pixmap_painter);
            //this->lb_profileEdit->setPixmap(pixmap_painter);
        }

        tmp_fileDownloader->deleteLater();
    }


    /**
     * @brief SettingMyPage::slot_loadImagePhotos [SLOT] 프로필 여러 사진 세팅
     */
    void userpickUserpage::slot_loadImagePhotos(){

        FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
        int tmp_index = fileDownloader->property("index").toInt();

        if(tmp_index>=0 && tmp_index<hl_photo->count()){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());

            if(flagLoaded){
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(PHOTOS_SIZE,PHOTOS_SIZE, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

                dynamic_cast<QLabel*>(this->hl_photo->itemAt(tmp_index)->widget())->setPixmap(tmp_pixmap);
                //listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
                //QPixmapCache::insert(dataTracks->at(tmp_index)->getThumbnail(), tmp_pixmap);
            }
        }

        fileDownloader->deleteLater();
    }


    void userpickUserpage::setResultOfMemberPlaylist(const QJsonObject &p_jsonObject){

        if(ProcJsonEasy::get_flagOk(p_jsonObject) == true){


            QVBoxLayout *vl_total = new QVBoxLayout();
            vl_total->setContentsMargins(0,0,0,0);
            vl_total->setSpacing(0);
            //vl_total->setAlignment(Qt::AlignLeft);//cheon211119-01

            //print_debug();
            //lb_title_type->setText(QString(tr("Friend Playlist (%1)")).arg(jsonArruserpick.size()));//cheon211115-01


            //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\n userpickUserpage::setResultOfUserPickPlaylist--strJson= " << strJson;

            QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObject, "playlists");
            //print_debug();
            //qDebug()<<"\nRoseHomeUserPick::setResultOfUserPickPlaylist---jsonArrSub.count()---"<<jsonArrSub.count();

            if(jsonArrSub.size() > 0){
                //this->list_UserPickPlaylist->clear();
                for(int i = 0; i < jsonArrSub.count(); i++){
                    QJsonObject tmp_json = jsonArrSub.at(i).toObject();
                    //print_debug();
                    //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));
                    //qDebug() << "\ni = " << i << ", userpickUserpage::setResultOfUserPickPlaylist--tmp_json= " << tmp_json;
                    roseHome::PlaylistItemData tmp_data;

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
                    this->list_MemberPlaylist->append(tmp_data);
                }

            }
            //this->setUIControl_Append_MemberContents(ProcJsonEasy::getInt(p_jsonObject, "page"), true, "Friend Playlist Album", ProcJsonEasy::getInt(p_jsonObject, "totalCount"));//cheon211115-01


        }
    }




    void userpickUserpage::setUIControl_Append_MemberContents(const int page, const bool flag, const QString userpickName,  const int item_cnt){//cheon211115-01

//print_debug();
//qDebug()<< "list_MemberPlaylist->length() = " << list_MemberPlaylist->count();
        this->createList_itemPlaylsit_applyingWithData(*this->list_MemberPlaylist, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->FlowLayout_userPick, 20*page, 1004);
//qDebug()<< "list_MemberPlaylist->length() = " << list_MemberPlaylist->count();
    }

    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */

    void userpickUserpage::slot_clickedItemPlaylist(const tidal::ItemPlaylist::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();
//print_debug();
        // ClickMode 별로 처리
        print_debug();
        qDebug() << "index = " << index;
        qDebug() << "section = " << section;
        qDebug()<< "list_MemberPlaylist->length() = " << list_MemberPlaylist->length();
        this->proc_clicked_itemPlaylist(this->list_MemberPlaylist, clickMode, index, section);
    }

    void userpickUserpage::setUIControl_Member_subTitle_withSideBtn(const QString subTitle, const int i, const int item_cnt){

        //QListWidgetItem *item = listWidget->item(i);

        this->lb_subTitle = new QLabel();
        //this->lb_subTitle->setText(subTitle);
        this->lb_subTitle->setText(QString(subTitle+"  (%1)").arg(item_cnt));

        this->lb_subTitle->setStyleSheet("font-size:18px;font-weight:bold;color:#FFFFFF;");


        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(10);
        tmp_hBox->setContentsMargins(0,0,0,0);
        //tmp_hBox->setAlignment(Qt::AlignTop);

print_debug();

        tmp_hBox->addWidget(this->lb_subTitle);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(10,5,5,0);

        widget_box_subTitle->setLayout(tmp_hBox);

        this->vbox_member_contents->addWidget(widget_box_subTitle, 1, Qt::AlignTop);

    }


}
