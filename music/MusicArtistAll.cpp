#include "music/MusicArtistAll.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/sqlitehelper.h"

#include "widget/NoData_Widget.h"
#include "widget/optionpopup.h"

#include <QResizeEvent>


namespace music {

    const int SECTION_FOR_MORE_POPUP___artists = 0;

    /**
     * @brief ArtistListAll::ArtistListAll
     * @param parent
     */
    ArtistListAll::ArtistListAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_artist = new QList<roseHome::ArtistItemData>();
        this->jsonArr_Artist = QJsonArray();

        // 기본 UI 세팅
        this->setUIControl_artists();
    }

    /**
     * @brief 소멸자.
     */
    ArtistListAll::~ArtistListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void ArtistListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(tmpTitle != this->curr_api_mainTitle){

            this->flagNeedReload = true;

            this->curr_api_mainTitle = tmpTitle;
            this->label_mainTitle->setText(this->curr_api_mainTitle);

            this->list_artist->clear();
            this->jsonArr_Artist = ProcJsonEasy::getJsonArray(jsonObj, "data");

            this->flag_flow_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void ArtistListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->flowLayout_artists);

//            QList<roseHome::ArtistItemData> list_artist;
//            for(int i = 0; i < this->jsonArr_Artist.size(); i++){

//                QJsonObject tmpObj = this->jsonArr_Artist.at(i).toObject();

//                QString tmpImg = ProcJsonEasy::getString(tmpObj, "album_art");
//                if(!tmpImg.isEmpty()){
//                    tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + ProcJsonEasy::getString(tmpObj, "album_art");
//                }

//                roseHome::ArtistItemData tmpArtist;
//                tmpArtist.id = ProcJsonEasy::getInt(tmpObj, "id");
//                tmpArtist.imageUrl = tmpImg;
//                tmpArtist.name = ProcJsonEasy::getString(tmpObj, "artist");
//                tmpArtist.albums_count = ProcJsonEasy::getInt(tmpObj, "number_of_albums");
//                tmpArtist.tracks_count = ProcJsonEasy::getInt(tmpObj, "number_of_tracks");

//                list_artist.append(tmpArtist);
//            }

//            this->slot_applyResult_artists(list_artist, QJsonArray(), true);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void ArtistListAll::setUIControl_artists(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("artists");

        // get widget width, right margin - by diskept j230317 start
        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemArtist_rosehome *listArtist = new roseHome::ItemArtist_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200);

        this->artist_widget_width = listArtist->get_fixedWidth();
        this->artist_widget_margin = listArtist->get_rightMargin();

        //qDebug() << "[Debug] MusicAlbum::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listArtist;
        //  get widget width, right margin - by diskept j230317 finish

        // layout for items
        this->flowLayout_artists = this->get_addUIControl_flowLayout(0, 20);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] [Override]
     * @param list_data
     */
    void ArtistListAll::slot_applyResult_artists(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){

            int start_index = this->list_artist->size();
            this->list_artist->append(list_data);

            if(start_index == 0){
                int max_cnt = this->list_artist->size();

                for(int i = start_index; i < max_cnt; i++){
                    this->listArtist_all[i] = new roseHome::ItemArtist_rosehome(i, SECTION_FOR_MORE_POPUP___artists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listArtist_all[i]->setData(this->list_artist->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->flowLayout_artists->addWidget(this->listArtist_all[i]);
                    connect(this->listArtist_all[i], &roseHome::ItemArtist_rosehome::signal_clicked, this, &ArtistListAll::slot_clickedItemArtist);
                }

                ContentLoadingwaitingMsgHide();
            }


        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->flowLayout_artists->addWidget(noData_widget);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - Itemartist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void ArtistListAll::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        if(section == SECTION_FOR_MORE_POPUP___artists){

            // ClickMode 별로 처리
            roseHome::ArtistItemData data_artist = this->list_artist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                SqliteHelper *sqlite = new SqliteHelper(this);
                QSqlError err = sqlite->addConnectionRose();

                QList<DataPopup*> *dataPopupList = new QList<DataPopup*>();
                if(err.type() == QSqlError::NoError){
                    QString strQuery = "";
                    strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
                    strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
                    strQuery += " WHERE A.artist_id="+QString::number(data_artist.id);

                    QVariantList data;
                    sqlite->exec(strQuery, data);

                    for(int i = 0; i < data.size(); i++){
                        DataPopup *tmp_dataPopup = new DataPopup(this);
                        tmp_dataPopup->setJsonData(data.at(i).toJsonObject());
                        dataPopupList->append(tmp_dataPopup);
                    }
                }

                sqlite->close();
                delete sqlite;

                QJsonArray jsonArrAlbum;
                for(int i = 0; i < dataPopupList->size(); i++){
                    jsonArrAlbum.append(dataPopupList->at(i)->getJsonData());
                }

                // Artist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TITLE_MAIN, data_artist.name);
                jsonData.insert(KEY_OP_TITLE_SUB, "");
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Home_Artist);
                jsonData.insert(KEY_OP_albumImg, data_artist.imageUrl);
                jsonData.insert(KEY_OP_cntStar, 0);
                jsonData.insert(KEY_OP_DATA, jsonArrAlbum);
                jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);


                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    void ArtistListAll::resizeEvent(QResizeEvent *event){//c230304

        Q_UNUSED(event);

        //qDebug() << "[Debug] ArtistListAll::resizeEvent " << this->width();

        // flowlayout spacing change - by diskept j230317 start
        this->setFlowLayoutResize(this, this->flowLayout_artists, this->artist_widget_width, this->artist_widget_margin);
        // flowlayout spacing change - by diskept j230317 finish

        if(this->flag_flow_draw == false){
            this->flag_flow_draw = true;

            QList<roseHome::ArtistItemData> list_artist;
            for(int i = 0; i < this->jsonArr_Artist.size(); i++){

                QJsonObject tmpObj = this->jsonArr_Artist.at(i).toObject();

                QString tmpImg = ProcJsonEasy::getString(tmpObj, "album_art");
                if(!tmpImg.isEmpty()){
                    tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + ProcJsonEasy::getString(tmpObj, "album_art");
                }

                roseHome::ArtistItemData tmpArtist;
                tmpArtist.id = ProcJsonEasy::getInt(tmpObj, "id");
                tmpArtist.imageUrl = tmpImg;
                tmpArtist.name = ProcJsonEasy::getString(tmpObj, "artist");
                tmpArtist.albums_count = ProcJsonEasy::getInt(tmpObj, "number_of_albums");
                tmpArtist.tracks_count = ProcJsonEasy::getInt(tmpObj, "number_of_tracks");

                list_artist.append(tmpArtist);
            }

            this->slot_applyResult_artists(list_artist, QJsonArray(), true);
        }
    }
}
