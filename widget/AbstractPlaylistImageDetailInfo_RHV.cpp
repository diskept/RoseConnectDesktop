#include "AbstractPlaylistImageDetailInfo_RHV.h"
#include "AbstractPlaylistTrackDetailInfo_RHV.h"

#include "qobuz/ItemPlaylist_forQobuz.h"
#include "tidal/AbstractItem.h"
#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QScroller>

/**
 * @brief 생성자
 * @param parent
 */
PlaylistImageDetailInfo_RHV::PlaylistImageDetailInfo_RHV(ContentsUIType p_ContentsUIType, QWidget *parent) : AbstractImageDetailInfo_RHV(p_ContentsUIType, parent){

    if(p_ContentsUIType == Qobuz_playlist){
        this->setImageSizeAndCorner(384, 186, 4);
    }
    else{
        this->setImageSizeAndCorner(400, 400, 4);
    }

    this->setUIControl_trackList();
    this->setUIControl_similarPlaylists();
}


PlaylistImageDetailInfo_RHV::~PlaylistImageDetailInfo_RHV(){

    this->deleteLater();
}

/**
 * @brief PlaylistImageDetailInfo::setUIControl_trackList
 */
void PlaylistImageDetailInfo_RHV::setUIControl_trackList(){

    this->vl_tracks = new QVBoxLayout();
    this->vl_tracks->setContentsMargins(0,0,0,0);
    this->vl_tracks->setSpacing(0);
    this->vl_tracks->setAlignment(Qt::AlignTop);

    this->widget_tab_tracks = new QWidget();
    this->widget_tab_tracks->setLayout(this->vl_tracks);
    this->widget_tab_tracks->hide();

    this->stackedwidget = new QStackedWidget();
    this->stackedwidget->addWidget(this->widget_tab_tracks);
    this->stackedwidget->hide();

    this->vbox_trackList = new QVBoxLayout();
    this->vbox_trackList->setSpacing(0);
    this->vbox_trackList->setContentsMargins(0, 0, 0, 0);
    this->vbox_trackList->setAlignment(Qt::AlignTop);
    this->vbox_trackList->addWidget(this->stackedwidget);
}


/**
 * @brief PlaylistImageDetailInfo::setUIControl_similarPlaylists
 */
void PlaylistImageDetailInfo_RHV::setUIControl_similarPlaylists(){

    this->lb_subTitle = new QLabel();
    this->btnView_all = new QPushButton();

    this->vbox_contents = new QVBoxLayout();
    this->vbox_contents->setSpacing(0);
    this->vbox_contents->setContentsMargins(0, 0, 0, 0);
}


/**
 * @brief PlaylistImageDetailInfo::setMyData
 * @param flagMine
 */
void PlaylistImageDetailInfo_RHV::setMyData(bool flagMine){
    // 항상 부모 먼저 호출
    AbstractImageDetailInfo_RHV::setMyData(flagMine);

    this->btn_addCollect->setVisible(!flagMine);                // Add Collection 부분
}


/**
 * @brief PlaylistImageDetailInfo::initView
 */
void PlaylistImageDetailInfo_RHV::initView(){

    this->widget_Addbtn_Open->hide();
    this->stackedwidget->hide();

    this->setImage("");
    this->label_titleUp->setText("");
    this->label_titleDown->setText("");
    this->label_creatorName->setText("");
    this->label_resolution->setText("");
    this->label_description->setText("");
    this->setFavorite(false);
    //this->setMyData(false);
    this->label_imageHiRes->hide();

    if(this->flagBtnOpen == true){
        this->flagBtnOpen = false;
    }

    if(this->flag_trackList == true){
        GSCommon::clearLayout(this->vl_tracks);
        this->vl_tracks->deleteLater();

        this->vbox_trackList->deleteLater();

        this->setUIControl_trackList();
    }
    GSCommon::clearLayout(this->vl_track_main);

    if(this->flag_similarPlaylist == true){
        this->lb_subTitle->setText("");
        this->btnView_all->setText("");

        GSCommon::clearLayout(this->hBox_similarPlaylists);
        this->hBox_similarPlaylists->deleteLater();

        this->vbox_contents->deleteLater();

        this->setUIControl_similarPlaylists();
    }
    GSCommon::clearLayout(this->vl_same_artist);
}


void PlaylistImageDetailInfo_RHV::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText){

    // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
    QHBoxLayout *tmp_hBox = new QHBoxLayout();
    tmp_hBox->setSpacing(0);
    tmp_hBox->setContentsMargins(0,0,0,0);
    tmp_hBox->setAlignment(Qt::AlignTop);

    QWidget *widget_box_subTitle = new QWidget();
    widget_box_subTitle->setContentsMargins(10,5,5,0);
    widget_box_subTitle->setLayout(tmp_hBox);

    this->lb_subTitle = new QLabel();
    this->lb_subTitle->setText(subTitle);
    this->lb_subTitle->setStyleSheet("font-size:24px; color:#FFFFFF; font-weight:bold");
    tmp_hBox->addWidget(this->lb_subTitle,0, Qt::AlignVCenter);

    // side button - it works when the button text is not empty
    if(btnText.isEmpty() == false){
        this->btnView_all = new QPushButton();
        this->btnView_all->setText(btnText);
        this->btnView_all->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
        this->btnView_all->setCursor(Qt::PointingHandCursor);
        connect(this->btnView_all, SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

        tmp_hBox->addWidget(this->btnView_all, 1, Qt::AlignVCenter | Qt::AlignRight);
    }

    this->vbox_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);
}


QHBoxLayout* PlaylistImageDetailInfo_RHV::setUIControl_hBoxLayout_forPlaylists(){

    //----------------------------------------------------------------------------------------------------
    QHBoxLayout *hBox_playlist = new QHBoxLayout();
    hBox_playlist->setSpacing(0);
    hBox_playlist->setContentsMargins(0,0,0,0);
    hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

    QWidget *widget_contentSub = new QWidget();
    widget_contentSub->setLayout(hBox_playlist);
    widget_contentSub->setContentsMargins(0,0,0,0);


    QScrollArea *tmp_scrollArea = new QScrollArea();
    tmp_scrollArea->setWidget(widget_contentSub);
    tmp_scrollArea->setWidgetResizable(false);
    tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
    tmp_scrollArea->setContentsMargins(0,0,0,0);
    tmp_scrollArea->setFixedHeight(320);

    QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
    //----------------------------------------------------------------------------------------------------

    QHBoxLayout *hBox_background = new QHBoxLayout();
    hBox_background->addWidget(tmp_scrollArea);

    QWidget *widget_background = new QWidget();
    widget_background->setLayout(hBox_background);
    widget_background->setContentsMargins(0,0,0,0);
    widget_background->setStyleSheet("background-color:#212121; border:0px;");

    // Apply Main Layout with spacing
    this->vbox_contents->addWidget(widget_background, 0, Qt::AlignTop);

    // return
    return hBox_playlist;
}


void PlaylistImageDetailInfo_RHV::setData_fromQobuzData(const qobuz::PlaylistItemData &data_playlist){

    // Playlist 모드로 배치한다.
    this->label_artist->setVisible(false);
    this->label_creatorName->setVisible(true);

    // 데이터를 UI에 세팅한다.
    this->setImage(data_playlist.image);

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("color:#FFFFFF;font-size:50px;font-weight:500;");
    tmp_title->setText(data_playlist.title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if(title_width <= 950 &&
            (!data_playlist.title.contains("(") && !data_playlist.title.contains(":") && !data_playlist.title.contains("-") && !data_playlist.title.contains("&"))){

        this->label_titleUp->setText(data_playlist.title);
    }
    else{
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_title_line1 = "";
        QString tmp_title_line2 = "";
        QString tmp_title_total = "";

        tmp_split = data_playlist.title;
        splitToken = tmp_split.split(" ");

        tmp_title->setText("");
        int i, j, k = 0;
        if(splitToken.size() > 1){
            for(i = 0; i < splitToken.count(); i++){
                tmp_title_line1 += splitToken.at(i) + " ";
                tmp_title->setText(tmp_title_line1);

                if(tmp_title->sizeHint().width() > 950 || splitToken.at(i).contains("(") ||
                        splitToken.at(i).contains(":") || splitToken.at(i).contains("-") || splitToken.at(i).contains("&")){

                    if(splitToken.at(i).contains(":") && splitToken.at(i).count() > 1){
                        i++;
                    }

                    tmp_title->setText("");
                    tmp_title_line1 = "";
                    break;
                }
            }

            for(j = i; j < splitToken.count(); j++){
                if(j + 1 == splitToken.count())
                {
                    tmp_title_line2 += splitToken.at(j);
                    tmp_title->setText(tmp_title_line2);

                    tmp_title->setText("");
                    tmp_title_line2 = "";
                }
                else if(!splitToken.at(j).isEmpty()){
                    tmp_title_line2 += splitToken.at(j) + " ";
                    tmp_title->setText(tmp_title_line2);
                }

                if(tmp_title->sizeHint().width() > 950)
                {
                    tmp_title->setText("");
                    tmp_title_line2 = "";
                    break;
                }
            }

            for(k = 0; k < splitToken.count(); k++){
                if((k + 1) == splitToken.count()){
                    tmp_title_total += splitToken.at(k);

                    if((k + 1) == j){
                        tmp_title_line2 += splitToken.at(k);
                    }
                }
                else{
                    if(k < i){
                        tmp_title_line1 += splitToken.at(k) + " ";
                    }
                    else if((k >= i) && ((k + 1) <= j)){
                        if((k + 1) == j){
                            tmp_title_line2 += splitToken.at(k);
                        }
                        else{
                            tmp_title_line2 += splitToken.at(k) + " ";
                        }

                        tmp_title_total += splitToken.at(k) + " ";

                    }
                    else{
                        tmp_title_total += splitToken.at(k);
                    }
                }
            }

            int hires_left = 0;
            tmp_title->setText("");
            tmp_title->setText(tmp_title_line2);
            hires_left = tmp_title->sizeHint().width() - 10;

            //qDebug() << tmp_title->sizeHint().width() << ", line1 : " << tmp_title_line1;
            //qDebug() << tmp_title->sizeHint().width() << ", line2 : " << tmp_title_line2;
            //qDebug() << tmp_title->sizeHint().width() << ", hires : " << tmp_title->text();

            tmp_title->setText("");
            tmp_title->setText(tmp_title_total);

            //qDebug() << tmp_title->sizeHint().width() << ", title : " << tmp_title->text();

            title_width = tmp_title->sizeHint().width();
            if(tmp_title_line2.isEmpty()){
                this->label_titleUp->setText(data_playlist.title);
            }
            else{
                this->label_titleUp->setText(tmp_title_line1);
                if(hires_left >= 950){
                    this->label_titleDown->setText(GSCommon::getTextCutFromLabelWidth(tmp_title_total, hires_left - 10, this->label_titleDown->font()));
                    if(this->label_titleDown->text().contains("…")){
                        this->label_titleDown->setToolTip(tmp_title_total);//c230321
                        this->label_titleDown->setToolTipDuration(2000);//c230321
                    }
                }
                else{
                    this->label_titleDown->setText(tmp_title_total);
                }
            }
        }
    }

    this->label_creatorName->setText(data_playlist.ownerName);

    QString str_resol = "";
    //str_resol = QString("총 %1개 트랙 · ").arg(data_playlist.tracks_count);
    if(global.lang == 0){
        if(data_playlist.tracks_count > 1){
            str_resol = QString("%1 tracks · ").arg(data_playlist.tracks_count);
        }
        else{
            str_resol = QString("%1 track · ").arg(data_playlist.tracks_count);
        }
    }
    else if(global.lang == 1){
        str_resol = QString("총 %1개 트랙 · ").arg(data_playlist.tracks_count);
    }

    if(data_playlist.duration >= 3600){
        str_resol += QString("%1 ").arg(QDateTime::fromTime_t(data_playlist.duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        str_resol += QString("%1 ").arg(QDateTime::fromTime_t(data_playlist.duration).toUTC().toString("mm:ss"));
    }

    if(!data_playlist.lastUpdated.isEmpty()){
        if(global.lang == 0){
            QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
            QDate tmpDate = QDateTime::fromTime_t(data_playlist.lastUpdated.toInt()).toUTC().date();
            str_resol += "| Last updated " + QLocale().toString(tmpDate, "MMM d, yyyy");

            QLocale::setDefault(QLocale::system());
        }
        else if(global.lang == 1){
            str_resol += QString("| Last updated %1").arg(QDateTime::fromTime_t(data_playlist.lastUpdated.toInt()).toUTC().toString("M월 d일, yyyy"));
        }
    }

    this->label_resolution->setText(str_resol);

    QString setHTML = "";
    int height = 0;
    bool flag = false;
    if(!data_playlist.description.isEmpty()){
        setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(data_playlist.description);

        QLabel *tmp_html = new QLabel();
        tmp_html->setTextFormat(Qt::RichText);
        tmp_html->setWordWrap(true);
        tmp_html->setFixedWidth(1450);
        tmp_html->setText(setHTML);

        height = tmp_html->sizeHint().height();
        flag = true;
    }
    else{
        setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(tr("There is no description for the album."));
    }

    this->setOpen_Height(height, flag);
    this->label_description->setText(setHTML);

    this->widget_Addbtn_Open->setFixedSize(1500, 80);
    this->label_description->setGeometry(0, 0, 1450, 40);
    this->label_open->setGeometry(0, 37, 1450, 30);
    this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
    this->widget_Addbtn_Open->show();

    this->data_key = data_playlist.id;
    //this->setFavorite(this->arr_myFavoriteIds.contains(data_playlist.id));       // 기본값
    this->setFavoritesIds(this->myFavoriteIds, this->myStarIds);

    // 항상 마지막에 한번 더 적용한다. 호출 순서가 다를 수 있으므로
    this->setMyData(this->flagMyData);
}


void PlaylistImageDetailInfo_RHV::setData_fromQobuzData(const QList<qobuz::TrackItemData> &data_track, const QJsonArray &jsonArr_dataToPlay){

    Q_UNUSED(jsonArr_dataToPlay);
    GSCommon::clearLayout(this->vl_tracks);

    QList<qobuz::TrackItemData> *list_track = new QList<qobuz::TrackItemData>();
    list_track->append(data_track);

    if(list_track->count() > 0){
        for(int i = 0; i < list_track->count(); i++){

            PlaylistTrackDetailInfo_RHV *tmp_track_info = new PlaylistTrackDetailInfo_RHV();
            connect(tmp_track_info, &PlaylistTrackDetailInfo_RHV::clicked, this, &PlaylistImageDetailInfo_RHV::slot_detailInfo_btnClicked);
            tmp_track_info->setProperty("index", i);
            tmp_track_info->setDataTrackInfo_Qobuz(list_track->at(i));

            this->vl_tracks->addWidget(tmp_track_info);
        }

        this->widget_tab_tracks->show();
        this->stackedwidget->setFixedHeight(this->vl_tracks->sizeHint().height());

        this->vl_track_main->addLayout(this->vbox_trackList);
        this->flag_trackList = true;

        this->stackedwidget->show();
    }
}


void PlaylistImageDetailInfo_RHV::setData_fromQobuzData(const QList<qobuz::PlaylistItemData> &data_playlist){
    this->setUIControl_subTitle_withSideBtn("Playlist suggestioins", tr("View All"));          // (" + QString("%1").number(data_playlist.count()) + ")", tr("View All"));
    this->hBox_similarPlaylists = this->setUIControl_hBoxLayout_forPlaylists();

    if(data_playlist.count() > 0){
        int max_count = 0;

        if(data_playlist.size() < 15){
            max_count = data_playlist.size();
        }
        else{
            max_count = 15;
        }

        for(int i = 0 ; i < max_count ; i++){
            qobuz::ItemPlaylist_qobuz *tmp_widget = new qobuz::ItemPlaylist_qobuz(i, 3, tidal::AbstractItem::ImageSizeMode::Ractangle_360x176);
            connect(tmp_widget, &qobuz::ItemPlaylist_qobuz::signal_clicked, this, &PlaylistImageDetailInfo_RHV::slot_clickedItemPlaylist);
            tmp_widget->setData(data_playlist.at(i));
            this->hBox_similarPlaylists->addWidget(tmp_widget);
        }

        this->flag_similarPlaylist = true;
    }
    else{
        NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
        noData_widget->setFixedHeight(60);
        this->hBox_similarPlaylists->addWidget(noData_widget);

        this->flag_similarPlaylist = true;
    }

    this->vl_same_artist->addLayout(this->vbox_contents);
}


void PlaylistImageDetailInfo_RHV::setData_fromRoseData(const roseHome::PlaylistItemData &data_playlist){
    // Playlist 모드로 배치한다.
    this->label_artist->setVisible(false);
    this->label_creatorName->setVisible(true);

    // 데이터를 UI에 세팅한다.
    this->setImage(data_playlist.thumbnail);

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("color:#FFFFFF;font-size:50px;font-weight:500;");
    tmp_title->setText(data_playlist.title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if((title_width <= 950) ||
            (!data_playlist.title.contains("(") && !data_playlist.title.contains(":") && !data_playlist.title.contains("-") && !data_playlist.title.contains("&"))){

        this->label_titleUp->setText(data_playlist.title);
    }
    else{
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_title_line1 = "";
        QString tmp_title_line2 = "";
        QString tmp_title_total = "";

        tmp_split = data_playlist.title;
        splitToken = tmp_split.split(" ");

        tmp_title->setText("");
        int i, j, k = 0;
        if(splitToken.size() > 1){
            for(i = 0; i < splitToken.count(); i++){
                tmp_title_line1 += splitToken.at(i) + " ";
                tmp_title->setText(tmp_title_line1);

                if(tmp_title->sizeHint().width() > 950 || splitToken.at(i).contains("(") ||
                        splitToken.at(i).contains(":") || splitToken.at(i).contains("-") || splitToken.at(i).contains("&")){

                    if(splitToken.at(i).contains(":") && splitToken.at(i).count() > 1){
                        i++;
                    }

                    tmp_title->setText("");
                    tmp_title_line1 = "";
                    break;
                }
            }

            for(j = i; j < splitToken.count(); j++){
                if(j + 1 == splitToken.count())
                {
                    tmp_title_line2 += splitToken.at(j);
                    tmp_title->setText(tmp_title_line2);

                    tmp_title->setText("");
                    tmp_title_line2 = "";
                }
                else if(!splitToken.at(j).isEmpty()){
                    tmp_title_line2 += splitToken.at(j) + " ";
                    tmp_title->setText(tmp_title_line2);
                }

                if(tmp_title->sizeHint().width() > 950)
                {
                    tmp_title->setText("");
                    tmp_title_line2 = "";
                    break;
                }
            }

            for(k = 0; k < splitToken.count(); k++){
                if((k + 1) == splitToken.count()){
                    tmp_title_total += splitToken.at(k);
                }
                else{
                    if(k < i){
                        tmp_title_line1 += splitToken.at(k) + " ";
                    }
                    else if((k >= i) && ((k + 1) <= j)){
                        if((k + 1) == j){
                            tmp_title_line2 += splitToken.at(k);
                        }
                        else{
                            tmp_title_line2 += splitToken.at(k) + " ";
                        }

                        tmp_title_total += splitToken.at(k) + " ";

                    }
                    else{
                        tmp_title_total += splitToken.at(k);
                    }
                }
            }

            int hires_left = 0;
            tmp_title->setText("");
            tmp_title->setText(tmp_title_line2);
            hires_left = tmp_title->sizeHint().width() - 10;

            tmp_title->setText("");
            tmp_title->setText(tmp_title_total);

            title_width = tmp_title->sizeHint().width();
            if(tmp_title_line2.isEmpty()){
                this->label_titleUp->setText(data_playlist.title);
            }
            else{
                this->label_titleUp->setText(tmp_title_line1);
                if(hires_left >= 950){
                    this->label_titleDown->setText(GSCommon::getTextCutFromLabelWidth(tmp_title_total, hires_left - 10, this->label_titleDown->font()));
                    if(this->label_titleDown->text().contains("…")){
                        this->label_titleDown->setToolTip(tmp_title_total);//c230321
                        this->label_titleDown->setToolTipDuration(2000);//c230321
                    }
                }
                else{
                    this->label_titleDown->setText(tmp_title_total);
                }
            }
        }
    }

    if(data_playlist.ownerName == global.user.getUsername()){
        this->label_creatorName->setText(data_playlist.share);
    }
    else{
        this->label_creatorName->setText(data_playlist.ownerName);
    }

    QString str_resol = "";
    //str_resol = QString(tr("총") + " %1" + tr("개 트랙") + " ").arg(data_playlist.trackCount);
    if(global.lang == 0){
        if(data_playlist.trackCount > 1){
            str_resol = QString("%1 tracks · ").arg(data_playlist.trackCount);
        }
        else{
            str_resol = QString("%1 track · ").arg(data_playlist.trackCount);
        }
    }
    else if(global.lang == 1){
        str_resol = QString("총 %1개 트랙 · ").arg(data_playlist.trackCount);
    }

    if(!data_playlist.playTime.isEmpty()){
        /*
        if(data_playlist.playTime >= 3600){
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(data_playlist.duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(data_playlist.duration).toUTC().toString("mm:ss"));
        }
        */
    }

    if(!data_playlist.registDateTime.isEmpty()){
        QString tmp_split = "";
        QStringList splitToken;

        tmp_split = data_playlist.registDateTime;
        splitToken = tmp_split.split("T");

        QDate lastupdated = QDate::fromString(splitToken.at(0), "yyyy-MM-dd");

        if(global.lang == 0){
            QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
            str_resol += "| Registration date " + QLocale().toString(lastupdated, "MMM d, yyyy");

            QLocale::setDefault(QLocale::system());
        }
        else if(global.lang == 1){
            str_resol += "| Last updated " + lastupdated.toString("M월 d일, yyyy");
        }
    }

    this->label_resolution->setText(str_resol);

    QString setHTML = "";
    int height = 0;
    bool flag = false;
    if(!data_playlist.comment.isEmpty()){
        setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(data_playlist.comment);

        QLabel *tmp_html = new QLabel();
        tmp_html->setTextFormat(Qt::RichText);
        tmp_html->setWordWrap(true);
        tmp_html->setFixedWidth(1450);
        tmp_html->setText(setHTML);

        height = tmp_html->sizeHint().height();
        flag = true;
    }
    else{
        setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(tr("There is no description for the album."));
    }

    this->setOpen_Height(height, flag);
    this->label_description->setText(setHTML);

    this->widget_Addbtn_Open->setFixedSize(1500, 80);
    this->label_description->setGeometry(0, 0, 1450, 40);
    this->label_open->setGeometry(0, 37, 1450, 30);
    this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
    this->widget_Addbtn_Open->show();

    this->data_key = data_playlist.id;
    //this->setFavorite(this->arr_myFavoriteIds.contains(data_playlist.id));       // 기본값
    this->setFavoritesIds(this->myFavoriteIds, this->myStarIds);

    // 항상 마지막에 한번 더 적용한다. 호출 순서가 다를 수 있으므로
    //this->setMyData(this->flagMyData);
}


void PlaylistImageDetailInfo_RHV::setData_fromRoseData(const QList<roseHome::TrackItemData> &data_track, const QJsonArray&){

    GSCommon::clearLayout(this->vl_tracks);

    QList<roseHome::TrackItemData> *list_track = new QList<roseHome::TrackItemData>();
    list_track->append(data_track);

    if(list_track->count() > 0){
        for(int i = 0; i < list_track->count(); i++){

            PlaylistTrackDetailInfo_RHV *tmp_track_info = new PlaylistTrackDetailInfo_RHV();
            connect(tmp_track_info, &PlaylistTrackDetailInfo_RHV::clicked, this, &PlaylistImageDetailInfo_RHV::slot_detailInfo_btnClicked);
            tmp_track_info->setProperty("index", i);
            tmp_track_info->setDataTrackInfo_Rose(list_track->at(i));

            this->vl_tracks->addWidget(tmp_track_info);
        }

        this->widget_tab_tracks->show();
        this->stackedwidget->setFixedHeight(this->vl_tracks->sizeHint().height());

        this->vl_track_main->addLayout(this->vbox_trackList);
        this->flag_trackList = true;

        this->stackedwidget->show();
    }
}


//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : [Slot]
//
//-----------------------------------------------------------------------------------------------------------------------

void PlaylistImageDetailInfo_RHV::slot_detailInfo_btnClicked(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){
    emit this->list_Clicked(idx, clickMode);
}


void PlaylistImageDetailInfo_RHV::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){
    int click_signal = clickMode;
    int index = ((tidal::AbstractItem*)sender())->index();
    int section = ((tidal::AbstractItem*)sender())->section();

    //emit this->album_Clicked(clickMode);
    emit this->similarPlaylist_Click(click_signal, index, section);
}


void PlaylistImageDetailInfo_RHV::slot_clickBtn_subTitle_viewAll(){
    emit this->similarPlaylist_btnVeiwAll_Click(this->lb_subTitle->text());
}


void PlaylistImageDetailInfo_RHV::setFavoritesIds(bool p_myFavoriteIds, int p_star){
    AbstractImageDetailInfo_RHV::setFavoritesIds(p_myFavoriteIds, p_star);

    if(!this->label_titleUp->text().isEmpty() && !this->flagMyData){
        this->setFavorite(this->myFavoriteIds, this->myStarIds);
    }
}


bool PlaylistImageDetailInfo_RHV::getFavoritesIds(){
    return this->myFavoriteIds;
}


int PlaylistImageDetailInfo_RHV::getFavoritesStars(){
    return this->myStarIds;
}
