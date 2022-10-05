#include "AbstractAlbumImageDetailInfo_RHV.h"
#include "AbstractAlbumTrackDetailInfo_RHV.h"

#include "qobuz/ItemAlbum_forQobuz.h"
#include "tidal/AbstractItem.h"
#include "tidal/ProcCommon.h"
#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QScroller>


const QString ALBTAB_STEP_TRACK = "track";
const QString ALBTAB_STEP_CREDIT = "credit";


/**
 * @brief 생성자
 * @param parent
 */
AlbumImageDetailInfo_RHV::AlbumImageDetailInfo_RHV(ContentsUIType p_ContentsUIType, QWidget *parent) : AbstractImageDetailInfo_RHV(p_ContentsUIType, parent){

    this->setImageSizeAndCorner(400, 400, 4);

    QJsonArray *p_jsonArray_titlSub = new QJsonArray();
    QJsonObject sub1 { {"name", tr("Track")}, {"code", ALBTAB_STEP_TRACK} };
    QJsonObject sub2 { {"name", tr("Credit")}, {"code", ALBTAB_STEP_CREDIT} };

    p_jsonArray_titlSub->push_back(sub1);
    p_jsonArray_titlSub->push_back(sub2);

    this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::AlbumDetail);
    this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);

    connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI(QJsonObject)));

    this->contentStep = "track";
    this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
    this->menubar->hide();

    this->setUIControl_trackList();
    this->setUIControl_sameArtist();
}


AlbumImageDetailInfo_RHV::~AlbumImageDetailInfo_RHV(){

}


void AlbumImageDetailInfo_RHV::setUIControl_trackList(){

    this->vl_tracks = new QVBoxLayout();
    this->vl_tracks->setContentsMargins(0,0,0,0);
    this->vl_tracks->setSpacing(0);
    this->vl_tracks->setAlignment(Qt::AlignTop);

    this->vl_credit = new QVBoxLayout();
    this->vl_credit->setContentsMargins(0,0,0,0);
    this->vl_credit->setSpacing(0);
    this->vl_credit->setAlignment(Qt::AlignTop);

    this->widget_tab_tracks = new QWidget();
    this->widget_tab_tracks->hide();

    this->widget_tab_credit = new QWidget();
    this->widget_tab_credit->hide();

    this->widget_tab_tracks->setLayout(this->vl_tracks);
    this->widget_tab_credit->setLayout(this->vl_credit);

    this->stackedwidget = new QStackedWidget();
    this->stackedwidget->addWidget(this->widget_tab_tracks);
    this->stackedwidget->addWidget(this->widget_tab_credit);
    this->stackedwidget->hide();

    this->vbox_trackList = new QVBoxLayout();
    this->vbox_trackList->setSpacing(0);
    this->vbox_trackList->setContentsMargins(0, 0, 0, 0);
    this->vbox_trackList->setAlignment(Qt::AlignTop);
    this->vbox_trackList->addWidget(this->menubar);
    this->vbox_trackList->addWidget(this->stackedwidget);
}


void AlbumImageDetailInfo_RHV::setUIControl_sameArtist(){
    this->lb_subTitle = new QLabel();
    this->btnView_all = new QPushButton();

    this->vbox_contents = new QVBoxLayout();
    this->vbox_contents->setSpacing(0);
    this->vbox_contents->setContentsMargins(0, 0, 0, 0);
}


/**
 * @brief AlbumImageDetailInfo::setMyData
 * @param flagMine
 */
void AlbumImageDetailInfo_RHV::setMyData(bool flagMine){
    // 항상 부모 먼저 호출
    AbstractImageDetailInfo_RHV::setMyData(flagMine);

    this->btn_addCollect->setVisible(!flagMine);                // Add Collection 부분
}


/**
 * @brief AlbumImageDetailInfo::initView
 */
void AlbumImageDetailInfo_RHV::initView(){

    this->widget_Addbtn_Open->hide();
    this->menubar->hide();
    this->stackedwidget->hide();

    this->setImage("");
    this->label_titleUp->setText("");
    this->label_titleDown->setText("");
    this->label_artist->setText("");
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

        GSCommon::clearLayout(this->vl_credit);
        this->vl_credit->deleteLater();

        this->vbox_trackList->deleteLater();

        this->setUIControl_trackList();
    }
    GSCommon::clearLayout(this->vl_track_main);

    if(this->flag_sameArtist == true){
        this->lb_subTitle->setText("");
        this->btnView_all->setText("");

        GSCommon::clearLayout(this->hBox_sameArtist);
        this->hBox_sameArtist->deleteLater();

        this->vbox_contents->deleteLater();

        this->setUIControl_sameArtist();
    }
    GSCommon::clearLayout(this->vl_same_artist);
}

/**
 * @brief Bugs용 initView
 */
void AlbumImageDetailInfo_RHV::initView_forBugs(){
    this->setImage("");
    this->label_titleUp->setText("");
    this->label_titleDown->setText("");
    this->label_artist->setText("");
    this->label_description->setText("");
    this->setFavorite(false);
//    this->setMyData(false);

    // 사용하지 않음
    this->btn_addCollect->setVisible(false);
}


void AlbumImageDetailInfo_RHV::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText){

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


QHBoxLayout* AlbumImageDetailInfo_RHV::setUIControl_hBoxLayout_forAlbum(){
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
    tmp_scrollArea->setFixedHeight(275);

    QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
    //----------------------------------------------------------------------------------------------------  BODY : END

    QHBoxLayout *hBox_background = new QHBoxLayout();
    hBox_background->addWidget(tmp_scrollArea);

    QWidget *widget_background = new QWidget();
    widget_background->setLayout(hBox_background);
    widget_background->setContentsMargins(0,0,0,0);
    widget_background->setStyleSheet("background-color:#212121; border:0px;");

    // Apply Main Layout with spacing
    this->vbox_contents->addWidget(widget_background, 0, Qt::AlignTop);
    //this->box_main_contents->addSpacing(30);

    // return
    return hBox_album;
}



/**
 * @brief TIDAL 앨범 적용 관련
 * @param data_album
 */
void AlbumImageDetailInfo_RHV::setData_fromTidalData(const tidal::AlbumItemData &data_album){

    // Album 모드로 배치한다.
    this->label_artist->setVisible(true);
    this->label_creatorName->setVisible(false);

    // 데이터를 UI에 세팅한다.
    this->setImage(data_album.image);
    //this->label_title->setText(data_album.title);
    //this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);

    this->label_artist->setText(data_album.list_artist_name.join(","));
    this->label_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);


    this->data_key = data_album.id;
    this->setFavorite( this->arr_myFavoriteIds.contains(data_album.id) );       // 기본값

    // 항상 마지막에 한번 더 적용한다. 호출 순서가 다를 수 있으므로
    this->setMyData(this->flagMyData);
}

/**
 * @brief TIDAL 앨범 적용 관련
 * @param data_album
 */
void AlbumImageDetailInfo_RHV::setData_fromQobuzData_album(const qobuz::AlbumItemData &data_album){

    // Album 모드로 배치한다.
    this->label_artist->setVisible(true);
    this->label_creatorName->setVisible(false);

    // 데이터를 UI에 세팅한다.
    this->setImage(data_album.image_large);

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("color:#FFFFFF;font-size:50px;font-weight:500;");
    tmp_title->setText(data_album.title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if(title_width <= 950 &&
            (!data_album.title.contains("(") && !data_album.title.contains(":") && !data_album.title.contains("-") && !data_album.title.contains("&"))){
        if(data_album.hires){
            this->setHiRes_Left(title_width, 0);
        }
        this->label_titleUp->setText(data_album.title);
    }
    else{
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_title_line1 = "";
        QString tmp_title_line2 = "";
        QString tmp_title_total = "";

        tmp_split = data_album.title;
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
                this->label_titleUp->setText(data_album.title);
                if(data_album.hires){
                    this->setHiRes_Left(title_width, 1);
                }
            }
            else{
                this->label_titleUp->setText(tmp_title_line1);
                if(hires_left >= 950){
                    this->label_titleDown->setText(GSCommon::getTextCutFromLabelWidth(tmp_title_total, hires_left - 10, this->label_titleDown->font()));
                }
                else{
                    this->label_titleDown->setText(tmp_title_total);
                }
                if(data_album.hires){
                    this->setHiRes_Left(hires_left, 1);
                }
            }
        }
    }

    //this->label_artist->setText(data_album.list_artist_name.join(","));
    this->label_artist->setText(data_album.artist_name);

    QString str_resol = "";
    if(data_album.numberOfTracks != 0){
        if(global.lang == 0){
            if(data_album.numberOfTracks > 1){
                str_resol = QString("%1 tracks · ").arg(data_album.numberOfTracks);
            }
            else{
                str_resol = QString("%1 track · ").arg(data_album.numberOfTracks);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 · ").arg(data_album.numberOfTracks);
        }
    }
    else if(data_album.numberOfTracks == 0 && data_album.tracks_count != 0){
        if(global.lang == 0){
            if(data_album.tracks_count > 1){
                str_resol = QString("%1 tracks · ").arg(data_album.tracks_count);
            }
            else{
                str_resol = QString("%1 track · ").arg(data_album.tracks_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 · ").arg(data_album.tracks_count);
        }
    }

    if(data_album.duration >= 3600){
        str_resol += QString("%1 ").arg(QDateTime::fromTime_t(data_album.duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        str_resol += QString("%1 ").arg(QDateTime::fromTime_t(data_album.duration).toUTC().toString("mm:ss"));
    }

    if(data_album.hires){
        str_resol += QString("| FLAC %1kHz %2bit ").arg(data_album.maximum_sampling_rate).arg(data_album.maximum_bit_depth);
    }
    else{
        str_resol += QString("| %1kHz %2bit ").arg(data_album.maximum_sampling_rate).arg(data_album.maximum_bit_depth);
    }

    if(!data_album.release_date_stream.isEmpty()){

        QDate releaseDate = QDate::fromString(data_album.release_date_stream, "yyyy-MM-dd");

        if(global.lang == 0){
            QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
            str_resol += "| Release " + QLocale().toString(releaseDate, "MMM d, yyyy");

            QLocale::setDefault(QLocale::system());
        }
        else if(global.lang == 1){
            str_resol += "| 발매 " + releaseDate.toString("M월 d일, yyyy");
        }
    }

    this->label_resolution->setText(str_resol);

    QString setHTML = "";
    int height = 0;
    bool flag = false;
    if(!data_album.description.isEmpty()){
        setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(data_album.description);

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

    this->data_key = data_album.id;
    //this->setFavorite(this->arr_myFavoriteIds.contains(data_album.id) );       // 기본값
    this->setFavoritesIds(this->myFavoriteIds, this->myStarIds);

    // 항상 마지막에 한번 더 적용한다. 호출 순서가 다를 수 있으므로
    this->setMyData(this->flagMyData);
}

void AlbumImageDetailInfo_RHV::setData_fromQobuzData_track(const QList<qobuz::TrackItemData> &data_track, const QJsonArray &jsonArr_dataToPlay){

    Q_UNUSED(jsonArr_dataToPlay);
    GSCommon::clearLayout(this->vl_tracks);

    QList<qobuz::TrackItemData> *list_track = new QList<qobuz::TrackItemData>();
    list_track->append(data_track);

    if(list_track->count() > 0){
        int cd_num = 1;
        for(int i = 0; i < list_track->count(); i++){
            if(i == 0){
                this->widget_tracks_header = new QWidget();
                this->widget_tracks_header->setFixedSize(1500, 52);
                this->widget_tracks_header->setContentsMargins(0, 0, 0, 0);

                QLabel *label_cd_ico = GSCommon::getUILabelImg(":/images/qobuz/album_cd_ico.png", this->widget_tracks_header);
                label_cd_ico->setGeometry(0, 19, 30, 30);

                QLabel *label_cd_Name = new QLabel(this->widget_tracks_header);
                label_cd_Name->setStyleSheet("color:#FFFFFF;font-size:24px;font-weight:bold;");
                label_cd_Name->setGeometry(40, 16, 47, 36);
                label_cd_Name->setText("CD" + QString("%1").number(cd_num));

                this->vl_tracks->addWidget(this->widget_tracks_header);
            }
            else if(i != 0 && data_track.at(i).track_number == 1){
                cd_num += 1;

                this->widget_tracks_header = new QWidget();
                this->widget_tracks_header->setFixedSize(1500, 66);
                this->widget_tracks_header->setContentsMargins(0, 0, 0, 0);

                QLabel *label_cd_ico = GSCommon::getUILabelImg(":/images/qobuz/album_cd_ico.png", this->widget_tracks_header);
                label_cd_ico->setGeometry(0, 33, 30, 30);

                QLabel *label_cd_Name = new QLabel(this->widget_tracks_header);
                label_cd_Name->setStyleSheet("color:#FFFFFF;font-size:24px;font-weight:bold;");
                label_cd_Name->setGeometry(40, 30, 47, 36);
                label_cd_Name->setText("CD" + QString("%1").number(cd_num));

                this->vl_tracks->addWidget(this->widget_tracks_header);
            }

            if(i == 0){
                QWidget *widget_work = new QWidget();
                widget_work->setFixedSize(1470, 23);

                if(!list_track->at(i).work.isEmpty()){
                    widget_work->setFixedSize(1470, 47);

                    QLabel *label_work = new QLabel(widget_work);
                    label_work->setStyleSheet("color:#FFFFFF;font-size:16px;font-weight:normal;");                    
                    label_work->setFixedSize(1470, 24);
                    label_work->setGeometry(30, 23, 0, 0);
                    label_work->setText(list_track->at(i).work);
                }

                this->vl_tracks->addWidget(widget_work);
            }
            else{
                if(list_track->at(i-1).work != list_track->at(i).work){
                    QWidget *widget_work = new QWidget();
                    widget_work->setFixedSize(1470, 47);

                    if(!list_track->at(i).work.isEmpty()){
                        QLabel *label_work = new QLabel(widget_work);
                        label_work->setStyleSheet("color:#FFFFFF;font-size:16px;font-weight:normal;");
                        label_work->setFixedSize(1470, 24);
                        label_work->setGeometry(30, 23, 0, 0);
                        label_work->setText(list_track->at(i).work);
                    }

                    this->vl_tracks->addWidget(widget_work);
                }
            }

            this->albumTrackDetailInfo_RHV[i] = new AlbumTrackDetailInfo_RHV;
            connect(this->albumTrackDetailInfo_RHV[i], &AlbumTrackDetailInfo_RHV::clicked, this, &AlbumImageDetailInfo_RHV::slot_detailInfo_btnClicked);
            this->albumTrackDetailInfo_RHV[i]->setProperty("index", i);
            //this->albumTrackDetailInfo_RHV[i]->setDataTrackInfo_Qobuz(list_track->at(i));

            this->vl_tracks->addWidget(this->albumTrackDetailInfo_RHV[i]);
        }

        for(int j = 0; j < list_track->count(); j++){
            this->albumTrackDetailInfo_RHV[j]->setDataTrackInfo_Qobuz(list_track->at(j));
        }

        if(this->contentStep == "track"){
            this->widget_tab_tracks->show();
            this->stackedwidget->setFixedHeight(this->vl_tracks->sizeHint().height());
        }
        else{
            this->widget_tab_credit->show();
            this->stackedwidget->setFixedHeight(this->vl_credit->sizeHint().height());
        }

        //this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
        QJsonObject p_data;
        p_data.insert("pageCode", "track");
        slot_changedSubTabUI(p_data);

        this->vl_track_main->addLayout(this->vbox_trackList);
        this->flag_trackList = true;

        this->menubar->show();
        this->stackedwidget->show();

        emit complete_applying_track();
    }
}


void AlbumImageDetailInfo_RHV::setData_fromQobuzData_credit(const QList<qobuz::TrackItemData> &data_track, const QJsonArray &jsonArr_dataToPlay){

    Q_UNUSED(jsonArr_dataToPlay);
    //GSCommon::clearLayout(this->vl_tracks);

    QList<qobuz::TrackItemData> *list_track = new QList<qobuz::TrackItemData>();
    list_track->append(data_track);

    if(list_track->count() > 0){
        int cd_num = 1;
        for(int i = 0; i < list_track->count(); i++){
            if(i == 0){
                this->widget_credit_header = new QWidget();
                this->widget_credit_header->setFixedSize(1500, 52);
                this->widget_credit_header->setContentsMargins(0, 0, 0, 0);

                QLabel *label_cd1_ico = GSCommon::getUILabelImg(":/images/qobuz/album_cd_ico.png", this->widget_credit_header);
                label_cd1_ico->setGeometry(0, 19, 30, 30);

                QLabel *label_cd1_Name = new QLabel(this->widget_credit_header);
                label_cd1_Name->setStyleSheet("color:#FFFFFF;font-size:24px;font-weight:bold;");
                label_cd1_Name->setGeometry(40, 16, 47, 36);
                label_cd1_Name->setText("CD" + QString("%1").number(cd_num));

                this->vl_credit->addWidget(this->widget_credit_header);
            }
            else if(i != 0 && data_track.at(i).track_number == 1){
                cd_num += 1;

                this->widget_credit_header = new QWidget();
                this->widget_credit_header->setFixedSize(1500, 66);
                this->widget_credit_header->setContentsMargins(0, 0, 0, 0);

                QLabel *label_cd1_ico = GSCommon::getUILabelImg(":/images/qobuz/album_cd_ico.png", this->widget_credit_header);
                label_cd1_ico->setGeometry(0, 33, 30, 30);

                QLabel *label_cd1_Name = new QLabel(this->widget_credit_header);
                label_cd1_Name->setStyleSheet("color:#FFFFFF;font-size:24px;font-weight:bold;");
                label_cd1_Name->setGeometry(40, 30, 47, 36);
                label_cd1_Name->setText("CD" + QString("%1").number(cd_num));

                this->vl_credit->addWidget(this->widget_credit_header);
            }

            if(i == 0){
                QWidget *widget_work_credit = new QWidget();
                widget_work_credit->setFixedSize(1470, 23);

                if(!list_track->at(i).work.isEmpty()){
                    widget_work_credit->setFixedSize(1470, 47);

                    QLabel *label_work1 = new QLabel(widget_work_credit);
                    label_work1->setStyleSheet("color:#FFFFFF;font-size:16px;font-weight:normal;");
                    label_work1->setFixedSize(1470, 24);
                    label_work1->setGeometry(30, 23, 0, 0);
                    label_work1->setText(list_track->at(i).work);
                }

                this->vl_credit->addWidget(widget_work_credit);
            }
            else{
                if(list_track->at(i-1).work != list_track->at(i).work){
                    QWidget *widget_work_credit = new QWidget();
                    widget_work_credit->setFixedSize(1470, 47);

                    if(!list_track->at(i).work.isEmpty()){
                        QLabel *label_work1 = new QLabel(widget_work_credit);
                        label_work1->setStyleSheet("color:#FFFFFF;font-size:16px;font-weight:normal;");
                        label_work1->setFixedSize(1470, 24);
                        label_work1->setGeometry(30, 23, 0, 0);
                        label_work1->setText(list_track->at(i).work);
                    }

                    this->vl_credit->addWidget(widget_work_credit);
                }
            }

            this->albumTrackDetailInfoCredit_RHV[i] = new AlbumTrackDetailInfoCredit_RHV;
            connect(this->albumTrackDetailInfoCredit_RHV[i], &AlbumTrackDetailInfoCredit_RHV::clicked, this, &AlbumImageDetailInfo_RHV::slot_detailInfo_btnClicked);
            this->albumTrackDetailInfoCredit_RHV[i]->setProperty("index", i);
            //this->albumTrackDetailInfoCredit_RHV[i]->setDataTrackInfo_Qobuz(list_track->at(i));

            this->vl_credit->addWidget(this->albumTrackDetailInfoCredit_RHV[i]);
        }

        for(int j = 0; j < list_track->count(); j++){
            this->albumTrackDetailInfoCredit_RHV[j]->setDataTrackInfo_Qobuz(list_track->at(j));
        }

        if(this->contentStep == "track"){
            this->widget_tab_tracks->show();
            this->stackedwidget->setFixedHeight(this->vl_tracks->sizeHint().height());
        }
        else{
            this->widget_tab_credit->show();
            this->stackedwidget->setFixedHeight(this->vl_credit->sizeHint().height());
        }

        //this->menubar->setSelectedSubMenuNoSignal(this->contentStep);
        QJsonObject p_data;
        p_data.insert("pageCode", "track");
        slot_changedSubTabUI(p_data);

        this->vl_track_main->addLayout(this->vbox_trackList);
        this->flag_trackList = true;

        this->menubar->show();
        this->stackedwidget->show();
    }
}


void AlbumImageDetailInfo_RHV::setData_fromQobuzData_artist(const QList<qobuz::AlbumItemData> &data_album){

    this->setUIControl_subTitle_withSideBtn("By the same artist", tr("View All"));          // (" + QString("%1").number(data_album.count()) + ")", tr("View All"));
    this->hBox_sameArtist = this->setUIControl_hBoxLayout_forAlbum();

    if(data_album.count() > 0){
        int max_count = 0;

        if(data_album.size() < 15){
            max_count = data_album.size();
        }
        else{
            max_count = 15;
        }

        for(int i = 0 ; i < max_count ; i++){
            qobuz::ItemAlbum_qobuz *tmp_widget = new qobuz::ItemAlbum_qobuz(i, 3, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(tmp_widget, &qobuz::ItemAlbum_qobuz::signal_clicked, this, &AlbumImageDetailInfo_RHV::slot_clickedItemAlbum);
            tmp_widget->setData(data_album.at(i));
            this->hBox_sameArtist->addWidget(tmp_widget);
        }

        this->flag_sameArtist = true;
    }
    else{
        NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
        noData_widget->setFixedHeight(60);
        this->hBox_sameArtist->addWidget(noData_widget);

        this->flag_sameArtist = true;
    }

    this->vl_same_artist->addLayout(this->vbox_contents);
}


void AlbumImageDetailInfo_RHV::setData_fromRoseData(const roseHome::AlbumItemData &data_album){
    // Album 모드로 배치한다.
    this->label_artist->setVisible(true);
    this->label_creatorName->setVisible(false);

    // 데이터를 UI에 세팅한다.
    this->setImage(data_album.thumbnail);

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("color:#FFFFFF;font-size:50px;font-weight:500;");
    tmp_title->setText(data_album.title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if(title_width <= 950 &&
            (!data_album.title.contains("(") && !data_album.title.contains(":") && !data_album.title.contains("-") && !data_album.title.contains("&"))){

        this->label_titleUp->setText(data_album.title);
    }
    else{
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_title_line1 = "";
        QString tmp_title_line2 = "";
        QString tmp_title_total = "";

        tmp_split = data_album.title;
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
                    if((k + 1) == j)
                    {
                        tmp_title_line2 = splitToken.at(k);
                    }

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
                this->label_titleUp->setText(data_album.title);
            }
            else{
                this->label_titleUp->setText(tmp_title_line1);
                if(hires_left >= 950){
                    this->label_titleDown->setText(GSCommon::getTextCutFromLabelWidth(tmp_title_total, hires_left - 10, this->label_titleDown->font()));
                }
                else{
                    this->label_titleDown->setText(tmp_title_total);
                }
            }
        }
    }

    //this->label_artist->setText(data_album.list_artist_name.join(","));
    this->label_artist->setText("");

    QString str_resol = "";
    //str_resol = QString(tr("총") + " %1" + tr("개 트랙") + " ").arg(data_album.totalCount);
    if(global.lang == 0){
        if(data_album.totalCount > 1){
            str_resol = QString("%1 tracks · ").arg(data_album.totalCount);
        }
        else{
            str_resol = QString("%1 track · ").arg(data_album.totalCount);
        }
    }
    else if(global.lang == 1){
        str_resol = QString("총 %1개 트랙 · ").arg(data_album.totalCount);
    }

    this->label_resolution->setText(str_resol);

    QString setHTML = "";
    int height = 0;
    bool flag = false;
    setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(tr("There is no description for the album."));

    this->setOpen_Height(height, flag);
    this->label_description->setText(setHTML);

    this->widget_Addbtn_Open->setFixedSize(1500, 80);
    this->label_description->setGeometry(0, 0, 1450, 40);
    this->label_open->setGeometry(0, 37, 1450, 30);
    this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
    this->widget_Addbtn_Open->show();

    this->data_key = data_album.id;
    this->setFavoritesIds(this->myFavoriteIds, this->myStarIds);
}


/**
 * @brief Bugs 앨범 적용 관련
 * @param data_album
 */
void AlbumImageDetailInfo_RHV::setData_fromBugsData(const bugs::AlbumItemData &data_album){

    // Album 모드로 배치한다.
    this->label_artist->setVisible(true);
    this->label_creatorName->setVisible(false);
    this->updateDate_playlist_bioText(data_album.descr);
    // 데이터를 UI에 세팅한다.
    this->setImage(data_album.album_image);
    //this->label_title->setText(data_album.title);
    this->label_artist->setText(data_album.list_artist_nm.join(","));

    this->data_key = data_album.album_id;
    this->setFavorite(data_album.likes_yn);

    // 항상 마지막에 한번 더 적용한다. 호출 순서가 다를 수 있으므로
//    this->setMyData(this->flagMyData);

    this->btn_addCollect->setVisible(false);
}


//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : [Slot]
//
//-----------------------------------------------------------------------------------------------------------------------

void AlbumImageDetailInfo_RHV::slot_detailInfo_btnClicked(const int idx, const AlbumTrackDetailInfo_RHV::ClickMode clickMode){
    emit this->list_Clicked(idx, clickMode);
}


void AlbumImageDetailInfo_RHV::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

    int click_signal = clickMode;
    int index = ((tidal::AbstractItem*)sender())->index();
    int section = ((tidal::AbstractItem*)sender())->section();

    //emit this->album_Clicked(clickMode);
    emit this->sameArtist_Click(click_signal, index, section);
}


void AlbumImageDetailInfo_RHV::slot_clickBtn_subTitle_viewAll(){
    emit this->sameArtist_btnVeiwAll_Click(this->lb_subTitle->text());
}



void AlbumImageDetailInfo_RHV::changedOnlyTabUI_notSendSignal(QString p_step){

    this->contentStep = p_step;

    // Tab 타이틀 변경
    this->menubar->setSelectedSubMenuNoSignal(contentStep);

    // DB 가져와서 데이터 세팅
    if(contentStep == ALBTAB_STEP_TRACK){
        this->stackedwidget->setCurrentIndex(0);
        this->stackedwidget->setFixedHeight(this->vl_tracks->sizeHint().height());
    }else if(contentStep == ALBTAB_STEP_CREDIT){
        this->stackedwidget->setCurrentIndex(1);
        this->stackedwidget->setFixedHeight(this->vl_credit->sizeHint().height());
    }
}


void AlbumImageDetailInfo_RHV::slot_changedSubTabUI(const QJsonObject &p_data){

    QString tmp_step = p_data[KEY_PAGE_CODE].toString();

    // 페이지 변경 및 데이터 세팅
    this->changedOnlyTabUI_notSendSignal(tmp_step);

    // 시그널 발생 : 페이지가 변경되었다고 부모에게 알려준다.
    /*QJsonObject tmp_data = this->getDataJson();
    tmp_data[KEY_CONTENT_STEP] = tmp_step;
    emit clickedSubTab(tmp_data);*/
}


void AlbumImageDetailInfo_RHV::updateDate_playlist_bioText(const QString &descriptionText){//cheon210605

    this->playlist_descriptionText = this->getNormalText(descriptionText);  //cheon210605

    if(this->playlist_descriptionText.isEmpty()){
        this->label_description->setVisible(false);
    }
    else{

    }
}


/**
 * @brief AlbumImageDetailInfo::setFavoritesIds
 * @param p_arr_myFavoriteIds
 */
void AlbumImageDetailInfo_RHV::setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds){

    AbstractImageDetailInfo_RHV::setFavoritesIds_forTidal(p_arr_myFavoriteIds);

    if(!this->label_titleUp->text().isEmpty()){
        // 데이터가 세팅된 경우
        this->setFavorite( this->arr_myFavoriteIds.contains(data_key) );
    }
}


/**
 * @brief AlbumImageDetailInfo::setFavoritesIds
 * @param p_arr_myFavoriteIds
 */
void AlbumImageDetailInfo_RHV::setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds){

    AbstractImageDetailInfo_RHV::setFavoritesIds_forQobuz(p_arr_myFavoriteIds);

    if(!this->label_titleUp->text().isEmpty()){
        // 데이터가 세팅된 경우
        this->setFavorite(this->arr_myFavoriteIds.contains(data_key) );
    }
}

void AlbumImageDetailInfo_RHV::setFavorites_forQobuz(const bool flag){

    this->setFavorite(flag);
}

void AlbumImageDetailInfo_RHV::setFavoritesIds(bool p_myFavoriteIds, int p_star){
    AbstractImageDetailInfo_RHV::setFavoritesIds(p_myFavoriteIds, p_star);

    if(!this->label_titleUp->text().isEmpty() && !this->flagMyData){
        this->setFavorite(this->myFavoriteIds, this->myStarIds);
    }
}


bool AlbumImageDetailInfo_RHV::getFavoritesIds(){
    return this->myFavoriteIds;
}


int AlbumImageDetailInfo_RHV::getFavoritesStars(){
    return this->myStarIds;
}

