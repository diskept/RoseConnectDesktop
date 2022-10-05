#include "AlbumImageDetailInfo.h"
#include "tidal/ProcCommon.h"
#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"


/**
 * @brief 생성자
 * @param parent
 */
AlbumImageDetailInfo::AlbumImageDetailInfo(QWidget *parent) : AbstractImageDetailInfo(parent){
    this->setImageSizeAndCorner(300, 300, 4);
}




/**
 * @brief PlaylistImageDetailInfo::setMyData
 * @param flagMine
 */
void AlbumImageDetailInfo::setMyData(bool flagMine){
    // 항상 부모 먼저 호출
    AbstractImageDetailInfo::setMyData(flagMine);

    this->btn_addCollect->setVisible(!flagMine);                // Add Collection 부분
}


/**
 * @brief AlbumImageDetailInfo::initView
 */
void AlbumImageDetailInfo::initView(){
    this->setImage("");
    this->label_title->setText("");
    this->label_artist->setText("");
    this->elidedLabel_description->setText("");//cheon210605
    this->setFavorite(false);
    this->setMyData(false);
}

/**
 * @brief Bugs용 initView
 */
void AlbumImageDetailInfo::initView_forBugs(){
    this->setImage("");
    this->label_title->setText("");
    this->label_artist->setText("");
    this->elidedLabel_description->setText("");//cheon210605
    this->setFavorite(false);
//    this->setMyData(false);

    // 사용하지 않음
    this->btn_addCollect->setVisible(false);
}


/**
 * @brief TIDAL 앨범 적용 관련
 * @param data_album
 */
void AlbumImageDetailInfo::setData_fromTidalData(const tidal::AlbumItemData &data_album){

    // Album 모드로 배치한다.
    this->label_artist->setVisible(true);
    this->label_creatorName->setVisible(false);

    // 데이터를 UI에 세팅한다.
    this->setImage(data_album.image);
    this->label_title->setText(data_album.title);
    this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    if(!data_album.streamStartDate.isEmpty())//cheontidalhires
        this->label_streamStartDate->setText(QString("%1").arg(data_album.streamStartDate).remove(10,18));//cheontidalhires
    if(!data_album.audioQuality.isEmpty())
        this->label_hires->setText(QString("%1").arg(data_album.audioQuality));//cheontidalhires
    if(data_album.numberOfTracks > 0)//cheon210604
        this->label_numberOfTracks->setText(QString("Tracks: %1").arg(data_album.numberOfTracks));//cheon210604
    if(data_album.popularity > 0){
        if(data_album.popularity <= 20){
            this->label_popularity->setText("★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else if(data_album.popularity <= 40){
            this->label_popularity->setText("★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else if(data_album.popularity <= 60){
            this->label_popularity->setText("★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else if(data_album.popularity <= 80){
            this->label_popularity->setText("★★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else {
            this->label_popularity->setText("★★★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
    }

    this->label_artist->setText(data_album.list_artist_name.join(","));
    this->label_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->widget_spaceWithTextAndBtn->setFixedHeight(70);           // 텍스트와 버튼아이콘 사이의 간격


    this->data_key = data_album.id;
    this->setFavorite( this->arr_myFavoriteIds.contains(data_album.id) );       // 기본값

    // 항상 마지막에 한번 더 적용한다. 호출 순서가 다를 수 있으므로
    this->setMyData(this->flagMyData);
}

/**
 * @brief TIDAL 앨범 적용 관련
 * @param data_album
 */
void AlbumImageDetailInfo::setData_fromQobuzData(const qobuz::AlbumItemData &data_album){

    // Album 모드로 배치한다.
    this->label_artist->setVisible(true);
    this->label_creatorName->setVisible(false);

    // 데이터를 UI에 세팅한다.
    this->setImage(data_album.image_large);
    this->label_title->setText(data_album.title);
    this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    if(!data_album.release_date_stream.isEmpty())//cheontidalhires
        this->label_streamStartDate->setText(QString("%1").arg(data_album.release_date_stream).remove(10,18));//cheontidalhires




    if(data_album.hires){ //cheon0812-hires
        QPixmap pic(":images/qobuz/hires_ico.png"); //cheon0812-hires
        this->label_hires_img->setPixmap(pic); // 해당 이미지를 라벨에 넣어줌//cheon0812-hires
        this->label_hires->setText(QString("flac %1bits %2kHz %3ch").arg(data_album.maximum_bit_depth ).arg( data_album.maximum_sampling_rate ).arg( data_album.maximum_channel_count ));//cheon210812-hires
        //this->label_hires->setText(QString("flac %1bits %2kHz %3ch@352.8kHz").arg(data_album.maximum_bit_depth ).arg( data_album.maximum_sampling_rate ).arg( data_album.maximum_channel_count ));//cheon210812-hires
    }
    else{
        this->label_hires_img->hide(); //cheon0812-hires
        this->label_hires->setText(QString("%1bits %2kHz %3ch").arg( data_album.maximum_bit_depth).arg(data_album.maximum_sampling_rate ).arg( data_album.maximum_channel_count ));//cheon210717-hires
    }


    if(data_album.numberOfTracks > 0)//cheon210604
        this->label_numberOfTracks->setText(QString("Tracks: %1").arg(data_album.numberOfTracks));//cheon210604
    if(data_album.popularity != 0){
        if(data_album.popularity <= 20){
            this->label_popularity->setText("★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else if(data_album.popularity <= 40){
            this->label_popularity->setText("★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else if(data_album.popularity <= 60){
            this->label_popularity->setText("★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else if(data_album.popularity <= 80){
            this->label_popularity->setText("★★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else {
            this->label_popularity->setText("★★★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
    }
    this->label_artist->setText(data_album.list_artist_name.join(","));
    this->label_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->widget_spaceWithTextAndBtn->setFixedHeight(70);           // 텍스트와 버튼아이콘 사이의 간격


    this->data_key = data_album.id;
    this->setFavorite( this->arr_myFavoriteIds.contains(data_album.id) );       // 기본값

    // 항상 마지막에 한번 더 적용한다. 호출 순서가 다를 수 있으므로
    this->setMyData(this->flagMyData);
}


void AlbumImageDetailInfo::updateDate_playlist_bioText(const QString &descriptionText){//cheon210605
   // print_debug();
    this->playlist_descriptionText = this->getNormalText(descriptionText);  //cheon210605

    if(this->playlist_descriptionText.isEmpty()){//cheon210605
        this->elidedLabel_description->setVisible(false);//cheon210605
        //this->widget_spaceWithTextAndBtn->setFixedHeight(70);  //cheon210605         // 텍스트와 버튼아이콘 사이의 간격
    }
    else{
        //print_debug();
        QPixmap pixmap_shuffle(":/images//album_more_icon.png");//cheon210605
        this->label_artistInfo->setText(tr("Album More Info"));//cheon210605
        this->label_artistInfo->setCursor(Qt::PointingHandCursor);//cheon210605
        this->label_artistInfo->setIcon(pixmap_shuffle);//cheon210605
        this->label_artistInfo->setIconSize(pixmap_shuffle.rect().size());//cheon210605

        this->textEdit->setText(this->playlist_descriptionText);//cheon210605
        this->label_artistInfo->setVisible(true);//cheon210605
        this->elidedLabel_description->setVisible(true);//cheon210605
        this->setTextDescription_withAdaptedWidgh(this->playlist_descriptionText);//cheon210605
    }
}


/**
 * @brief Bugs 앨범 적용 관련
 * @param data_album
 */
void AlbumImageDetailInfo::setData_fromBugsData(const bugs::AlbumItemData &data_album){

    // Album 모드로 배치한다.
    this->label_artist->setVisible(true);
    this->label_creatorName->setVisible(false);
    this->updateDate_playlist_bioText(data_album.descr);//cheon210605
    // 데이터를 UI에 세팅한다.
    this->setImage(data_album.album_image);
    this->label_title->setText(data_album.title);
    this->label_artist->setText(data_album.list_artist_nm.join(","));

    this->widget_spaceWithTextAndBtn->setFixedHeight(70);           // 텍스트와 버튼아이콘 사이의 간격

   // if(!data_album.descr.isEmpty())//cheon210605
   //     this->label_descr->setText(QString("%1").arg(data_album.descr));//cheon210605 remove(10,18)

    if(!data_album.upd_dt.isEmpty())//cheon210606
        this->label_upd_dt->setText(QString("%1").arg(data_album.upd_dt).remove(10,18));//cheon210606
  //  qDebug() << "\n\n\n\n\ndata.genre_nm" << data_album.genre_nm;
   // if(!data_album.genre_nm.isEmpty())//cheon210605
    //    this->label_descr->setText(QString("%1").arg(data_album.genre_nm));//cheon210605 remove(10,18)

    /*
    if(!data_album.release_date_stream.isEmpty())//cheontidalhires
        this->label_streamStartDate->setText(QString("%1").arg(data_album.release_date_stream).remove(10,18));//cheontidalhires
    if(!data_album.description.isEmpty())//cheontidalhires
        this->label_hires->setText(QString("%1").arg(data_album.description));//cheontidalhires
    if(data_album.numberOfTracks > 0)//cheon210604
        this->label_numberOfTracks->setText(QString("Tracks: %1").arg(data_album.numberOfTracks));//cheon210604
    if(data_album.popularity != 0){
        if(data_album.popularity <= 20){
            this->label_popularity->setText("★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else if(data_album.popularity <= 40){
            this->label_popularity->setText("★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else if(data_album.popularity <= 60){
            this->label_popularity->setText("★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else if(data_album.popularity <= 80){
            this->label_popularity->setText("★★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else {
            this->label_popularity->setText("★★★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
    }
    */

    this->data_key = data_album.album_id;
    this->setFavorite(data_album.likes_yn);

    // 항상 마지막에 한번 더 적용한다. 호출 순서가 다를 수 있으므로
//    this->setMyData(this->flagMyData);

    this->btn_addCollect->setVisible(false);
}


/**
 * @brief AlbumImageDetailInfo::setFavoritesIds
 * @param p_arr_myFavoriteIds
 */
void AlbumImageDetailInfo::setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds){

    AbstractImageDetailInfo::setFavoritesIds_forTidal(p_arr_myFavoriteIds);

    if(!this->label_title->text().isEmpty()){
        // 데이터가 세팅된 경우
        this->setFavorite( this->arr_myFavoriteIds.contains(data_key) );
    }
}


/**
 * @brief AlbumImageDetailInfo::setFavoritesIds
 * @param p_arr_myFavoriteIds
 */
void AlbumImageDetailInfo::setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds){

    AbstractImageDetailInfo::setFavoritesIds_forQobuz(p_arr_myFavoriteIds);

    if(!this->label_title->text().isEmpty()){
        // 데이터가 세팅된 경우
        this->setFavorite( this->arr_myFavoriteIds.contains(data_key) );
    }
}

void AlbumImageDetailInfo::setFavorites_forQobuz(const bool flag){

    //AbstractImageDetailInfo::setFavoritesIds_forQobuz(p_arr_myFavoriteIds);

    this->setFavorite(flag);

}
