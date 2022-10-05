#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"
#include "tidal/PushBtn_withID.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"

#include "login/dialoglogin_fortidal.h"     // 타이달 로그인

#include "widget/OptMorePopup.h"
#include "widget/VerticalScrollArea.h"
#include "widget/toastmsg.h"
#include "widget/NoData_Widget.h"

#include <QPixmapCache>
#include <QSettings>
#include "roseHome/ProcCommon_forRosehome.h"//c220824-1


namespace tidal {

    /**
     * @brief 타이달 서브페이지 위젯을 위한 부모 클래스
     * @param parent
     */
    AbstractTidalSubWidget::AbstractTidalSubWidget(MainUIType p_mainUIType, QWidget *parent) : common::AbstractCommonSubWidget(p_mainUIType, parent) {

        // to check memory address number
        linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_Artist_movePage(QJsonObject)), SLOT(slot_dlg_signal_artist(QJsonObject)));
    }

    /**
     * @brief 소멸자 체크.
     */
    AbstractTidalSubWidget::~AbstractTidalSubWidget(){

        // to check memory address number
        this->deleteLater();
    }

    /**
     * @brief 사용자가 Bugs 로그인하지 않았을 경우의 화면을 위한 Widget
     */
    void AbstractTidalSubWidget::setUIControl_notLogin(){
        // 로그인 하지 않았을 때의 화면
        QLabel *label_topComment = new QLabel(tr("You can use various services by logging in."));
        //QLabel *label_topComment = new QLabel(tr("로그인하시면 다양한 서비스를 이용하실 수 있습니다."));
        label_topComment->setStyleSheet("font-size: 18px; color: #ffffff;");
        label_topComment->setContentsMargins(0,0,0,0);
        label_topComment->setAlignment(Qt::AlignCenter);
        QPushButton *btn_login = new QPushButton();
        btn_login->setText(tr("Log-in"));//btn_login->setText(tr("로그인"));
        btn_login->setFixedHeight(40);
        btn_login->setStyleSheet("font-size:18px; color:#FFFFFF; background-color:#B18658; border-radius:20px; padding-left:44px; padding-right:44px;");
        btn_login->setCursor(Qt::PointingHandCursor);
        btn_login->setContentsMargins(0,0,0,0);
        connect(btn_login, &QPushButton::clicked, this, &AbstractTidalSubWidget::slot_clickBtnLogin_toShowDialog);

        // 라벨과 버튼을 세로로 배치
        QVBoxLayout *vBox = new QVBoxLayout();
        vBox->setSpacing(0);
        vBox->setContentsMargins(0,0,0,0);
        vBox->addWidget(label_topComment);
        vBox->addSpacing(30);
        vBox->addWidget(btn_login, 0, Qt::AlignCenter);

        vBox->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);

        // Widget
        QWidget *widget_notLogin = new QWidget();
        widget_notLogin->setContentsMargins(0,0,0,0);
        widget_notLogin->setLayout(vBox);


        this->box_contents->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);
        this->box_contents->addWidget(widget_notLogin, 0, Qt::AlignVCenter | Qt::AlignCenter);

    }








    /**
     * @brief TIDAL 로그인이 필요하다는 화면 Widget을 반환한다. box_layout에 넣지는 않음
     * @return
     */
    QWidget* AbstractTidalSubWidget::get_createUIControl_notLoginView(){

        QLabel *label_topComment = new QLabel(tr("You can use it by logging in to your TIDAL account."));
        //QLabel *label_topComment = new QLabel(tr("TIDAL 계정을 로그인하시면 이용하실 수 있습니다."));
        label_topComment->setStyleSheet("font-size: 18px; color: #ffffff;");
        label_topComment->setContentsMargins(0,0,0,0);
        label_topComment->setAlignment(Qt::AlignCenter);

        QPushButton *btn_login = new QPushButton();
        btn_login->setText(tr("Log-in"));
        //btn_login->setText(tr("로그인"));
        btn_login->setFixedHeight(40);
        btn_login->setStyleSheet("font-size:18px; color:#FFFFFF; background-color:#B18658; border-radius:20px; padding-left:44px; padding-right:44px;");
        btn_login->setCursor(Qt::PointingHandCursor);
        btn_login->setContentsMargins(0,0,0,0);
        connect(btn_login, &QPushButton::clicked, this, &AbstractTidalSubWidget::slot_clickBtnLogin_toShowDialog);

        // 라벨과 버튼을 세로로 배치
        QVBoxLayout *vBox = new QVBoxLayout();
        vBox->setSpacing(0);
        vBox->setContentsMargins(0,0,0,0);
        vBox->addWidget(label_topComment);
        vBox->addSpacing(30);
        vBox->addWidget(btn_login, 0, Qt::AlignCenter);

        vBox->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);

        // Widget
        QWidget *widget_output = new QWidget();
        widget_output->setContentsMargins(0,0,0,0);
        widget_output->setLayout(vBox);


        // 화면에 보이도록 설정
//        GSCommon::clearLayout(this->box_contents);
//        this->box_contents->addWidget(widget_output, 1);
//        this->box_contents->setAlignment(Qt::AlignCenter);


        // return
        return widget_output;

    }




    /**
     * @brief "로그인" 버튼 클릭 처리 - 로그인 다이얼로그 띄우기
     */
    void AbstractTidalSubWidget::slot_clickBtnLogin_toShowDialog(){
        //DialogLogin_forTidal *dlg = new DialogLogin_forTidal(this);
        //connect(dlg, &DialogLogin_forTidal::successLogin, this, &AbstractTidalSubWidget::slot_acceptedDialogLogin);
        //dlg->exec();

        DialogLogin_forTidal *dlg = new DialogLogin_forTidal(this);
        connect(dlg, &DialogLogin_forTidal::signal_successLogin, this, &AbstractTidalSubWidget::slot_dialogLogin_acceptedAuthCode);
        dlg->exec();
    }


    /**
     * @brief [Slot] 벅스 로그인 Dialog에서 authCode 정상적으로 발급된 경우의 처리
     */
    void AbstractTidalSubWidget::slot_dialogLogin_acceptedAuthCode(){
        // token 요청
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::signal_completeReq_getToken, this, &AbstractTidalSubWidget::slot_getToken);
        proc->request_tidal_getTokenAndSave();
    }


    /**
     * @brief AbstractBugsSubWidget::slot_success_getToken
     * @param p_bugsTokenInfo
     */
    void AbstractTidalSubWidget::slot_getToken(const bool flagSuccess, const QString errorMsg){
        if(flagSuccess){
            ProcCommon *proc = new ProcCommon(this);
            tidal::TidalTokenInfo ptokenInfo = proc->getTokenInfo_tidalDB();
            //global.user_forTidal.set_loginState(ptokenInfo.access_token, ptokenInfo.refresh_token, "", "", 0, ptokenInfo.email);
            global.user_forTidal.set_loginState(ptokenInfo.access_token, ptokenInfo.refresh_token, global.user_forTidal.getCountryCode(), global.user_forTidal.getSessionId(), global.user_forTidal.getUserId(), ptokenInfo.email);

            // Session 정보를 요청함
            connect(proc, &ProcCommon::signal_completeReq_getSession, this, &AbstractTidalSubWidget::slot_completeReq_getSessionInfo);
            proc->request_tidal_getSession();
        }
        else{
            // error
            this->showToastMsg(errorMsg);
        }
    }


    void AbstractTidalSubWidget::slot_completeReq_getSessionInfo(const QString errorMsg, const QJsonObject& jsonObj_session){
        if(!errorMsg.isEmpty()){
            //refresh token 요청
            if(!global.user_forTidal.get_refresh_token().isEmpty())
            {
                ProcCommon *proc = new ProcCommon(this);
                connect(proc, &ProcCommon::signal_completeReq_getRefreshToken, this, &AbstractTidalSubWidget::slot_getToken);
                proc->request_tidal_getRefreshTokenAndSave();
            }
            else
            {
                tidal::ProcRosePlay_withTidal *procRose = new tidal::ProcRosePlay_withTidal(this);
                connect(procRose, &ProcRosePlay_withTidal::signal_completeReq_get_session_info, this, &AbstractTidalSubWidget::slot_completeReq_get_session_info);
                procRose->request_get_session_info();
            }
        }
        else{
            ProcCommon *proc = new ProcCommon(this);
            // Rose 에게 tidal session 정보를 저장
            tidal::RoseSessionInfo_forTidal sessionInfo;
            sessionInfo.TIDAL_CountryCode = ProcJsonEasy::getString(jsonObj_session, "countryCode");
            sessionInfo.TIDAL_SessionId = ProcJsonEasy::getString(jsonObj_session, "sessionId");
            sessionInfo.TIDAL_UserID = ProcJsonEasy::getInt(jsonObj_session, "userId");

            // 토큰 정보를 global에 저장함
            tidal::TidalTokenInfo ptokenInfo = proc->getTokenInfo_tidalDB();
            sessionInfo.TIDAL_AccessToken = ptokenInfo.access_token;
            sessionInfo.TIDAL_UserName = ptokenInfo.email;

            global.user_forTidal.set_loginState(ptokenInfo.access_token, ptokenInfo.refresh_token, sessionInfo.TIDAL_CountryCode, sessionInfo.TIDAL_SessionId, sessionInfo.TIDAL_UserID, sessionInfo.TIDAL_UserName);

            // Session 정보를 요청함
            connect(proc, &ProcCommon::signal_completeReq_getSubscription, this, &AbstractTidalSubWidget::slot_completeReq_getSubscription);
            proc->request_tidal_getSubscription(sessionInfo.TIDAL_UserID);

            emit linker->signal_logined_tidal();
        }
    }


    void AbstractTidalSubWidget::slot_completeReq_getSubscription(const QString errorMsg, const QJsonObject& jsonObj_subscription){

        if(!errorMsg.isEmpty()){
            this->showToastMsg(errorMsg);
        }
        else{
            // Rose 에게 tidal session + subscription 정보를 전달함
            int highest = 0;
            int type = 0;
            QString tmp_highest = ProcJsonEasy::getString(jsonObj_subscription, "highestSoundQuality");
            QJsonObject tmp_json = ProcJsonEasy::getJsonObject(jsonObj_subscription, "subscription");
            QString tmp_type = ProcJsonEasy::getString(tmp_json, "type");

            if(tmp_highest.compare("LOW") == 0)
            {
                highest = 0;
            }
            else if(tmp_highest.compare("HIGH") == 0)
            {
                highest = 1;
            }
            else if(tmp_highest.compare("LOSSLESS") == 0)
            {
                highest = 2;
            }
            else if(tmp_highest.compare("HI_RES") == 0)
            {
                highest = 3;
            }

            if(tmp_type.compare("NORMAL") == 0)
            {
                type = 0;
            }
            else if(tmp_type.compare("HIGH") == 0)
            {
                type = 1;
            }
            else if(tmp_type.compare("HIFI") == 0)
            {
                type = 2;
            }
            else if(tmp_type.compare("MASTER") == 0)
            {
                type = 3;
            }

            ProcCommon *proc = new ProcCommon(this);
            // 토큰 정보를 global에 저장함
            tidal::TidalTokenInfo ptokenInfo = proc->getTokenInfo_tidalDB();

            // Rose 에게 tidal session 정보를 저장
            tidal::RoseSessionInfo_forTidal sessionInfo;
            sessionInfo.TIDAL_AccessToken = ptokenInfo.access_token;
            sessionInfo.TIDAL_RefreshToken = ptokenInfo.refresh_token;
            sessionInfo.TIDAL_CountryCode = global.user_forTidal.getCountryCode();  //ptokenInfo.country_code;
            sessionInfo.TIDAL_SessionId = global.user_forTidal.getSessionId();    //ptokenInfo.session_id;
            sessionInfo.TIDAL_UserID = global.user_forTidal.getUserId();
            sessionInfo.TIDAL_UserName = ptokenInfo.email;
            sessionInfo.TIDAL_HighestSoundQuality = highest;
            sessionInfo.TIDAL_SoundQuality = type;

            ProcRosePlay_withTidal *procRose = new ProcRosePlay_withTidal(this);
            procRose->request_set_session_info(sessionInfo);

            // 전체 장르 정보를 요청함 - 공통으로 사용 (세팅 안된 경우)
            ProcCommon *procTidal = new ProcCommon(this);
            // Genre, Mood 종류 요청 (Tidal 공통사용)
            procTidal->request_tidal_getListGenres();
            procTidal->request_tidal_getListMoods();


            // 로그인 상태를 반영하여 페이지 변경함
            //this->slot_acceptedDialogLogin();
            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_HOME);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);
        }
    }


    void AbstractTidalSubWidget::slot_completeReq_get_session_info(const RoseSessionInfo_forTidal& sessionInfo){

        // Rose로부터 받은 Session 정보
        if(!sessionInfo.TIDAL_AccessToken.isEmpty()){
            // 로그인 상태를 저장함
            global.user_forTidal.set_loginState(sessionInfo.TIDAL_AccessToken, sessionInfo.TIDAL_RefreshToken, sessionInfo.TIDAL_CountryCode, sessionInfo.TIDAL_SessionId, sessionInfo.TIDAL_UserID, sessionInfo.TIDAL_UserName);
            global.user_forTidal.set_soundState(sessionInfo.TIDAL_HighestSoundQuality, sessionInfo.TIDAL_SoundQuality);
            //global.user_forTidal.setUsername(sessionInfo.TIDAL_UserName);

            ProcRosePlay_withTidal *procRose = new ProcRosePlay_withTidal(this);
            procRose->request_set_session_info(sessionInfo);

            // 전체 장르 정보를 요청함 - 공통으로 사용 (세팅 안된 경우)
            ProcCommon *procTidal = new ProcCommon(this);
            // Genre, Mood 종류 요청 (Tidal 공통사용)
            procTidal->request_tidal_getListGenres();
            procTidal->request_tidal_getListMoods();


            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_HOME);

            // 부모에게 페이지 변경하라고 시그널 보냄
            emit this->signal_clickedMovePage(tmp_data);
        }
        else{
            // 로그아웃으로 처리함
            global.user_forBugs.set_logoutState();
        }
    }


    /**
     * @brief 타이달 다이얼로그에서 로그인 성공 이벤트 발생에 대한 처리
     * @details 토스트 메시지를 띄우고, 로그인한 화면으로 전환
     */
    void AbstractTidalSubWidget::slot_acceptedDialogLogin(){
        ToastMsg::show(this, "", tr("TIDAL service login was successful."));
        //ToastMsg::show(this, "", tr("TIDAL 서비스 로그인에 성공했습니다."));
    }


    void AbstractTidalSubWidget::slot_acceptedDialogLogout(){
        ToastMsg::show(this, "", tr("TIDAL service logout was successful."));
        //ToastMsg::show(this, "", tr("TIDAL 서비스 로그인에 성공했습니다."));
    }


    /**
     * @brief TIDAL 로그인 여부 반환
     * @return
     */
    bool AbstractTidalSubWidget::is_tidal_logined(){
        return global.user_forTidal.isLogined();
    }


    /**
     * @brief 화면에 로그인이 필요하다고 알려줌
     */
    void AbstractTidalSubWidget::showNeededLoginTidal(){
        this->showToastMsg(tr("     TIDAL account login required.     "));
        //this->showToastMsg(tr("      TIDAL 계정 로그인이 필요합니다.      "));
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : list_data 들을 토대로 Item (AbstractItem 상속받은) 객체들을 생성하여 Layout에 배치한다.
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief AlbumItemData 데이터로 ItemAlbum 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param seciton
     */
    void AbstractTidalSubWidget::createList_itemAlbum_applyingWithData(const QList<tidal::AlbumItemData> &list_data, AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i = 0; i < list_data.size(); i++){
            tidal::ItemAlbum *tmp_widget = new tidal::ItemAlbum(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &tidal::ItemAlbum::signal_clicked, this, &AbstractTidalSubWidget::slot_clickedItemAlbum);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);

        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
            print_debug();
        }
    }


    /**
     * @brief PlaylistItemData 데이터로 ItemPlaylist 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param section
     */
    void AbstractTidalSubWidget::createList_itemPlaylsit_applyingWithData(const QList<tidal::PlaylistItemData> &list_data, AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            tidal::ItemPlaylist *tmp_widget = new tidal::ItemPlaylist(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &tidal::ItemPlaylist::signal_clicked, this, &AbstractTidalSubWidget::slot_clickedItemPlaylist);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);


        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }
    }


    /**
     * @brief ArtistItemData 데이터로 ItemArtist 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param section
     */
    void AbstractTidalSubWidget::createList_itemArtist_applyingWithData(const QList<tidal::ArtistItemData> &list_data, AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            tidal::ItemArtist *tmp_widget = new tidal::ItemArtist(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &tidal::ItemArtist::signal_clicked, this, &AbstractTidalSubWidget::slot_clickedItemArtist);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }

    }


    /**
     * @brief VideoItemData 데이터로 ItemVideo 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param section
     */
    void AbstractTidalSubWidget::createList_itemVideo_applyingWithData(const QList<tidal::VideoItemData> &list_data, AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            tidal::ItemVideo *tmp_widget = new tidal::ItemVideo(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &tidal::ItemVideo::signal_clicked, this, &AbstractTidalSubWidget::slot_clickedItemVideo);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Video_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }
    }




    /**
     * @brief TrackItemData 데이터로 ListWidgetItem 생성하여 ListWidget에 추가한다. 이미지 없음. (slot 연결도 없음)
     * @param list_data
     * @param p_listWidget
     * @param flagAdjustHeight
     */
    void AbstractTidalSubWidget::createList_itemTrackDelegate_applyingWithData(const QList<tidal::TrackItemData> &list_data, QListWidget *p_listWidget, bool flagAdjustHeight){
        for(int i=0 ; i<list_data.size() ; i++){
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(Qt::UserRole, ConvertData::getObjectJson_trackData(list_data.at(i)));
            p_listWidget->addItem(item);
        }

        if(list_data.length() > 0){
            p_listWidget->setVisible(true);
            // listWidget height 조절
            if(flagAdjustHeight){
                p_listWidget->setFixedHeight(70 * list_data.length());         // 고정값 설정
            }
        }
        else{
            p_listWidget->setVisible(false);
        }
    }


    /**
     * @brief TrackItemData 데이터로 ListWidgetItem 생성하여 ListWidget에 추가한다. 이미지 처리가 포함됨. (slot 연결이 있음)
     * @param list_data
     * @param p_listWidget
     * @param flagAdjustHeight 기본값은 false. list_data 개수에 맞춰서 ListWidget의 사이즈를 조정할지의 여부임
     */
    void AbstractTidalSubWidget::createList_itemTrackDelegate_applyingWithData_showImage(const QList<tidal::TrackItemData>& list_data, QListWidget *p_listWidget, const int start_index, const int section, bool flagAdjustHeight){
        // Track 기본 이미지
        QPixmap pixmap_default = this->getDefaultImage_forTrack();

        for(int i=0 ; i<list_data.size() ; i++){
            QListWidgetItem *tmp_item = new QListWidgetItem();
            tmp_item->setData(Qt::UserRole, ConvertData::getObjectJson_trackData(list_data.at(i)));

            QPixmap pixmap;
            QString tmp_imgPath = list_data.at(i).image;
            if(tmp_imgPath.isEmpty()){
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image
            }
            else{
                // download (no-cache)
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image (download 실패 케이스 대응을 위한 기본값 우선 세팅)

                FileDownloader *fileDownloader = new FileDownloader(this);
                connect(fileDownloader, &FileDownloader::downloaded, this, &AbstractTidalSubWidget::slot_thumbnailDownloaded_fotListWidget_track);
                fileDownloader->setProperty("section", section);
                fileDownloader->setProperty("index", i + start_index);
                fileDownloader->setImageURL(tmp_imgPath);
            }

            p_listWidget->addItem(tmp_item);
        }


        if(list_data.length() > 0){
            p_listWidget->setVisible(true);
            // listWidget height 조절
            if(flagAdjustHeight){
                p_listWidget->setFixedHeight(70 * (start_index + list_data.length()));         // 고정값 설정
            }
        }
        else{
            p_listWidget->setVisible(false);
        }
    }



    /**
     * @brief ListWidget과 관련 - Image 다운로드가 완료되면 ListWidget에 보이도록 하기 위한 처리 (DecorationRole 로 설정)
     * @param p_listWidget
     * @param p_list_track
     */
    void AbstractTidalSubWidget::applyImageData_itemTrackDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<tidal::TrackItemData>* p_list_track){
        int tmp_index = fileDownload_sender->property("index").toInt();

        if(tmp_index < p_list_track->count()){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownload_sender->downloadedData());

            if(flagLoaded){
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(IMAGE_SIZE_OF_TRACK, IMAGE_SIZE_OF_TRACK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                p_listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
                QPixmapCache::insert(p_list_track->at(tmp_index).image, tmp_pixmap);
            }
        }

        fileDownload_sender->deleteLater();
    }







    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Item (AbstractItem 상속받은) 객체의 custom click 이벤트에 대하여 처리하는 함수 모음
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // Item 클릭 관련 처리 of Playlist ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemPlsylist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_playlist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractTidalSubWidget::proc_clicked_itemPlaylist(tidal::PlaylistItemData &data_playlist, const AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == AbstractItem::ClickMode::PlayBtn){
                    // Playlist 바로 재생
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byPlaylistUUID(data_playlist, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(clickMode == AbstractItem::ClickMode::AllBox){
                    if(data_playlist.flagCreatedByUser){
                        // Playlist Detail 페이지 진입 - User Created Playlist
                        QJsonObject jsonObj_move = ConvertData::getObjectJson_playlistData(data_playlist);
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_PLAYLIST_DETAIL);
                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                    else{
                        // Playlist Detail 페이지 진입 - General Playlist
                        QJsonObject jsonObj_move = ConvertData::getObjectJson_playlistData(data_playlist);
                        if(data_playlist.description == "mixes"){
                            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MIXES_DETAIL);
                        }
                        else{
                            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_PLAYLIST_DETAIL);
                        }
                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                }
                else if(clickMode == AbstractItem::ClickMode::MoreBtn){
                    // OptionPopup 띄우기 필요
                    if(data_playlist.flagCreatedByUser){
                        // User Created Playlist
                        this->makeObj_optMorePopup(OptMorePopup::Tidal_Playlist_Created, ConvertData::getConvertOptHeaderData(data_playlist), index, section);
                    }
                    else{
                        // Tidal Playlist
                        this->makeObj_optMorePopup(OptMorePopup::Tidal_Playlist, ConvertData::getConvertOptHeaderData(data_playlist), index, section);
                    }
                }



            }
        }
    }


    /**
     * @brief ItemPlsylist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함  [overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_playlist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractTidalSubWidget::proc_clicked_itemPlaylist(QList<tidal::PlaylistItemData>* list_playlist, const AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_playlist->length(), index);
            if(real_index >= 0){
                tidal::PlaylistItemData data = list_playlist->at(real_index);
                this->proc_clicked_itemPlaylist(data, clickMode, real_index, section);
            }
        }
    }



    // Item 클릭 관련 처리 of Album ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemAlbum에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractTidalSubWidget::proc_clicked_itemAlbum(tidal::AlbumItemData& data_album, const AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == AbstractItem::ClickMode::PlayBtn){
                    // Album 바로 재생
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byAlbumID(data_album.id, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(clickMode == AbstractItem::ClickMode::AllBox){
                    // Album Detail 페이지 진입
                    QJsonObject jsonObj_move = ConvertData::getObjectJson_albumData(data_album);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ALBUM_DETAIL);
                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
                else if(clickMode == AbstractItem::ClickMode::MoreBtn){

                    // OptionPopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Tidal_Album, ConvertData::getConvertOptHeaderData(data_album), index, section);
                }
            }
        }
        print_debug();
    }


    /**
     * @brief ItemAlbum에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함  [overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractTidalSubWidget::proc_clicked_itemAlbum(QList<tidal::AlbumItemData>* list_album, const AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_album->length(), index);
            if(real_index >= 0){                
                tidal::AlbumItemData data = list_album->at(real_index);
                this->proc_clicked_itemAlbum(data, clickMode, real_index, section);


            }
        }
        print_debug();
    }



    // Item 클릭 관련 처리 of Artist ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemArtist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_artist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractTidalSubWidget::proc_clicked_itemArtist(tidal::ArtistItemData &data_artist, const AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == AbstractItem::ClickMode::AllBox){
                    // Artist Detail 페이지 진입
                    QJsonObject jsonObj_move = ConvertData::getObjectJson_artistData(data_artist);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ARTIST_DETAIL);
                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
                else if(clickMode == AbstractItem::ClickMode::MoreBtn){
                    // OptionPopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Tidal_Artist, ConvertData::getConvertOptHeaderData(data_artist), index, section);
                }
            }
        }
    }



    /**
     * @brief ItemArtist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함  [overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_artist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractTidalSubWidget::proc_clicked_itemArtist(QList<tidal::ArtistItemData>* list_artist, const AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_artist->length(), index);
            if(real_index >= 0){
                tidal::ArtistItemData data = list_artist->at(real_index);
                this->proc_clicked_itemArtist(data, clickMode, real_index, section);
            }
        }
    }


    // Item 클릭 관련 처리 of Video ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemVideo에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_video
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractTidalSubWidget::proc_clicked_itemVideo(tidal::VideoItemData &data_video, const AbstractItem::ClickMode clickMode, const int index, const int section){

        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == AbstractItem::ClickMode::PlayBtn
                        || clickMode == AbstractItem::ClickMode::AllBox
                ){
                    // Video 바로 재생 - OptMorePopup의 Play_RightNow와 같음
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_video(data_video.jsonObj_toPlay, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(clickMode == AbstractItem::ClickMode::MoreBtn){

                    QJsonObject tmpObj = data_video.jsonObj_toPlay;

                    tidal::VideoItemData tmp_data = ConvertData::make_videoData_fromTidalJsonObj(tmpObj);
                    QString desc = tmp_data.list_artist_name.join(",");


                    // OptMorePopup 띄우기
                    this->makeObj_optMorePopup(OptMorePopup::Tidal_Video, ConvertData::getConvertOptHeaderData(data_video), index, section);
                }
            }
        }
        print_debug();
    }


    void AbstractTidalSubWidget::slot_applyResult_getShareLink(const QString &link){//c220824-1

        this->shareLink = link;
    }


    /**
     * @brief ItemVideo에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함  [overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_artist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractTidalSubWidget::proc_clicked_itemVideo(QList<tidal::VideoItemData>* list_video, const AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_video->length(), index);
            if(real_index >= 0){
                tidal::VideoItemData data = list_video->at(real_index);
                this->proc_clicked_itemVideo(data, clickMode, real_index, section);
            }


        }
        print_debug();

    }



    // Item 클릭 관련 처리 of Track (ListWidget with Delegate) ---------------------------------------------------------------------------------------------
    //      AbstractItem 상속 아님.  - AbstractTidalTrackDelegate 상속 구조 참고

    /**
     * @brief AbstractTidalTrackDelegate 에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_track
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractTidalSubWidget::proc_clicked_itemTrack_inList(tidal::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == AbstractTidalTrackDelegate::ClickMode::AllBox){
                    // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
                    /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
                    int curr_clickMode = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 0:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 1:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 2:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 3:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 4:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 5:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 6:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }*/

                    int curr_clickMode = global.device.getMusicPlayType();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 13:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 12:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 15:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 16:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 17:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 18:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 19:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }

                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byTracks(jsonArr_toPlayAll, index, playType);
                }
                else if(clickMode == AbstractTidalTrackDelegate::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Track);

                    QJsonArray tracks = QJsonArray();
                    tracks.append(jsonArr_toPlayAll.at(index).toObject());

                    QJsonObject json = QJsonObject();
                    json.insert("tracks", tracks);

                    Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                    dialog_playlist->request_playlist_fetch();
                    connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractTidalSubWidget::slot_add_rosePlaylist_withTidal);
                    int result = dialog_playlist->exec();

                    if(result == QDialog::Accepted){
                        delete dialog_playlist;
                    }
                }
                else if(clickMode == AbstractTidalTrackDelegate::ClickMode::MoreBtn){
                    // OptMorePopup 띄우기 필요
                    print_debug();
                        this->makeObj_optMorePopup(OptMorePopup::Tidal_Track, ConvertData::getConvertOptHeaderData(data_track), index, section);
                }
            }
        }
    }


    /**
     * @brief AbstractTidalTrackDelegate 에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함 [overloading]
     * @param list_track
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractTidalSubWidget::proc_clicked_itemTrack_inList(QList<tidal::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                tidal::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_itemTrack_inList(data, jsonArr_toPlayAll, clickMode, real_index, section);
            }
        }
    }


    void AbstractTidalSubWidget::proc_clicked_itemTrack_inList(tidal::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const AlbumTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::AllBox){
                    // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
                    /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
                    int curr_clickMode = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 0:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 1:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 2:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 3:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 4:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 5:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 6:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }*/

                    int curr_clickMode = global.device.getMusicPlayType();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 13:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 12:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 15:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 16:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 17:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 18:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 19:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }

                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byTracks(jsonArray_toPlayAll, index, playType);
                }
                else if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Track);

                    QJsonArray tracks = QJsonArray();
                    tracks.append(jsonArray_toPlayAll.at(index).toObject());

                    QJsonObject json = QJsonObject();
                    json.insert("tracks", tracks);

                    Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::TIDAL, json, this);
                    dialog_playlist->request_playlist_fetch();
                    connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractTidalSubWidget::slot_add_rosePlaylist_withTidal);
                    int result = dialog_playlist->exec();

                    if(result == QDialog::Accepted){
                        delete dialog_playlist;
                    }
                }
                else if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::MoreBtn){



                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Tidal_Track, ConvertData::getConvertOptHeaderData(data_track), index, section);
                }
            }
        }
    }


    void AbstractTidalSubWidget::proc_clicked_itemTrack_inList(QList<tidal::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const AlbumTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                tidal::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_itemTrack_inList(data, jsonArray_toPlayAll, clickMode, real_index, section);
            }
        }
    }


    void AbstractTidalSubWidget::proc_clicked_itemTrack_inList(tidal::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                    // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
                    /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
                    int curr_clickMode = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 0:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 1:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 2:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 3:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 4:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 5:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 6:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }*/

                    int curr_clickMode = global.device.getMusicPlayType();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 13:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 12:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 15:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 16:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 17:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 18:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 19:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }

                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byTracks(jsonArray_toPlayAll, index, playType);
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Track);

                    QJsonArray tracks = QJsonArray();
                    tracks.append(jsonArray_toPlayAll.at(index).toObject());

                    QJsonObject json = QJsonObject();
                    json.insert("tracks", tracks);

                    Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::TIDAL, json, this);
                    dialog_playlist->request_playlist_fetch();
                    connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractTidalSubWidget::slot_add_rosePlaylist_withTidal);
                    int result = dialog_playlist->exec();

                    if(result == QDialog::Accepted){
                        delete dialog_playlist;
                    }
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                    this->makeObj_optMorePopup(OptMorePopup::Tidal_Track, ConvertData::getConvertOptHeaderData(data_track), index, section);
                }

            }
        }
    }


    void AbstractTidalSubWidget::proc_clicked_itemTrack_inList(QList<tidal::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                tidal::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_itemTrack_inList(data, jsonArray_toPlayAll, clickMode, real_index, section);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : OptMorePopup 객체의 signal 대하여, Data Type 별로 처리하는 함수 모음
    //
    //-----------------------------------------------------------------------------------------------------------------------


    // OptMorePopup 관련 처리 of Album ---------------------------------------------------------------------------------------------

    /**
     * @brief AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractTidalSubWidget::proc_clicked_optMorePopup_fromAlbum(tidal::AlbumItemData& data_album, const OptMorePopup::ClickMode clickMode){


        if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
        )
        {
            // Rose Play 요청
            ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
            if(data_album.visualAlbum == true && data_album.isVideo == true){
                procRosePlay->requestPlayRose_byVisualAlbumID(data_album.id, data_album.numberOfTracks, clickMode);
            }
            else{
                procRosePlay->requestPlayRose_byAlbumID(data_album.id, clickMode);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
            // 플레이리스트 담기 - Album
            //this->showDialog_toAddMyCollection(data_album.id, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Album);

            /*QJsonObject json;
            Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
            dialog_playlist->request_playlist_fetch();
            dialog_playlist->exec();*/
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - 앨범
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_addFavorite_album(data_album.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 추가 - 삭제
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_deleteFavorite_album(data_album.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist DetailView
            //this->movePage_artist_detailView(data_album.list_artist_id, data_album.list_artist_name);

            // Artist DetailView
            if(data_album.list_artist_id.count() > 1){
                QJsonObject tmp_artist = ConvertData::getObjectJson_albumData(data_album);
                tmp_artist.insert("manufacture", "tidal");

                this->dlgArtist = new DialogArtistList(this);
                this->dlgArtist->setData(tmp_artist);
                this->dlgArtist->exec();
            }
            else{
                if(global.user_forTidal.dlg_isShow() == true){
                    global.user_forTidal.dlg_set_state(false);
                }

                if(global.user_forTidal.flag_rosehome == true){
                    global.user_forTidal.rosehome_obj = QJsonObject();
                    global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_ARTIST_DETAIL);
                    tidal::ArtistItemData tmp_data_artist;
                    tmp_data_artist.id = data_album.list_artist_id.first();
                    QJsonObject tmp_data = ConvertData::getObjectJson_artistData(tmp_data_artist);
                    global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

                    emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Tidal));
                }
                else{
                    this->movePage_artist_detailView(data_album.list_artist_id, data_album.list_artist_name);
                }
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220823
            print_debug();
            setUIShare();
            qDebug() << "this->shareLink="<<this->shareLink;
        }
    }



    /**
     * @brief AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractTidalSubWidget::proc_clicked_optMorePopup_fromAlbum(QList<tidal::AlbumItemData>* list_album, const int index, const OptMorePopup::ClickMode clickMode){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_album->length(), index);
            if(real_index >= 0){
                tidal::AlbumItemData data = list_album->at(real_index);
                this->proc_clicked_optMorePopup_fromAlbum(data, clickMode);
            }
        }

    }


    // OptMorePopup 관련 처리 of Video ---------------------------------------------------------------------------------------------

    /**
     * @brief AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractTidalSubWidget::proc_clicked_optMorePopup_fromVideo(tidal::VideoItemData& data_video, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
        )
        {
            // Rose Play 요청
            ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
            procRosePlay->requestPlayRose_video(data_video.jsonObj_toPlay, clickMode);
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - Video
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_addFavorite_video(data_video.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 삭제 - Video
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_deleteFavorite_video(data_video.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220823
            print_debug();
            setUIShare();
            qDebug() << "this->shareLink="<<this->shareLink;
        }
    }



    /**
     * @brief AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractTidalSubWidget::proc_clicked_optMorePopup_fromVideo(QList<tidal::VideoItemData>* list_video, const int index, const OptMorePopup::ClickMode clickMode){

        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_video->length(), index);
            if(real_index >= 0){
                tidal::VideoItemData data = list_video->at(real_index);
                this->proc_clicked_optMorePopup_fromVideo(data, clickMode);
            }
        }

    }



    // OptMorePopup 관련 처리 of Playlist ---------------------------------------------------------------------------------------------

    /**
     * @brief PlaylistItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractTidalSubWidget::proc_clicked_optMorePopup_fromPlaylist(tidal::PlaylistItemData& data_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
        )
        {
            // Rose Play 요청
            ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
            procRosePlay->requestPlayRose_byPlaylistUUID(data_playlist, clickMode);
        }

        else if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - Playlist
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_addFavorite_playlist(data_playlist.uuid);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 삭제 - Playlist
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_deleteFavorite_playlist(data_playlist.uuid);
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
            // 플레이리스트 담기 - Playlist
            //this->showDialog_toAddMyCollection(data_playlist.uuid, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Playlist);

            /*QJsonObject json;
            Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
            dialog_playlist->request_playlist_fetch();
            dialog_playlist->exec();*/
        }
        else if(clickMode == OptMorePopup::ClickMode::Edit){
            // 사용자가 만든 playlist에 한하여 동작하는 것임.  (OptMorePopup 생성을 구분해서 하고 있음)
            this->movePage_playlist_editOfMine(data_playlist);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete){
            // 사용자가 만든 playlist에 한하여 동작하는 것임.  (OptMorePopup 생성을 구분해서 하고 있음)

            // update UI
            ItemPositionData itemPosData;
            itemPosData.section = section;
            itemPosData.index = index;
            itemPosData.data_id = data_playlist.uuid;
            this->proc_preUpdate_delete_myCreatePlaylist(itemPosData);

            // request Tidal PI
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_delete_myCreatedPlaylist(data_playlist.uuid, QJsonObject());
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220823
            print_debug();
            setUIShare();
            qDebug() << "this->shareLink="<<this->shareLink;
        }
    }



    /**
     * @brief PlaylistItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractTidalSubWidget::proc_clicked_optMorePopup_fromPlaylist(QList<tidal::PlaylistItemData>* list_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode){

        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_playlist->length(), index);
            if(real_index >= 0){
                tidal::PlaylistItemData data = list_playlist->at(real_index);
                this->proc_clicked_optMorePopup_fromPlaylist(data, real_index, section, clickMode);
            }
        }

    }


    // OptMorePopup 관련 처리 of Track ---------------------------------------------------------------------------------------------

    /**
     *

     * @brief TrackItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractTidalSubWidget::proc_clicked_optMorePopup_fromTrack(tidal::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last

        )
        {
            // Rose Play 요청
            //ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
            //procRosePlay->requestPlayRose_byTracks(jsonArr_toPlayAll, index, clickMode);

            if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
            {
                QJsonArray tmpJsonArr = QJsonArray();
                for(int i = index; i < jsonArr_toPlayAll.size(); i++){
                    QJsonObject tmpJsonObj = jsonArr_toPlayAll.at(i).toObject();
                    tmpJsonArr.append(tmpJsonObj);
                }

                // Rose Play 요청
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(tmpJsonArr, 0, clickMode);
            }
            else{
                QJsonObject tmpJsonObj = jsonArr_toPlayAll.at(index).toObject();
                QJsonArray tmpJsonArr = QJsonArray();
                tmpJsonArr.append(tmpJsonObj);

                // Rose Play 요청
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(tmpJsonArr, 0, clickMode);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - Track
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_addFavorite_track(data_track.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 삭제 - Track
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_deleteFavorite_track(data_track.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
            // 플레이리스트 담기 - Track
            //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Track);

            QJsonArray tracks = QJsonArray();
            //tracks.append(jsonArr_toPlayAll.at(index).toObject());
            tracks = jsonArr_toPlayAll;

            QJsonObject json = QJsonObject();
            json.insert("tracks", tracks);

            Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
            dialog_playlist->request_playlist_fetch();
            connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractTidalSubWidget::slot_add_rosePlaylist_withTidal);
            int result = dialog_playlist->exec();

            if(result == QDialog::Accepted){
                delete dialog_playlist;
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist DetailView
            //this->movePage_artist_detailView(data_track.list_artist_id, data_track.list_artist_name);

            // Artist DetailView
            if(data_track.list_artist_id.count() > 1){
                QJsonObject tmp_artist = ConvertData::getObjectJson_trackData(data_track);
                tmp_artist.insert("manufacture", "tidal");

                this->dlgArtist = new DialogArtistList(this);
                this->dlgArtist->setData(tmp_artist);
                this->dlgArtist->exec();
            }
            else{
                if(global.user_forTidal.dlg_isShow() == true){
                    global.user_forTidal.dlg_set_state(false);
                }

                if(global.user_forTidal.flag_rosehome == true){
                    global.user_forTidal.rosehome_obj = QJsonObject();
                    global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_ARTIST_DETAIL);
                    tidal::ArtistItemData tmp_data_artist;
                    tmp_data_artist.id = data_track.list_artist_id.first();
                    QJsonObject tmp_data = ConvertData::getObjectJson_artistData(tmp_data_artist);
                    global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

                    emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Tidal));
                }
                else{
                    this->movePage_artist_detailView(data_track.list_artist_id, data_track.list_artist_name);
                }
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Album){
            // Album DetailView
            if(global.user_forTidal.flag_rosehome == true){
                global.user_forTidal.rosehome_obj = QJsonObject();
                global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_ALBUM_DETAIL);
                tidal::ArtistItemData tmp_data_album;
                tmp_data_album.id = data_track.album_id;
                QJsonObject tmp_data = ConvertData::getObjectJson_artistData(tmp_data_album);
                global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Tidal));
            }
            else{
                this->movePage_album_detailView(data_track.album_id);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220824_4
            print_debug();
            setUIShare();
            qDebug() << "this->shareLink="<<this->shareLink;

        }
    }



    /**
     * @brief TrackItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractTidalSubWidget::proc_clicked_optMorePopup_fromTrack(QList<tidal::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                tidal::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_optMorePopup_fromTrack(data, jsonArr_toPlayAll, real_index, clickMode);
            }
        }
    }




    // OptMorePopup 관련 처리 of Artist ---------------------------------------------------------------------------------------------

    /**
     * @brief ArtistItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractTidalSubWidget::proc_clicked_optMorePopup_fromArtist(tidal::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - Artist
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_addFavorite_artist(data_artist.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 삭제 - Artist
            ProcCommon *proc = new ProcCommon(this);
            proc->request_tidal_deleteFavorite_artist(data_artist.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){

            //220907 share code ... --
            setUIShare();//c220816
        }

    }



    /**
     * @brief ArtistItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractTidalSubWidget::proc_clicked_optMorePopup_fromArtist(QList<tidal::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode){
        if(this->is_tidal_logined() == false){
            this->showNeededLoginTidal();               // TIDAL 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_artist->length(), index);
            if(real_index >= 0){
                tidal::ArtistItemData data = list_artist->at(real_index);
                this->proc_clicked_optMorePopup_fromArtist(data, clickMode);
            }
        }
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 자식 클래스에서 DetailInfo 클릭에 대한 상세처리하기 위한 함수들
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief AbstractTidalSubWidget::proc_clicked_imageDetailInfo_fromAlbum
     * @param data_album - 현재는 id만 사용하지만, 차후 확장 고려위해 AlbumItemData 을 받도록함
     * @param btnClickMode
     */
    void AbstractTidalSubWidget::proc_clicked_imageDetailInfo_fromAlbum(tidal::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_album.id > 0){
            if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - Album
                //this->showDialog_toAddMyCollection(data_album.id, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Album);

                QJsonArray tracks = QJsonArray();
                //tracks.append(jsonArr_toPlayAll.at(index).toObject());
                tracks = p_jsonArr_toPlay;

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractTidalSubWidget::slot_add_rosePlaylist_withTidal);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Album
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_addFavorite_album(data_album.id);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Album
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_deleteFavorite_album(data_album.id);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::More){
                print_debug();
                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                print_debug();
                        this->makeObj_optMorePopup(OptMorePopup::Tidal_Album, ConvertData::getConvertOptHeaderData(data_album), 0, section, true);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
                // 전체재생
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    void AbstractTidalSubWidget::proc_clicked_imageDetailInfo_fromAlbum(tidal::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_album.id > 0){
            if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - Album
                //this->showDialog_toAddMyCollection(data_album.id, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Album);

                QJsonArray tracks = QJsonArray();
                //tracks.append(jsonArr_toPlayAll.at(index).toObject());
                tracks = p_jsonArr_toPlay;

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::TIDAL, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractTidalSubWidget::slot_add_rosePlaylist_withTidal);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Album
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_addFavorite_album(data_album.id);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Album
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_deleteFavorite_album(data_album.id);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){
                print_debug();
                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                print_debug();
                        this->makeObj_optMorePopup(OptMorePopup::Tidal_Album, ConvertData::getConvertOptHeaderData(data_album), 0, section, true);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){
                // 전체재생
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    /**
     * @brief AbstractTidalSubWidget::proc_clicked_imageDetailInfo_fromArtist
     * @param data_artist
     * @param section
     * @param btnClickMode
     */
    void AbstractTidalSubWidget::proc_clicked_imageDetailInfo_fromArtist(tidal::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_artist.id > 0){
            if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Artiest
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_addFavorite_artist(data_artist.id);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Artiest
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_deleteFavorite_artist(data_artist.id);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::More){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Tidal_Artist, ConvertData::getConvertOptHeaderData(data_artist), 0, section);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
                // 전체재생
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    void AbstractTidalSubWidget::proc_clicked_imageDetailInfo_fromArtist(tidal::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode){

        // 데이터 유효할 때에만
        if(data_artist.id > 0){
            if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Artiest
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_addFavorite_artist(data_artist.id);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Artiest
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_deleteFavorite_artist(data_artist.id);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Tidal_Artist, ConvertData::getConvertOptHeaderData(data_artist), 0, section);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){
                // 전체재생
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    /**
     * @brief AbstractTidalSubWidget::proc_clicked_imageDetailInfo_fromPlaylist
     * @param data_playlist
     * @param section
     * @param btnClickMode
     */
    void AbstractTidalSubWidget::proc_clicked_imageDetailInfo_fromPlaylist(tidal::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode){

        if(data_playlist.uuid.isEmpty() == false){
            if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Playlist
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_addFavorite_playlist(data_playlist.uuid);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Playlist
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_deleteFavorite_playlist(data_playlist.uuid);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::More){
                print_debug();
                // OptMorePopup 띄우기
                if(data_playlist.flagCreatedByUser){
                    print_debug();
                        this->makeObj_optMorePopup(OptMorePopup::Tidal_Playlist_Created, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
                }
                else{
                    print_debug();
                        this->makeObj_optMorePopup(OptMorePopup::Tidal_Playlist, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
                }
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::ShufflePlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - playlist용
                //this->showDialog_toAddMyCollection(data_playlist.uuid, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Playlist);

                QJsonArray tracks = QJsonArray();
                //tracks.append(jsonArr_toPlayAll.at(index).toObject());
                tracks = p_jsonArr_toPlay;

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractTidalSubWidget::slot_add_rosePlaylist_withTidal);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
        }
    }


    void AbstractTidalSubWidget::proc_clicked_imageDetailInfo_fromPlaylist(tidal::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode){
        if(data_playlist.uuid.isEmpty() == false){
            if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Playlist
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_addFavorite_playlist(data_playlist.uuid);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Playlist
                ProcCommon *proc = new ProcCommon(this);
                proc->request_tidal_deleteFavorite_playlist(data_playlist.uuid);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){
                print_debug();
                if(data_playlist.flagCreatedByUser){
                    print_debug();
                        this->makeObj_optMorePopup(OptMorePopup::Tidal_Playlist_Created, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
                }
                else{
                    print_debug();
                        this->makeObj_optMorePopup(OptMorePopup::Tidal_Playlist, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
                }
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withTidal::PlayShuffleMode::ShufflePlay);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - playlist용
                //this->showDialog_toAddMyCollection(data_playlist.uuid, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection::Playlist);

                QJsonArray tracks = QJsonArray();
                //tracks.append(jsonArr_toPlayAll.at(index).toObject());
                tracks = p_jsonArr_toPlay;

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::TIDAL, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractTidalSubWidget::slot_add_rosePlaylist_withTidal);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 자식 클래스에서 페이지 이동의 간편화. SubPage에 대해, PageInfo struct로 페이지 이동을 처리하기 위함.
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief Video All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractTidalSubWidget::movePage_video_allView(tidal::PageInfo_VideoAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_videoAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_VIDEO_ALL_LIST_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
    }

    /**
     * @brief Track All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractTidalSubWidget::movePage_track_allView(tidal::PageInfo_TrackAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_trackAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_TRACK_ALL_LIST_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
    }

    /**
     * @brief Album All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractTidalSubWidget::movePage_album_allView(tidal::PageInfo_AlbumAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ALBUM_ALL_LIST_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
    }


    /**
     * @brief Playlist All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractTidalSubWidget::movePage_playlist_allView(tidal::PageInfo_PlaylistAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_playlistAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_PLAYLIST_ALL_LIST_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);
    }


    /**
     * @brief My Playlist 편집 페이지 이동
     * @param data_playlist
     */
    void AbstractTidalSubWidget::movePage_playlist_editOfMine(tidal::PlaylistItemData& data_playlist){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_playlistData(data_playlist);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_MY_COLLECTION_PLAYLIST_EDIT);
        emit this->signal_clickedMovePage(jsonObj_move);
    }


    /**
     * @brief AbstractTidalSubWidget::movePage_artist_detailView
     * @param artist_id
     */
    void AbstractTidalSubWidget::movePage_artist_detailView(const QList<int>& list_artist_id, const QList<QString>& list_artist_name){
        Q_UNUSED(list_artist_name);

        if(list_artist_id.length() > 0){
            // 바로 Artist Detail 페이지 진입 (id 만 넣어서 진입)
            tidal::ArtistItemData tmp_data_artist;
            tmp_data_artist.id = list_artist_id.first();

            QJsonObject jsonObj_move = ConvertData::getObjectJson_artistData(tmp_data_artist);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ARTIST_DETAIL);
            emit this->signal_clickedMovePage(jsonObj_move);                    // 페이지 이동 signal
        }
    }


    /**
     * @brief Album ID로, Album Detail View 페이지 이동을 위한 처리
     * @param album_id
     */
    void AbstractTidalSubWidget::movePage_album_detailView(const int album_id){
        if(album_id > 0){
            // Album Detail 페이지 진입 (id 만 넣어서 진입)
            tidal::AlbumItemData tmp_data_album;
            tmp_data_album.id = album_id;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_albumData(tmp_data_album);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ALBUM_DETAIL);
            emit this->signal_clickedMovePage(jsonObj_move);                    // 페이지 이동 signal
        }
    }


    void AbstractTidalSubWidget::slot_dlg_signal_artist(const QJsonObject &p_jsonObject){

        QList<int> list_artist_id;
        QList<QString> list_artist_name;

        list_artist_id.append(ProcJsonEasy::getInt(p_jsonObject, "artist_id"));

        this->movePage_artist_detailView(list_artist_id, list_artist_name);
        global.user_forTidal.dlg_set_state(true);
    }








    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 자식 클래스에서 사용자의 플레이리스트 담기 관련
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 플레이리스트 담기 다이얼로그 띄우기
     * @param content_id
     * @param content_type
     */
    void AbstractTidalSubWidget::showDialog_toAddMyCollection(QVariant content_id, Dialog_ChoosePlaylist_forTidal::ItemType_forAddCollection content_type){
        // 플레이리스트 담기 다이얼로그 띄우기 - 선택한 옵션 함수는 하나이지만, slot은 의도적으로 구분함
        Dialog_ChoosePlaylist_forTidal *dialog_chooslPlaylist = new Dialog_ChoosePlaylist_forTidal(this);
        dialog_chooslPlaylist->set_contentData(content_id, content_type);
        connect(dialog_chooslPlaylist, &Dialog_ChoosePlaylist_forTidal::signal_choosePlaylist_forTrack, this, &AbstractTidalSubWidget::slot_add_choosePlaylist_forTrack);
        connect(dialog_chooslPlaylist, &Dialog_ChoosePlaylist_forTidal::signal_choosePlaylist_forAlbum, this, &AbstractTidalSubWidget::slot_add_choosePlaylist_forAlbum);
        connect(dialog_chooslPlaylist, &Dialog_ChoosePlaylist_forTidal::signal_choosePlaylist_forPlaylist, this, &AbstractTidalSubWidget::slot_add_choosePlaylist_forPlaylist);
        dialog_chooslPlaylist->exec();
    }

    /**
     * @brief 선택한 Track 에 대해서, 플레이리스트 담기 처리 진행
     * @param playlist_uuid_target - 선택한 플레이리스트
     * @param track_id
     */
    void AbstractTidalSubWidget::slot_add_choosePlaylist_forTrack(QString playlist_uuid_target, int track_id){
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::completeReq_add_items_in_myPlaylist, this, &AbstractTidalSubWidget::slot_done_addPlaylist);
        proc->request_tidal_add_track_in_myPlaylist(playlist_uuid_target, track_id);
    }

    /**
     * @brief 선택한 Album 에 대해서, 플레이리스트 담기 처리 진행
     * @param playlist_uuid_target - 선택한 플레이리스트
     * @param album_id
     */
    void AbstractTidalSubWidget::slot_add_choosePlaylist_forAlbum(QString playlist_uuid_target, int album_id){
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::completeReq_add_items_in_myPlaylist, this, &AbstractTidalSubWidget::slot_done_addPlaylist);
        proc->request_tidal_add_album_in_myPlaylist(playlist_uuid_target, album_id);
    }


    void AbstractTidalSubWidget::slot_add_rosePlaylist_withTidal(const int &idx, const QJsonObject &dataObj){

        QString view_type = "";
        if(idx < 0){
            view_type = "create";
        }
        else{
            view_type = "add";
        }

        if(global.user_forTidal.flag_rosehome == false){

            QJsonObject jsonObj_move = dataObj;
            jsonObj_move.insert("view_type", view_type);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_ADD_PLAYLIST);

            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else{

            QJsonObject data;
            data.insert("tracks", ProcJsonEasy::getJsonArray(dataObj, "tracks"));
            data.insert("view_type", view_type);
            data.insert("type", "TIDAL");

            QJsonObject jsonObj_move;
            jsonObj_move.insert("data", data);

            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_ADD_PLAYLIST);

            emit linker->signal_clicked_movePage(jsonObj_move);
        }
    }


    /**
     * @brief 선택한 Playlist 에 대해서, 플레이리스트 담기 처리 진행
     * @param playlist_uuid_target - 선택한 플레이리스트
     * @param playlist_uuid
     */
    void AbstractTidalSubWidget::slot_add_choosePlaylist_forPlaylist(QString playlist_uuid_target, QString playlist_uuid){
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::completeReq_add_items_in_myPlaylist, this, &AbstractTidalSubWidget::slot_done_addPlaylist);
        proc->request_tidal_add_playlist_in_myPlaylist(playlist_uuid_target, playlist_uuid);
    }


    /**
     * @brief 플레이리스트 담기 처리 완료
     */
    void AbstractTidalSubWidget::slot_done_addPlaylist(){
       this->showToastMsg(tr("Added to playlist."));
       // this->showToastMsg(tr("플레이리스트에 추가되었습니다."));
    }






    // MARK : about Filter Option Box -----------------------------------------------------------------------------------------------------

    /**
     * @brief My Collection 에서 사용하는 Filter Option Data 목록 반환함
     * @return
     */
    QList<bugs::FilterOptionData> AbstractTidalSubWidget::get_list_filterOptData_forMyCollection(){

        QList<bugs::FilterOptionData> list_output;

        bugs::FilterOptionData filterOpt_date;
        filterOpt_date.opt_code = QVariant::fromValue(ProcCommon::DataOrderOption::DATE);
        filterOpt_date.opt_name = tr("Date added");
        list_output.append(filterOpt_date);

        bugs::FilterOptionData filterOpt_name;
        filterOpt_name.opt_code = QVariant::fromValue(ProcCommon::DataOrderOption::NAME);
        filterOpt_name.opt_name = tr("A-Z");
        list_output.append(filterOpt_name);

        return list_output;

    }




    // MARK : Rose 요청 실패에 대한 공용 처리 -----------------------------------------------------------------------------------

    /**
     * @brief errorMsg를 Toast로 띄움
     * @param errorMsg
     */
    void AbstractTidalSubWidget::slot_cannotRosePlay(const QString errorMsg){
        this->showToastMsg(errorMsg);
    }

}
