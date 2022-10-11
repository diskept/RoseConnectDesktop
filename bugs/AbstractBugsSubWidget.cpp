#include "bugs/AbstractBugsSubWidget.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/DataCommon_Bugs.h"
#include "bugs/ItemAlbum_bugs.h"
#include "bugs/ItemTrackSqure_bugs.h"
#include "bugs/ItemVideo_bugs.h"
#include "bugs/ItemPD_Album_bugs.h"
#include "bugs/ItemMyAlbum_bugs.h"
#include "bugs/ItemArtist_bugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "roseHome/ProcCommon_forRosehome.h"//c220825_1

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"

#include "login/DialogLogin_forBugs.h"

#include "widget/toastmsg.h"
#include "widget/NoData_Widget.h"

#include <QPixmapCache>
#include <QSettings>


namespace bugs {


    const QString ICON_PATH___playAll = ":/images/icon_list_allplay.png";
    const QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";



    /**
     * @brief Bugs 서브페이지 위젯을 위한 부모 클래스
     * @param parent
     */
    AbstractBugsSubWidget::AbstractBugsSubWidget(MainUIType p_mainUIType, QWidget *parent) : common::AbstractCommonSubWidget(p_mainUIType, parent) {

        // to check memory address number
        linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_Video_movePage(QJsonObject)), SLOT(slot_dlg_signal_video(QJsonObject)));
        connect(linker, SIGNAL(signal_Artist_movePage(QJsonObject)), SLOT(slot_dlg_signal_artist(QJsonObject)));

        // 기본은 not login 상태의 UI 보이도록 한다.
        this->m_flagCurrLogined = false;
        this->setUIControl_notLogin();
    }

    /**
     * @brief 소멸자
     */
    AbstractBugsSubWidget::~AbstractBugsSubWidget(){

        // to check memory address number
        this->deleteLater();
    }


    /**
     * @brief Bugs의 페이지들의 공통처리
     */
    void AbstractBugsSubWidget::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractCommonSubWidget::setActivePage();

        // Login 상태변경에 대한 처리와 동일.
        this->applyLoginSuccessState();
    }


    /**
     * @brief 사용자가 Bugs 로그인하지 않았을 경우의 화면을 위한 Widget
     */
    void AbstractBugsSubWidget::setUIControl_notLogin(){
        // 로그인 하지 않았을 때의 화면
        QLabel *label_topComment = new QLabel("로그인하시면 다양한 서비스를 이용하실 수 있습니다.");
        label_topComment->setStyleSheet("font-size: 18px; color: #ffffff;");
        label_topComment->setContentsMargins(0,0,0,0);
        label_topComment->setAlignment(Qt::AlignCenter);
        QPushButton *btn_login = new QPushButton();
        btn_login->setText("로그인");
        btn_login->setFixedHeight(40);
        btn_login->setStyleSheet("font-size:18px; color:#FFFFFF; background-color:#B18658; border-radius:20px; padding-left:44px; padding-right:44px;");
        btn_login->setCursor(Qt::PointingHandCursor);
        btn_login->setContentsMargins(0,0,0,0);
        connect(btn_login, &QPushButton::clicked, this, &AbstractBugsSubWidget::slot_clickBtnLogin_toShowDialog);

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
     * @brief 로그인 되었을 때의화면
     */
    void AbstractBugsSubWidget::setUIControl_logined(){

        // 전체 Widget에 넣기
        QWidget *tmp_widget = new QWidget(this);
        tmp_widget->setContentsMargins(0,0,0,0);
        tmp_widget->setMaximumWidth(1200);

        this->box_contents->addWidget(tmp_widget);

    }





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 전체재생, 셔플재생 버튼 Widget 관련
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 플레이 관련 버튼 구성 (전체재생, 셔플재생)
     */
    QWidget* AbstractBugsSubWidget::get_UIControl_btnPlays(const bool flagShowPlay, const bool flagShowShuffle){

        // layout & widget
        QHBoxLayout *hBox_btnPlays = new QHBoxLayout;
        hBox_btnPlays->setAlignment(Qt::AlignLeft);
        hBox_btnPlays->setSpacing(0);
        hBox_btnPlays->setContentsMargins(0,0,0,0);

        QWidget* widget_output = new QWidget();
        widget_output->setContentsMargins(0,0,0,0);
        widget_output->setLayout(hBox_btnPlays);
        widget_output->setFixedHeight(50);

        if(flagShowPlay){
            //  전체재생
            QPixmap pixmap_play(ICON_PATH___playAll);
            QPushButton *btn_playAll = new QPushButton;
            btn_playAll->setIcon(pixmap_play);
            btn_playAll->setIconSize(pixmap_play.rect().size());
            btn_playAll->setText("전체재생");
            btn_playAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_playAll->setCursor(Qt::PointingHandCursor);
            connect(btn_playAll, &QPushButton::clicked, this, &AbstractBugsSubWidget::slot_btnClicked_playAll);

            hBox_btnPlays->addWidget(btn_playAll, 0, Qt::AlignVCenter);
            hBox_btnPlays->addSpacing(30);

        }

        if(flagShowShuffle){
            // 셔플재생
            QPixmap pixmap_shuffle(ICON_PATH___playShuffle);
            QPushButton *btn_shuffleAll = new QPushButton;
            btn_shuffleAll->setIcon(pixmap_shuffle);
            btn_shuffleAll->setIconSize(pixmap_shuffle.rect().size());
            btn_shuffleAll->setText("셔플재생");
            btn_shuffleAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_shuffleAll->setCursor(Qt::PointingHandCursor);
            connect(btn_shuffleAll, &QPushButton::clicked, this, &AbstractBugsSubWidget::slot_btnClicked_playShuffle);

            hBox_btnPlays->addWidget(btn_shuffleAll, 0, Qt::AlignVCenter);
            hBox_btnPlays->addSpacing(30);
        }

        return widget_output;
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Slot 함수
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief "로그아웃" 버튼 클릭에 대한 처리
     */
    void AbstractBugsSubWidget::slot_clickBtnLogout(){
        // 로그아웃 처리 (PC 상태)
        ProcBugsAPI *proc = new ProcBugsAPI(this);
        proc->request_logout();

        // Rose 기기에 Bugs 로그아웃 처리
        ProcRoseAPI_withBugs *procRose = new ProcRoseAPI_withBugs(this);
        bugs::RoseSessionInfo_forBugs sessionInfo_init;
        procRose->request_set_session_info(sessionInfo_init);

        // 상태 변경
        this->applyLoginSuccessState();
    }

    /**
     * @brief "로그인" 버튼 클릭 처리 - 로그인 다이얼로그 띄우기
     */
    void AbstractBugsSubWidget::slot_clickBtnLogin_toShowDialog(){
        // Bugs 로그인 다이얼로그
        DialogLogin_forBugs *dlg_login = new DialogLogin_forBugs(this);
        connect(dlg_login, &DialogLogin_forBugs::signal_successLogin, this, &AbstractBugsSubWidget::slot_dialogLogin_acceptedAuthCode);
        dlg_login->exec();
    }


    /**
     * @brief [Slot] 벅스 로그인 Dialog에서 authCode 정상적으로 발급된 경우의 처리
     */
    void AbstractBugsSubWidget::slot_dialogLogin_acceptedAuthCode(){
        // token 요청
        ProcBugsAPI *proc = new ProcBugsAPI(this);
        connect(proc, &ProcBugsAPI::signal_completeReq_getToken, this, &AbstractBugsSubWidget::slot_getToken);
        proc->request_bugs_getTokenAndSave();
    }


    /**
     * @brief AbstractBugsSubWidget::slot_success_getToken
     * @param p_bugsTokenInfo
     */
    void AbstractBugsSubWidget::slot_getToken(const bool flagSuccess, const QString errorMsg){
        if(flagSuccess){
            ToastMsg::show(this, "", "벅스 서비스 로그인에 성공했습니다.");

            // 토큰 정보를 global에 저장함
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            bugs::BugsTokenInfo tokenInfo = proc->getTokenInfo_bugsDB();
            global.user_forBugs.set_loginState(tokenInfo.access_token, tokenInfo.refresh_token);

            // 전체 장르 정보를 요청함 - 공통으로 사용 (세팅 안된 경우)
            DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
            if(dataCommon_bugs.list_all.size() == 0){
                proc->request_bugs_getListGenres();
            }

            // 사용자 정보를 요청함
            connect(proc, &ProcBugsAPI::signal_completeReq_getMemberInfo, this, &AbstractBugsSubWidget::slot_completeReq_getMemberInfo);
            proc->request_bugs_getMemberInfo();

        }
        else{
            // error
            this->showToastMsg(errorMsg);
        }
    }

    /**
     * @brief AbstractBugsSubWidget::slot_completeReq_getMemberInfo
     * @param errorMsg
     * @param jsonObj_member
     * @param jsonObj_product
     */
    void AbstractBugsSubWidget::slot_completeReq_getMemberInfo(const QString errorMsg, const QJsonObject& jsonObj_member, const QJsonObject& jsonObj_product){
        if(!errorMsg.isEmpty()){
            this->showToastMsg(errorMsg);
        }
        else{
            // get token info
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            bugs::BugsTokenInfo tokenInfo = proc->getTokenInfo_bugsDB();


            // Rose 에게 bugs session 정보를 전달함
            bugs::RoseSessionInfo_forBugs sessionInfo;

            sessionInfo.BUGS_TokenType = tokenInfo.token_type;
            sessionInfo.BUGS_Scope = tokenInfo.scope;
            sessionInfo.BUGS_ExpiresIn = QString("%1").arg(tokenInfo.expires_in);
            sessionInfo.BUGS_RefreshToken = tokenInfo.refresh_token;
            sessionInfo.BUGS_AccessToken = tokenInfo.access_token;

            sessionInfo.BUGS_Nickname = ProcJsonEasy::getString(jsonObj_member, "nickname");
            sessionInfo.BUGS_AdultYn = ProcJsonEasy::getBool(jsonObj_member, "adult_yn");

            sessionInfo.BUGS_ProductName = ProcJsonEasy::getString(jsonObj_product, "product_name");
            sessionInfo.BUGS_ProductDisplayEndDt = ProcJsonEasy::getString(jsonObj_product, "product_display_end_dt");

            QJsonObject jsonObj_right = ProcJsonEasy::getJsonObject(jsonObj_product, "rights");
            sessionInfo.BUGS_StreamingYn = ProcJsonEasy::getBool(jsonObj_right, "streaming_yn");
            sessionInfo.BUGS_FlacPremiumYn = ProcJsonEasy::getBool(jsonObj_right, "flac_premium_yn");

            ProcRoseAPI_withBugs *procRose = new ProcRoseAPI_withBugs(this);
            procRose->request_set_session_info(sessionInfo);


            // 사용자의 정보를 저장한다.
            global.user_forBugs.setNickName(sessionInfo.BUGS_Nickname);
            global.user_forBugs.setProductName(sessionInfo.BUGS_ProductName);


            // 로그인 상태를 반영하여 페이지 변경함
            this->applyLoginSuccessState();
        }
    }

    /**
     * @brief Bugs 로그인 여부 반환
     * @return
     */
    bool AbstractBugsSubWidget::is_bugs_logined(){
        return global.user_forBugs.isLogined();
    }


    /**
     * @brief 화면에 로그인이 필요하다고 알려줌
     */
    void AbstractBugsSubWidget::showNeededLoginBugs(){
        this->showToastMsg("      Bugs 계정 로그인이 필요합니다.      ");
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : list_data 들을 토대로 Item (AbstractItem 상속받은) 객체들을 생성하여 Layout에 배치한다.
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief AbstractBugsSubWidget::createList_itemTrackSqure_applyingWithData
     * @param list_data
     * @param p_layout
     * @param section
     */
    void AbstractBugsSubWidget::createList_itemTrackSqure_applyingWithData(const QList<bugs::TrackItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int section){
        for(int i=0 ; i<list_data.size() ; i++){

            bugs::ItemTrackSqure_bugs *tmp_widget = new bugs::ItemTrackSqure_bugs(i, section, imageSizeMode);
            connect(tmp_widget, &bugs::ItemTrackSqure_bugs::signal_clicked, this, &AbstractBugsSubWidget::slot_clickedItemTrackSqure);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);

        }

        if( (list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }
    }


    /**
     * @brief AbstractBugsSubWidget::createList_itemAlbum_applyingWithData
     * @param list_data
     * @param imageSizeMode
     * @param p_layout
     * @param section
     */
    void AbstractBugsSubWidget::createList_itemAlbum_applyingWithData(const QList<bugs::AlbumItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            bugs::ItemAlbum_bugs *tmp_widget = new bugs::ItemAlbum_bugs(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &bugs::ItemAlbum_bugs::signal_clicked, this, &AbstractBugsSubWidget::slot_clickedItemAlbum);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }
    }


    /**
     * @brief AbstractBugsSubWidget::createList_itemPD_Album_applyingWithData
     * @param list_data
     * @param imageSizeMode
     * @param p_layout
     * @param start_index
     * @param section
     */
    void AbstractBugsSubWidget::createList_itemPD_Album_applyingWithData(const QList<bugs::PD_AlbumItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            bugs::ItemPD_Album_bugs *tmp_widget = new bugs::ItemPD_Album_bugs(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &bugs::ItemPD_Album_bugs::signal_clicked, this, &AbstractBugsSubWidget::slot_clickedItemPD_Album);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::PD_Album_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }
    }


    /**
     * @brief AbstractBugsSubWidget::createList_itemMyAlbum_applyingWithData
     * @param list_data
     * @param imageSizeMode
     * @param p_layout
     * @param start_index
     * @param section
     */
    void AbstractBugsSubWidget::createList_itemMyAlbum_applyingWithData(const QList<bugs::MyAlbumItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            bugs::ItemMyAlbum_bugs *tmp_widget = new bugs::ItemMyAlbum_bugs(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &bugs::ItemMyAlbum_bugs::signal_clicked, this, &AbstractBugsSubWidget::slot_clickedItemMyAlbum);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::MyAlbum_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }
    }



    /**
     * @brief AbstractBugsSubWidget::createList_itemArtist_applyingWithData
     * @param list_data
     * @param imageSizeMode
     * @param p_layout
     * @param start_index
     * @param section
     */
    void AbstractBugsSubWidget::createList_itemArtist_applyingWithData(const QList<bugs::ArtistItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            bugs::ItemArtist_bugs *tmp_widget = new bugs::ItemArtist_bugs(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &bugs::ItemArtist_bugs::signal_clicked, this, &AbstractBugsSubWidget::slot_clickedItemArtist);
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
     * @brief AbstractBugsSubWidget::createList_itemVideo_applyingWithData
     * @param list_data
     * @param imageSizeMode
     * @param p_layout
     * @param section
     */
    void AbstractBugsSubWidget::createList_itemVideo_applyingWithData(const QList<bugs::VideoItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            bugs::ItemVideo_bugs *tmp_widget = new bugs::ItemVideo_bugs(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &bugs::ItemVideo_bugs::signal_clicked, this, &AbstractBugsSubWidget::slot_clickedItemVideo);
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
    void AbstractBugsSubWidget::createList_itemTrackDelegate_applyingWithData(const QList<bugs::TrackItemData> &list_data, QListWidget *p_listWidget, bool flagAdjustHeight){
        for(int i=0 ; i<list_data.size() ; i++){
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_trackData(list_data.at(i)));
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
            // No Data
            p_listWidget->setVisible(false);
        }
    }


    /**
     * @brief TrackItemData 데이터로 ListWidgetItem 생성하여 ListWidget에 추가한다. 이미지 처리가 포함됨. (slot 연결이 있음)
     * @param list_data
     * @param p_listWidget
     * @param flagAdjustHeight 기본값은 false. list_data 개수에 맞춰서 ListWidget의 사이즈를 조정할지의 여부임
     */
    void AbstractBugsSubWidget::createList_itemTrackDelegate_applyingWithData_showImage(const QList<bugs::TrackItemData>& list_data, QListWidget *p_listWidget, const int start_index, const int section, bool flagAdjustHeight){
        // Track 기본 이미지
        QPixmap pixmap_default = this->getDefaultImage_forTrack();

        for(int i=0 ; i<list_data.size() ; i++){
            QListWidgetItem *tmp_item = new QListWidgetItem();
            tmp_item->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_trackData(list_data.at(i)));

            QPixmap pixmap;
            QString tmp_imgPath = list_data.at(i).album_image;
            if(tmp_imgPath.isEmpty()){
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image
            }
            else{
                // download (no-cache)
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image (download 실패 케이스 대응을 위한 기본값 우선 세팅)

                FileDownloader *fileDownloader = new FileDownloader(this);
                connect(fileDownloader, &FileDownloader::downloaded, this, &AbstractBugsSubWidget::slot_thumbnailDownloaded_fotListWidget_delegate);
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
            // No Data
            p_listWidget->setVisible(false);
        }
    }


    /**
     * @brief ListWidget과 관련 - Image 다운로드가 완료되면 ListWidget에 보이도록 하기 위한 처리 (DecorationRole 로 설정)
     * @param p_listWidget
     * @param p_list_track
     */
    void AbstractBugsSubWidget::applyImageData_itemTrackDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<bugs::TrackItemData>* p_list_track){
        int tmp_index = fileDownload_sender->property("index").toInt();

        if(tmp_index < p_list_track->count()){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownload_sender->downloadedData());

            if(flagLoaded){
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(IMAGE_SIZE_OF_TRACK, IMAGE_SIZE_OF_TRACK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                p_listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
                QPixmapCache::insert(p_list_track->at(tmp_index).album_image, tmp_pixmap);
            }
        }

        fileDownload_sender->deleteLater();
    }


    /**
     * @brief ArtistItemData 데이터로 ListWidgetItem 생성하여 ListWidget에 추가한다. 이미지 처리가 포함됨. (slot 연결이 있음)
     * @param list_data
     * @param p_listWidget
     * @param start_index
     * @param section
     * @param flagAdjustHeight
     */
    void AbstractBugsSubWidget::createList_itemArtistDelegate_applyingWithData_showImage(const QList<bugs::ArtistItemData>& list_data, QListWidget *p_listWidget, const int start_index, const int section, bool flagAdjustHeight){
        // Track 기본 이미지
        QPixmap pixmap_default = this->getDefaultImage_forTrack();

        for(int i=0 ; i<list_data.size() ; i++){
            QListWidgetItem *tmp_item = new QListWidgetItem();
            tmp_item->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_artistData(list_data.at(i)));

            QPixmap pixmap;
            QString tmp_imgPath = list_data.at(i).image;
            if(tmp_imgPath.isEmpty()){
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image
            }
            else{
                // download (no-cache)
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image (download 실패 케이스 대응을 위한 기본값 우선 세팅)

                FileDownloader *fileDownloader = new FileDownloader(this);
                connect(fileDownloader, &FileDownloader::downloaded, this, &AbstractBugsSubWidget::slot_thumbnailDownloaded_fotListWidget_delegate);
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
            // No Data
            p_listWidget->setVisible(false);
        }
    }


    /**
     * @brief ListWidget과 관련 - Image 다운로드가 완료되면 ListWidget에 보이도록 하기 위한 처리 (DecorationRole 로 설정)
     * @param p_listWidget
     * @param p_list_artist
     */
    void AbstractBugsSubWidget::applyImageData_itemArtistDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<bugs::ArtistItemData>* p_list_artist){
        int tmp_index = fileDownload_sender->property("index").toInt();

        if(tmp_index < p_list_artist->count()){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownload_sender->downloadedData());

            if(flagLoaded){
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(IMAGE_SIZE_OF_TRACK, IMAGE_SIZE_OF_TRACK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                p_listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
                QPixmapCache::insert(p_list_artist->at(tmp_index).image, tmp_pixmap);
//                QPixmap tmp_pixmap;
//                tmp_pixmap = tmp_pixmap.fromImage(image);

//                // 빈 Pixmap
//                QPixmap pixmap_painter = QPixmap(QSize(IMAGE_SIZE_OF_TRACK, IMAGE_SIZE_OF_TRACK));
//                pixmap_painter.fill(Qt::transparent);

//                QPainter painter (&pixmap_painter);
//                painter.setRenderHint(QPainter::Antialiasing, true);
//                QBrush brush = QBrush(tmp_pixmap);
//                painter.setBrush(brush);
//                painter.drawRoundedRect(0, 0, IMAGE_SIZE_OF_TRACK, IMAGE_SIZE_OF_TRACK, IMAGE_SIZE_OF_TRACK/2, IMAGE_SIZE_OF_TRACK/2);
//                p_listWidget->item(tmp_index)->setData(Qt::DecorationRole, pixmap_painter);

            }
        }

        fileDownload_sender->deleteLater();
    }






    // 자식 클래스에서 Track목록 바로 재생 ---------------------------------------------------------------------------------------------

    /**
     * @brief JsonArr_dataToPlay 에 대해서, 전체재생 or 셔플재생을 Rose로 요청.
     * @param jsonArr_dataToPlay
     * @param flagShuffle
     */
    void AbstractBugsSubWidget::proc_play_allTrack(const QJsonArray& jsonArr_trackToPlay, const bool flagShuffle){
        ProcRoseAPI_withBugs::PlayShuffleMode shuffleMode = ProcRoseAPI_withBugs::PlayShuffleMode::JustPlay;
        if(flagShuffle){
            shuffleMode = ProcRoseAPI_withBugs::PlayShuffleMode::ShufflePlay;
        }

        // Rose Play 요청 - Bugs Track (전체재생 / 셔플재생)
        ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
        procRosePlay->requestPlayRose_byTracks(jsonArr_trackToPlay, 0, OptMorePopup::ClickMode::Play_RightNow, shuffleMode);

    }


    /**
     * @brief AbstractBugsSubWidget::proc_play_allVideo
     * @param jsonArr_videoToPlay
     * @param flagShuffle
     */
    void AbstractBugsSubWidget::proc_play_allVideo(const QJsonArray& jsonArr_videoToPlay, const bool flagShuffle){
        ProcRoseAPI_withBugs::PlayShuffleMode shuffleMode = ProcRoseAPI_withBugs::PlayShuffleMode::JustPlay;
        if(flagShuffle){
            shuffleMode = ProcRoseAPI_withBugs::PlayShuffleMode::ShufflePlay;
        }

        // Rose Play 요청 - Bugs Video (전체재생 / 셔플재생)
        ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
        procRosePlay->requestPlayRose_byVideos(jsonArr_videoToPlay, 0, OptMorePopup::ClickMode::Play_RightNow, shuffleMode);

    }



    // Item 클릭 관련 처리 of Album ---------------------------------------------------------------------------------------------

    /**
     * @brief AbstractBugsSubWidget::proc_clicked_itemAlbum
     * @param data_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemAlbum(bugs::AlbumItemData& data_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                    // Album 바로 재생
                    ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byAlbumID(data_album.album_id, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                    // Album Detail 페이지 진입
                    this->movePage_album_detailView(data_album.album_id);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    print_debug();

                    // OptionPopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Bugs_Album, ConvertData_forBugs::getConvertOptHeaderData(data_album), index, section);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
                    // 좋아요 하트 On 처리 - Case (2) :: 아래 proc_clicked_itemTrack_inList 함수 주석 참고
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_album.album_id);
                    itemPosData.likes_yn = true;
                    this->proc_preUpdate_favorite_album(itemPosData);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    proc->request_bugs_addFavorite_album(data_album.album_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
                    // 좋아요 하트 Off (해제) 처리 - Case (2) :: 아래 proc_clicked_itemTrack_inList 함수 주석 참고
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_album.album_id);
                    itemPosData.likes_yn = false;
                    this->proc_preUpdate_favorite_album(itemPosData);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    proc->request_bugs_deleteFavorite_album(data_album.album_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
            }

        }
    }

    /**
     * @brief AbstractBugsSubWidget::proc_clicked_itemAlbum
     * @param list_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemAlbum(QList<bugs::AlbumItemData>* list_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_album->length(), index);
            if(real_index >= 0){
                bugs::AlbumItemData data = list_album->at(real_index);
                this->proc_clicked_itemAlbum(data, clickMode, real_index, section);
            }
        }
    }






    // Item 클릭 관련 처리 of PD_Album ---------------------------------------------------------------------------------------------

    /**
     * @brief AbstractBugsSubWidget::proc_clicked_itemPD_Album
     * @param data_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemPD_Album(bugs::PD_AlbumItemData& data_pd_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                    // PD Album 바로 재생
                    ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byPD_AlbumID(data_pd_album.es_album_id, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                    // PD Album Detail 페이지 진입
                    this->movePage_pd_album_detailView(data_pd_album.es_album_id);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    print_debug();

                    // OptionPopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Bugs_PD_Album, ConvertData_forBugs::getConvertOptHeaderData(data_pd_album), index, section);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
                    // 좋아요 하트 On 처리 - Case (2) :: 아래 proc_clicked_itemTrack_inList 함수 주석 참고
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_pd_album.es_album_id);
                    itemPosData.likes_yn = true;
                    this->proc_preUpdate_favorite_pd_album(itemPosData);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    proc->request_bugs_addFavorite_pd_album(data_pd_album.es_album_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
                    // 좋아요 하트 Off (해제) 처리 - Case (2) :: 아래 proc_clicked_itemTrack_inList 함수 주석 참고
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_pd_album.es_album_id);
                    itemPosData.likes_yn = false;
                    this->proc_preUpdate_favorite_pd_album(itemPosData);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    proc->request_bugs_deleteFavorite_pd_album(data_pd_album.es_album_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
            }

        }
    }

    /**
     * @brief AbstractBugsSubWidget::proc_clicked_itemPD_Album
     * @param list_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemPD_Album(QList<bugs::PD_AlbumItemData>* list_pd_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_pd_album->length(), index);
            if(real_index >= 0){
                bugs::PD_AlbumItemData data = list_pd_album->at(real_index);
                this->proc_clicked_itemPD_Album(data, clickMode, real_index, section);
            }
        }
    }



    // Item 클릭 관련 처리 of My Album ---------------------------------------------------------------------------------------------

    /**
     * @brief AbstractBugsSubWidget::proc_clicked_itemMyAlbum
     * @param data_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemMyAlbum(bugs::MyAlbumItemData& data_my_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                    // My Album 바로 재생
                    ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byMyAlbumID(data_my_album.playlist_id, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                    // My Album Detail 페이지 진입
                    this->movePage_my_album_detailView(data_my_album);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    // OptionPopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Bugs_MyAlbum, ConvertData_forBugs::getConvertOptHeaderData(data_my_album), index, section);
                }
            }
        }
    }

    /**
     * @brief AbstractBugsSubWidget::proc_clicked_itemMyAlbum
     * @param list_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemMyAlbum(QList<bugs::MyAlbumItemData>* list_my_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_my_album->length(), index);
            if(real_index >= 0){
                bugs::MyAlbumItemData data = list_my_album->at(real_index);
                this->proc_clicked_itemMyAlbum(data, clickMode, real_index, section);
            }
        }
    }






    // Item 클릭 관련 처리 of Artist ---------------------------------------------------------------------------------------------

    /**
     * @brief AbstractBugsSubWidget::proc_clicked_itemArtist
     * @param data_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemArtist(bugs::ArtistItemData& data_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                    // Artist Detail 페이지 진입
                    this->movePage_artist_detailView(data_artist.artist_id);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    print_debug();
                    // OptionPopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Bugs_Artist, ConvertData_forBugs::getConvertOptHeaderData(data_artist), index, section);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
                    // 좋아요 하트 On 처리 - Case (2) :: 아래 proc_clicked_itemTrack_inList 함수 주석 참고
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_artist.artist_id);
                    itemPosData.likes_yn = true;
                    this->proc_preUpdate_favorite_album(itemPosData);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    proc->request_bugs_addFavorite_artist(data_artist.artist_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
                    // 좋아요 하트 Off (해제) 처리 - Case (2) :: 아래 proc_clicked_itemTrack_inList 함수 주석 참고
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_artist.artist_id);
                    itemPosData.likes_yn = false;
                    this->proc_preUpdate_favorite_album(itemPosData);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    proc->request_bugs_deleteFavorite_artist(data_artist.artist_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
            }

        }
    }

    /**
     * @brief AbstractBugsSubWidget::proc_clicked_itemArtist
     * @param list_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemArtist(QList<bugs::ArtistItemData>* list_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_artist->length(), index);
            if(real_index >= 0){
                bugs::ArtistItemData data = list_artist->at(real_index);
                this->proc_clicked_itemArtist(data, clickMode, real_index, section);
            }
        }
    }




    // Item 클릭 관련 처리 of Video ---------------------------------------------------------------------------------------------

    void AbstractBugsSubWidget::proc_clicked_itemVideo(bugs::VideoItemData& data_video, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn
                        || clickMode == tidal::AbstractItem::ClickMode::AllBox
                ){
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

                    // Video 바로 재생
                    ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                    //procRosePlay->requestPlayRose_byVideos(jsonArray_toPlayAll, index, OptMorePopup::ClickMode::Play_RightNow_withReorder);
                    procRosePlay->requestPlayRose_byVideos(jsonArray_toPlayAll, index, playType);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    print_debug();

                    // OptionPopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Bugs_Video, ConvertData_forBugs::getConvertOptHeaderData(data_video), index, section);
                }
            }

        }
    }


    /**
     * @brief AbstractBugsSubWidget::proc_clicked_itemVideo
     * @param list_video
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemVideo(QList<bugs::VideoItemData>* list_video, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_video->length(), index);
            if(real_index >= 0){
                bugs::VideoItemData data = list_video->at(real_index);
                this->proc_clicked_itemVideo(data, jsonArray_toPlayAll, clickMode, real_index, section);
            }
        }
    }



    // Item 클릭 관련 처리 of Track_Square ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemVideo에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_video
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemTrackSquare(bugs::TrackItemData &data_track, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn
                        || clickMode == tidal::AbstractItem::ClickMode::AllBox
                ){
                     // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음
                     ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                     procRosePlay->requestPlayRose_byTracks(jsonArray_toPlayAll, index, OptMorePopup::ClickMode::Play_RightNow_withReorder);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){

                    //---------------------------------//c220825_1

                    // OptMorePopup 띄우기 필요 - (연관 영상 있는지의 여부에 따라 다름)
                    if(data_track.list_mv_id.count() > 0){
                        this->makeObj_optMorePopup(OptMorePopup::Bugs_Track_withVideo, ConvertData_forBugs::getConvertOptHeaderData(data_track), index, section);
                    }
                    else{
                        this->makeObj_optMorePopup(OptMorePopup::Bugs_Track, ConvertData_forBugs::getConvertOptHeaderData(data_track), index, section);
                    }
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
                    // 좋아요 하트 On 처리 - Case (2) :: 아래 proc_clicked_itemTrack_inList 함수 주석 참고
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_track.track_id);
                    itemPosData.likes_yn = true;
                    this->proc_preUpdate_favorite_track(itemPosData);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    proc->request_bugs_addFavorite_track(data_track.track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
                    // 좋아요 하트 Off (해제) 처리 - Case (2) :: 아래 proc_clicked_itemTrack_inList 함수 주석 참고
                    // for sync data with UI (after response)
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_track.track_id);
                    itemPosData.likes_yn = false;
                    this->proc_preUpdate_favorite_track(itemPosData);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    proc->request_bugs_deleteFavorite_track(data_track.track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
            }

        }
    }



    /**
     * @brief ItemVideo에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함  [overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_artist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemTrackSquare(QList<bugs::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                bugs::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_itemTrackSquare(data, jsonArray_toPlayAll, clickMode, real_index, section);
            }
        }
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
    void AbstractBugsSubWidget::proc_clicked_itemTrack_inList(bugs::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::AllBox){
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

                    ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byTracks(jsonArr_toPlayAll, index, playType);
                }
                else if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::MoreBtn){
                    //---------------------------------//c220825_1
                    print_debug();

                    // OptMorePopup 띄우기 필요 - (연관 영상 있는지의 여부에 따라 다름)
                    if(data_track.list_mv_id.count() > 0){
                        this->makeObj_optMorePopup(OptMorePopup::Bugs_Track_withVideo, ConvertData_forBugs::getConvertOptHeaderData(data_track), index, section);
                    }
                    else{
                        this->makeObj_optMorePopup(OptMorePopup::Bugs_Track, ConvertData_forBugs::getConvertOptHeaderData(data_track), index, section);
                    }
                }
                else if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::FavBtn){
                    // for sync data with UI (after response)
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_track.track_id);
                    itemPosData.likes_yn = !data_track.likes_yn;

                    // Case 2 : Data, View를 먼저 업데이트 (사용자 이벤에 의한), Http 요청에 대하여 실패의 경우만 원복 처리 진행.
                    //      (원복 안해도 될것 같고. 어차피 사용자가 계속 요청하니까. 필요한 경우에 실패에 대한 원복 처리만 추가보강)
                    this->proc_preUpdate_favorite_track(itemPosData);
                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    if(data_track.likes_yn){
                        proc->request_bugs_deleteFavorite_track(data_track.track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else{
                        proc->request_bugs_addFavorite_track(data_track.track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }


                    // Case 1 : Http 요청을 하고, 응답에 따라 Data, View에 동기화
                    /*
                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    if(data_track.likes_yn){
                        connect(proc, &ProcBugsAPI::completeReq_favarite_track, this, &AbstractBugsSubWidget::slot_completedReg_favorite_track);
                        proc->request_tidal_deleteFavorite_track(data_track.track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else{
                        connect(proc, &ProcBugsAPI::completeReq_favarite_track, this, &AbstractBugsSubWidget::slot_completedReg_favorite_track);
                        proc->request_tidal_addFavorite_track(data_track.track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    */
                }
                else if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::MusicVideoBtn){

                    QJsonObject tmp_track = ConvertData_forBugs::getObjectJson_trackData(data_track);

                    this->dlgVideo = new DialogVideoList(this);
                    this->dlgVideo->setData(tmp_track);
                    this->dlgVideo->exec();

                }
            }
        }
    }


    void AbstractBugsSubWidget::slot_applyResult_getShareLink(const QString &link){//c220825_1

        this->shareLink = link;
        print_debug();
        qDebug() << "this->shareLink=" << this->shareLink;
    }

    void AbstractBugsSubWidget::proc_clicked_itemTrack_inList(bugs::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                    // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
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

                    ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byTracks(jsonArr_toPlayAll, index, playType);
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){

                    // OptMorePopup 띄우기 필요 - (연관 영상 있는지의 여부에 따라 다름)
                    if(data_track.list_mv_id.count() > 0){
                        this->makeObj_optMorePopup(OptMorePopup::Bugs_Track_withVideo, ConvertData_forBugs::getConvertOptHeaderData(data_track), index, section);
                    }
                    else{
                        this->makeObj_optMorePopup(OptMorePopup::Bugs_Track, ConvertData_forBugs::getConvertOptHeaderData(data_track), index, section);
                    }
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){
                    // for sync data with UI (after response)
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_track.track_id);
                    itemPosData.likes_yn = !data_track.likes_yn;

                    // Case 2 : Data, View를 먼저 업데이트 (사용자 이벤에 의한), Http 요청에 대하여 실패의 경우만 원복 처리 진행.
                    //      (원복 안해도 될것 같고. 어차피 사용자가 계속 요청하니까. 필요한 경우에 실패에 대한 원복 처리만 추가보강)
                    this->proc_preUpdate_favorite_track(itemPosData);
                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    if(data_track.likes_yn){
                        proc->request_bugs_deleteFavorite_track(data_track.track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else{
                        proc->request_bugs_addFavorite_track(data_track.track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }


                    // Case 1 : Http 요청을 하고, 응답에 따라 Data, View에 동기화
                    /*
                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    if(data_track.likes_yn){
                        connect(proc, &ProcBugsAPI::completeReq_favarite_track, this, &AbstractBugsSubWidget::slot_completedReg_favorite_track);
                        proc->request_tidal_deleteFavorite_track(data_track.track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else{
                        connect(proc, &ProcBugsAPI::completeReq_favarite_track, this, &AbstractBugsSubWidget::slot_completedReg_favorite_track);
                        proc->request_tidal_addFavorite_track(data_track.track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    */
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MusicVideoBtn){

                    QJsonObject tmp_track = ConvertData_forBugs::getObjectJson_trackData(data_track);

                    this->dlgVideo = new DialogVideoList(this);
                    this->dlgVideo->setData(tmp_track);
                    this->dlgVideo->exec();

                }
            }
        }
    }


    void AbstractBugsSubWidget::slot_dlg_signal_video(const QJsonObject &p_jsonObject){

        QJsonArray tmp_artist_id = ProcJsonEasy::getJsonArray(p_jsonObject, "list_artist_id");

        if(tmp_artist_id.count() == 1){            
            this->movePage_artist_detailView(tmp_artist_id.at(0).toInt());
            global.user_forBugs.dlg_set_state(true);
        }
        else{
            if(global.user_forBugs.dlg_isSignal() == false){
                QJsonObject tmp_artist = p_jsonObject;
                tmp_artist.insert("manufacture", "bugs");

                this->dlgArtist = new DialogArtistList(this);
                this->dlgArtist->setData(tmp_artist);
                this->dlgArtist->exec();
                global.user_forBugs.dlg_set_signal(true);
            }
        }
    }

    void AbstractBugsSubWidget::slot_dlg_signal_artist(const QJsonObject &p_jsonObject){

        int artist_id = ProcJsonEasy::getInt(p_jsonObject, "artist_id");

        this->movePage_artist_detailView(artist_id);
        global.user_forBugs.dlg_set_state(true);
    }


    /**
     * @brief AbstractTidalTrackDelegate 에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함 [overloading]
     * @param list_track
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractBugsSubWidget::proc_clicked_itemTrack_inList(QList<bugs::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{

            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                bugs::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_itemTrack_inList(data, jsonArr_toPlayAll, clickMode, real_index, section);
            }
        }
    }


    void AbstractBugsSubWidget::proc_clicked_itemTrack_inList(QList<bugs::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{

            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                bugs::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_itemTrack_inList(data, jsonArr_toPlayAll, clickMode, real_index, section);
            }
        }
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
    void AbstractBugsSubWidget::proc_clicked_itemArtist_inList(bugs::ArtistItemData& data_artist, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::AllBox){
                    // Artist - Detail 페이지 이동
                    if(global.user_forBugs.dlg_isShow() == true){
                        global.user_forBugs.dlg_set_state(false);
                    }
                    this->movePage_artist_detailView(data_artist.artist_id);
                }
                else if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::MoreBtn){
                    print_debug();
                    // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Bugs_Artist, ConvertData_forBugs::getConvertOptHeaderData(data_artist), index, section, true);
                }
                else if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::FavBtn){
                    // for sync data with UI (after response)
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = index;
                    itemPosData.data_id = QString("%1").arg(data_artist.artist_id);
                    itemPosData.likes_yn = !data_artist.likes_yn;

                    // Case 2 : Data, View를 먼저 업데이트
                    this->proc_preUpdate_favorite_artist(itemPosData);
                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    if(data_artist.likes_yn){
                        proc->request_bugs_deleteFavorite_artist(data_artist.artist_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else{
                        proc->request_bugs_addFavorite_artist(data_artist.artist_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
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
    void AbstractBugsSubWidget::proc_clicked_itemArtist_inList(QList<bugs::ArtistItemData>* list_artist, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_artist->length(), index);
            if(real_index >= 0){
                bugs::ArtistItemData data = list_artist->at(real_index);
                this->proc_clicked_itemArtist_inList(data, clickMode, real_index, section);
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
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromAlbum(bugs::AlbumItemData& data_album, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
        )
        {
            // Rose Play 요청
            ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
            procRosePlay->requestPlayRose_byAlbumID(data_album.album_id, clickMode);
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist DetailView
            //this->movePage_artist_detailView(data_album.list_artist_id, data_album.list_artist_nm);

            // Artist DetailView
            if(data_album.list_artist_id.count() > 1){
                QJsonObject tmp_artist = ConvertData_forBugs::getObjectJson_albumData(data_album);
                tmp_artist.insert("manufacture", "bugs");

                this->dlgArtist = new DialogArtistList(this);
                this->dlgArtist->setData(tmp_artist);
                this->dlgArtist->exec();
            }
            else{
                if(global.user_forBugs.dlg_isShow() == true){
                    global.user_forBugs.dlg_set_state(false);
                }

                if(global.user_forBugs.flag_rosehome == true){
                    global.user_forBugs.rosehome_obj = QJsonObject();
                    global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ARTIST_DETAIL);
                    bugs::ArtistItemData tmp_data_artist;
                    tmp_data_artist.artist_id = data_album.list_artist_id.first();
                    QJsonObject tmp_data = ConvertData_forBugs::getObjectJson_artistData(tmp_data_artist);
                    global.user_forBugs.rosehome_obj.insert(KEY_DATA, tmp_data);

                    emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
                }
                else{
                    this->movePage_artist_detailView(data_album.list_artist_id, data_album.list_artist_nm);
                }
            }
        }

    }



    /**
     * @brief AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromAlbum(QList<bugs::AlbumItemData>* list_album, const int index, const OptMorePopup::ClickMode clickMode){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_album->length(), index);
            if(real_index >= 0){
                bugs::AlbumItemData data = list_album->at(real_index);
                this->proc_clicked_optMorePopup_fromAlbum(data, clickMode);
            }
        }

    }





    // OptMorePopup 관련 처리 of PD's Album ---------------------------------------------------------------------------------------------

    /**
     * @brief PD_AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromPD_Album(bugs::PD_AlbumItemData& data_pd_album, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
        )
        {
            // Rose Play 요청
            ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
            procRosePlay->requestPlayRose_byPD_AlbumID(data_pd_album.es_album_id, clickMode);
        }

    }



    /**
     * @brief PD_AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromPD_Album(QList<bugs::PD_AlbumItemData>* list_pd_album, const int index, const OptMorePopup::ClickMode clickMode){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_pd_album->length(), index);
            if(real_index >= 0){
                bugs::PD_AlbumItemData data = list_pd_album->at(real_index);
                this->proc_clicked_optMorePopup_fromPD_Album(data, clickMode);
            }
        }

    }





    // OptMorePopup 관련 처리 of My Album ---------------------------------------------------------------------------------------------

    /**
     * @brief PD_AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromMyAlbum(bugs::MyAlbumItemData& data_my_album, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
        )
        {
            // Rose Play 요청
            ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
            procRosePlay->requestPlayRose_byMyAlbumID(data_my_album.playlist_id, clickMode);
        }

    }



    /**
     * @brief PD_AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromMyAlbum(QList<bugs::MyAlbumItemData>* list_my_album, const int index, const OptMorePopup::ClickMode clickMode){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_my_album->length(), index);
            if(real_index >= 0){
                bugs::MyAlbumItemData data = list_my_album->at(real_index);
                this->proc_clicked_optMorePopup_fromMyAlbum(data, clickMode);
            }
        }

    }




    // OptMorePopup 관련 처리 of Video ---------------------------------------------------------------------------------------------

    /**
     * @brief TrackItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromVideo(bugs::VideoItemData& data_video, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

        }
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
            ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
            procRosePlay->requestPlayRose_byVideos(jsonArr_toPlayAll, index, clickMode);
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist DetailView
            //this->movePage_artist_detailView(data_video.list_artist_id, data_video.list_artist_nm);

            // Artist DetailView
            if(data_video.list_artist_id.count() > 1){
                QJsonObject tmp_artist = ConvertData_forBugs::getObjectJson_videoData(data_video);
                tmp_artist.insert("manufacture", "bugs");

                this->dlgArtist = new DialogArtistList(this);
                this->dlgArtist->setData(tmp_artist);
                this->dlgArtist->exec();
            }
            else{
                if(global.user_forBugs.dlg_isShow() == true){
                    global.user_forBugs.dlg_set_state(false);
                }
                this->movePage_artist_detailView(data_video.list_artist_id, data_video.list_artist_nm);
            }
        }

    }



    /**
     * @brief TrackItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromVideo(QList<bugs::VideoItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                bugs::VideoItemData data = list_track->at(real_index);
                this->proc_clicked_optMorePopup_fromVideo(data, jsonArr_toPlayAll, real_index, clickMode);
            }
        }
    }



    // OptMorePopup 관련 처리 of Track ---------------------------------------------------------------------------------------------

    /**
     * @brief TrackItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromTrack(bugs::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
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
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(tmpJsonArr, 0, clickMode);
            }
            else{
                QJsonObject tmpJsonObj = jsonArr_toPlayAll.at(index).toObject();
                QJsonArray tmpJsonArr = QJsonArray();
                tmpJsonArr.append(tmpJsonObj);

                // Rose Play 요청
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(tmpJsonArr, 0, clickMode);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist DetailView
            //this->movePage_artist_detailView(data_track.list_artist_id, data_track.list_artist_nm);

            // Artist DetailView
            if(data_track.list_artist_id.count() > 1){
                QJsonObject tmp_artist = ConvertData_forBugs::getObjectJson_trackData(data_track);
                tmp_artist.insert("manufacture", "bugs");

                this->dlgArtist = new DialogArtistList(this);
                this->dlgArtist->setData(tmp_artist);
                this->dlgArtist->exec();
            }
            else{
                if(global.user_forBugs.dlg_isShow() == true){
                    global.user_forBugs.dlg_set_state(false);
                }

                if(global.user_forBugs.flag_rosehome == true){
                    global.user_forBugs.rosehome_obj = QJsonObject();
                    global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ARTIST_DETAIL);
                    bugs::ArtistItemData tmp_data_artist;
                    tmp_data_artist.artist_id = data_track.list_artist_id.first();
                    QJsonObject tmp_data = ConvertData_forBugs::getObjectJson_artistData(tmp_data_artist);
                    global.user_forBugs.rosehome_obj.insert(KEY_DATA, tmp_data);

                    emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
                }
                else{
                    this->movePage_artist_detailView(data_track.list_artist_id, data_track.list_artist_nm);
                }
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Album){
            // Album DetailView
            if(global.user_forBugs.flag_rosehome == true){
                global.user_forBugs.rosehome_obj = QJsonObject();
                global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_DETAIL);
                bugs::AlbumItemData tmp_data_album;
                tmp_data_album.album_id = data_track.album_id;
                QJsonObject tmp_data = ConvertData_forBugs::getObjectJson_albumData(tmp_data_album);
                global.user_forBugs.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
            }
            else{
                this->movePage_album_detailView(data_track.album_id);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Ref_Videos){
            // 영상보기 (Track에 대한 연관 영상)
            this->movePage_video_allView_ofTrack(data_track);
        }
    }



    /**
     * @brief TrackItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromTrack(QList<bugs::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                bugs::TrackItemData data = list_track->at(real_index);
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
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromArtist(bugs::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            if(global.user_forBugs.dlg_isShow() == true){
                global.user_forBugs.dlg_set_state(false);
            }
            // Artist DetailView
            this->movePage_artist_detailView(data_artist.artist_id);
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
    void AbstractBugsSubWidget::proc_clicked_optMorePopup_fromArtist(QList<bugs::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode){
        if(this->is_bugs_logined() == false){
            this->showNeededLoginBugs();               // Bugs 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_artist->length(), index);
            if(real_index >= 0){
                bugs::ArtistItemData data = list_artist->at(real_index);
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
     * @brief AbstractBugsSubWidget::proc_clicked_imageDetailInfo_fromAlbum
     * @param data_album - 현재는 id만 사용하지만, 차후 확장 고려위해 AlbumItemData 을 받도록함
     * @param btnClickMode
     */
    void AbstractBugsSubWidget::proc_clicked_imageDetailInfo_fromAlbum(bugs::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_album.album_id > 0){
            if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd
                    || btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete
            ){

                // 즐겨찾기 추가/삭제 - Album

                // for sync data with UI (after response)
                ItemPositionData itemPosData;
                itemPosData.section = section;
                itemPosData.index = 0;
                itemPosData.data_id = QString("%1").arg(data_album.album_id);
                itemPosData.likes_yn = !data_album.likes_yn;

                // Case 2 : Data, View를 먼저 업데이트 (사용자 이벤에 의한), 그 후 Http 요청
                this->proc_preUpdate_favorite_album(itemPosData);
                ProcBugsAPI *proc = new ProcBugsAPI(this);
                if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
                    proc->request_bugs_deleteFavorite_album(data_album.album_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
                else{
                    proc->request_bugs_addFavorite_album(data_album.album_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }

            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Share){
                //c220818
                //220816 share code ... --
                setUIShare();//c220818
                print_debug();
                qDebug() << this->shareLink;
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::More){

                //c220825_2  이하 10줄

                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                this->makeObj_optMorePopup(OptMorePopup::Bugs_Album, ConvertData_forBugs::getConvertOptHeaderData(data_album), 0, section, true);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
                // 전체재생 - Rose Play 요청
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRoseAPI_withBugs::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
                // 셔플재생 - Rose Play 요청
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRoseAPI_withBugs::PlayShuffleMode::ShufflePlay);
            }
        }
    }



    /**
     * @brief AbstractBugsSubWidget::proc_clicked_imageDetailInfo_fromPDAlbum
     * @param data_pd_album
     * @param p_jsonArr_toPlay
     * @param section
     * @param btnClickMode
     */
    void AbstractBugsSubWidget::proc_clicked_imageDetailInfo_fromPDAlbum(bugs::PD_AlbumItemData& data_pd_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_pd_album.es_album_id > 0){
            if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd
                    || btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete
            ){

                // 즐겨찾기 추가/삭제 - Album

                // for sync data with UI (after response)
                ItemPositionData itemPosData;
                itemPosData.section = section;
                itemPosData.index = 0;
                itemPosData.data_id = QString("%1").arg(data_pd_album.es_album_id);
                itemPosData.likes_yn = !data_pd_album.likes_yn;

                // Case 2 : Data, View를 먼저 업데이트 (사용자 이벤에 의한), 그 후 Http 요청
                this->proc_preUpdate_favorite_album(itemPosData);
                ProcBugsAPI *proc = new ProcBugsAPI(this);
                if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
                    proc->request_bugs_deleteFavorite_pd_album(data_pd_album.es_album_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
                else{
                    proc->request_bugs_addFavorite_pd_album(data_pd_album.es_album_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }

            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Share){
                //c220818
                print_debug();
                //220816 share code ... --
                qDebug() << this->shareLink;

                setUIShare();//c220818
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::More){
                print_debug();
                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                this->makeObj_optMorePopup(OptMorePopup::Bugs_PD_Album, ConvertData_forBugs::getConvertOptHeaderData(data_pd_album), 0, section, true);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
                // 전체재생 - Rose Play 요청
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRoseAPI_withBugs::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
                // 셔플재생 - Rose Play 요청
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRoseAPI_withBugs::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    /**
     * @brief AbstractBugsSubWidget::proc_clicked_imageDetailInfo_fromArtist
     * @param data_artist
     * @param p_jsonArr_toPlay
     * @param section
     * @param btnClickMode
     */
    void AbstractBugsSubWidget::proc_clicked_imageDetailInfo_fromArtist(bugs::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_artist.artist_id > 0){
            if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd
                    || btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete
            ){

                // 즐겨찾기 추가/삭제 - Artist

                // for sync data with UI (after response)
                ItemPositionData itemPosData;
                itemPosData.section = section;
                itemPosData.index = 0;
                itemPosData.data_id = QString("%1").arg(data_artist.artist_id);
                itemPosData.likes_yn = !data_artist.likes_yn;

                // Case 2 : Data, View를 먼저 업데이트 (사용자 이벤에 의한), 그 후 Http 요청
                this->proc_preUpdate_favorite_artist(itemPosData);
                ProcBugsAPI *proc = new ProcBugsAPI(this);
                if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
                    proc->request_bugs_deleteFavorite_artist(data_artist.artist_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
                else{
                    proc->request_bugs_addFavorite_artist(data_artist.artist_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }

            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
                // 전체재생 - Rose Play 요청
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRoseAPI_withBugs::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
                // 셔플재생 - Rose Play 요청
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRoseAPI_withBugs::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    void AbstractBugsSubWidget::proc_clicked_imageDetailInfo_fromArtist(bugs::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode){

        // 데이터 유효할 때에만
        if(data_artist.artist_id > 0){
            if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd
                    || btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete
            ){

                // 즐겨찾기 추가/삭제 - Artist

                // for sync data with UI (after response)
                ItemPositionData itemPosData;
                itemPosData.section = section;
                itemPosData.index = 0;
                itemPosData.data_id = QString("%1").arg(data_artist.artist_id);
                itemPosData.likes_yn = !data_artist.likes_yn;

                // Case 2 : Data, View를 먼저 업데이트 (사용자 이벤에 의한), 그 후 Http 요청
                this->proc_preUpdate_favorite_artist(itemPosData);
                ProcBugsAPI *proc = new ProcBugsAPI(this);
                if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){
                    proc->request_bugs_deleteFavorite_artist(data_artist.artist_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
                else{
                    proc->request_bugs_addFavorite_artist(data_artist.artist_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }

            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){
                // 전체재생 - Rose Play 요청
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRoseAPI_withBugs::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생 - Rose Play 요청
                ProcRoseAPI_withBugs *procRosePlay = new ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRoseAPI_withBugs::PlayShuffleMode::ShufflePlay);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){
                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                this->makeObj_optMorePopup(OptMorePopup::Bugs_Artist, ConvertData_forBugs::getConvertOptHeaderData(data_artist), 0, section, true);
            }
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 자식 클래스에서 페이지 이동의 간편화. SubPage에 대해, PageInfo struct로 페이지 이동을 처리하기 위함.
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief AbstractBugsSubWidget::movePage_track_allView
     * @param data_pageInfo
     */
    void AbstractBugsSubWidget::movePage_track_allView(bugs::PageInfo_TrackAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_pageInfo_trackAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___TRACK_ALL_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);
    }



    /**
     * @brief movePage_pd_album_allView
     * @param data_pageInfo
     */
    void AbstractBugsSubWidget::movePage_pd_album_allView(bugs::PageInfo_PDAlbumAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_pageInfo_pd_albumAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___PD_ALBUM_ALL_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);
    }



    /**
     * @brief AbstractTidalSubWidget::movePage_artist_detailView
     * @param artist_id
     */
    void AbstractBugsSubWidget::movePage_artist_detailView(const QList<int>& list_artist_id, const QList<QString>& list_artist_name){
        Q_UNUSED(list_artist_name);
        if(list_artist_id.length() > 0){
            // 바로 첫번째 Artist 정보로 넘어감
            this->movePage_artist_detailView(list_artist_id.first());
        }
    }


    /**
     * @brief AbstractBugsSubWidget::movePage_artist_detailView
     * @param artist_id
     */
    void AbstractBugsSubWidget::movePage_artist_detailView(const int artist_id){
        if(artist_id > 0){
            // Artist Detail 페이지 진입 (id 만 넣어서 진입)
            bugs::ArtistItemData tmp_data_artist;
            tmp_data_artist.artist_id = artist_id;

            QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_artistData(tmp_data_artist);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ARTIST_DETAIL);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
    }


    /**
     * @brief Album ID로, Album Detail View 페이지 이동을 위한 처리
     * @param album_id
     */
    void AbstractBugsSubWidget::movePage_album_detailView(const int album_id){
        if(album_id > 0){
            // Album Detail 페이지 진입 (id 만 넣어서 진입)
            bugs::AlbumItemData tmp_data_album;
            tmp_data_album.album_id = album_id;

            QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_albumData(tmp_data_album);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_DETAIL);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
    }


    /**
     * @brief PD's Album ID로, PD's Album Detail View 페이지 이동을 위한 처리
     * @param es_album_id
     */
    void AbstractBugsSubWidget::movePage_pd_album_detailView(const int es_album_id){
        if(es_album_id > 0){
            // PD Album Detail 페이지 진입 (id 만 넣어서 진입)
            bugs::PD_AlbumItemData tmp_data_pd_album;
            tmp_data_pd_album.es_album_id = es_album_id;

            QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_pd_albumData(tmp_data_pd_album);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___PD_ALBUM_DETAIL);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
    }


    /**
     * @brief AbstractBugsSubWidget::movePage_my_album_detailView
     * @param data_my_album
     */
    void AbstractBugsSubWidget::movePage_my_album_detailView(bugs::MyAlbumItemData& data_my_album){
        if(data_my_album.playlist_id > 0){
            // My Album Detail 페이지 진입
            QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_my_albumData(data_my_album);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___MY_ALBUM_DETAIL);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
    }



    /**
     * @brief Video 영상 목록 페이지로 이동한다. (Track 과 관련된 영상목록)
     * @param track_id
     * @param track_title
     */
    void AbstractBugsSubWidget::movePage_video_allView_ofTrack(bugs::TrackItemData& data_track){
        if(data_track.track_id > 0){
            QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_trackData(data_track);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___VIDEO_ALL_VIEW___OF_TRACK);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
    }


    // MARK : about Filter Option Box -----------------------------------------------------------------------------------------------------

    /**
     * @brief My Collection 에서 사용하는 Filter Option Data 목록 반환함
     * @return
     */
    QList<bugs::FilterOptionData> AbstractBugsSubWidget::get_list_filterOptData_forMyCollection(){

        QList<bugs::FilterOptionData> list_output;

        bugs::FilterOptionData filterOpt_date;
        filterOpt_date.opt_code = QVariant::fromValue(ProcBugsAPI::DataOrderOption::DATE);
        filterOpt_date.opt_name = tr("Date added");
        list_output.append(filterOpt_date);

        bugs::FilterOptionData filterOpt_name;
        filterOpt_name.opt_code = QVariant::fromValue(ProcBugsAPI::DataOrderOption::NAME);
        filterOpt_name.opt_name = tr("A-Z");
        list_output.append(filterOpt_name);

        return list_output;

    }
}
