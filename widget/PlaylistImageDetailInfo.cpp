#include "PlaylistImageDetailInfo.h"

#include <QFontMetrics>
#include "common/gscommon.h"


/**
 * @brief 생성자
 * @param parent
 */
PlaylistImageDetailInfo::PlaylistImageDetailInfo(QWidget *parent) : AbstractImageDetailInfo(parent){
    this->setImageSizeAndCorner(300, 300, 4);
}


/**
 * @brief PlaylistImageDetailInfo::initView
 */
void PlaylistImageDetailInfo::initView(){
    this->setImage("");
    this->label_title->setText("");
    this->label_creatorName->setText("");
    this->elidedLabel_description->setText("");

    this->setFavorite(false);
    this->setMyData(false);
}

/**
 * @brief PlaylistImageDetailInfo::setMyData
 * @param flagMine
 */
void PlaylistImageDetailInfo::setMyData(bool flagMine){
    // 항상 부모 먼저 호출
    AbstractImageDetailInfo::setMyData(flagMine);

    this->elidedLabel_description->setVisible(!flagMine);           // 설명 부분
    this->btn_addCollect->setVisible(!flagMine);                    // Add Collection 부분
    this->btn_fav_toAdd->setVisible(!flagMine);
    this->btn_fav_toDelete->setVisible(!flagMine);

}



void PlaylistImageDetailInfo::updateDate_playlist_bioText(const QString &descriptionText){//cheon210603
   // print_debug();
    this->playlist_descriptionText = this->getNormalText(descriptionText);  //cheon0514

    if(this->playlist_descriptionText.isEmpty()){
        this->elidedLabel_description->setVisible(false);
        //this->widget_spaceWithTextAndBtn->setFixedHeight(70);           // 텍스트와 버튼아이콘 사이의 간격
    }
    else{
        //print_debug();
        QPixmap pixmap_shuffle(":/images//album_more_icon.png");//artistmoreinfo
        this->label_artistInfo->setText(tr("Playlist More Info"));//artistmoreinfo
        this->label_artistInfo->setCursor(Qt::PointingHandCursor);//cheonartistmoreinfo
        this->label_artistInfo->setIcon(pixmap_shuffle);//artistmoreinfo
        this->label_artistInfo->setIconSize(pixmap_shuffle.rect().size());//artistmoreinfo

        this->textEdit->setText(this->playlist_descriptionText);//cheonartist
        this->label_artistInfo->setVisible(true);//artistmoreinfo
        this->elidedLabel_description->setVisible(true);
        this->setTextDescription_withAdaptedWidgh(this->playlist_descriptionText);
    }
}



/**
 * @brief TIDAL 앨범 적용 관련
 * @param data_album
 */
void PlaylistImageDetailInfo::setData_fromTidalData(const tidal::PlaylistItemData &data_playlist){

    // Playlist 모드로 배치한다.
    this->label_artist->setVisible(false);
    this->label_creatorName->setVisible(true);
    this->elidedLabel_description->setVisible(true);

    // 데이터를 UI에 세팅한다.
    this->setImage(data_playlist.image);
    this->label_title->setText(data_playlist.title);
    this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->setTextDescription_withAdaptedWidgh(data_playlist.description);


    this->label_creatorName->setText(data_playlist.creatorName);

    this->updateDate_playlist_bioText(data_playlist.description);//cheon210603
    if(!data_playlist.lastUpdated.isEmpty())//cheon210603
        this->label_streamStartDate->setText(QString("%1").arg(data_playlist.lastUpdated).remove(10,18));//cheontidalhires
    //if(!data_playlist.audioQuality.isEmpty())
    //    this->label_hires->setText(QString("%1").arg(data_album.audioQuality));//cheon210603
    if(data_playlist.popularity != 0){
        if(data_playlist.popularity <= 20){
            this->label_popularity->setText("★");//cheon210603
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheon210603
        }
        else if(data_playlist.popularity <= 40){
            this->label_popularity->setText("★★");//cheon210603
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheon210603
        }
        else if(data_playlist.popularity <= 60){
            this->label_popularity->setText("★★★");//cheon210603
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheon210603
        }
        else if(data_playlist.popularity <= 80){
            this->label_popularity->setText("★★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else {
            this->label_popularity->setText("★★★★★");//cheon210603
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheon210603
        }
    }

    this->widget_spaceWithTextAndBtn->setFixedHeight(20);           // 텍스트와 버튼아이콘 사이의 간격


    this->data_key = data_playlist.uuid;
    if(data_playlist.flagCreatedByUser){
        this->setMyData(true);
    }
    else{
        this->setMyData(false);
        this->setFavorite( this->arr_myFavoriteIds.contains(data_playlist.uuid) );       // 기본값
    }

}



/**
 * @brief PlaylistImageDetailInfo::setFavoritesIds
 * @param p_arr_myFavoriteIds
 */
void PlaylistImageDetailInfo::setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds){

    AbstractImageDetailInfo::setFavoritesIds_forTidal(p_arr_myFavoriteIds);

    if(!this->label_title->text().isEmpty() && !this->flagMyData){
        // 데이터가 세팅된 경우, User created playlist 아닌 경우
        this->setFavorite( this->arr_myFavoriteIds.contains(data_key) );
    }
}




/**
 * @brief My playlist 의 편집모드에서 사용하기 위함
 */
void PlaylistImageDetailInfo::set_display_forEditMode(){
    this->setVisible_playBtn(false);
    this->setVisible_optionBtn(false);
}



/**
 * @brief 생성자
 * @param parent
 */
//PlaylistImageDetailInfo_forQobuz::PlaylistImageDetailInfo_forQobuz(QWidget *parent) : AbstractImageDetailInfo(parent){
PlaylistImageDetailInfo_forQobuz::PlaylistImageDetailInfo_forQobuz(QWidget *parent) : AbstractImageDetailInfo_forQobuz(parent){
    this->setImageSizeAndCorner(384, 186, 4);
}


/**
 * @brief PlaylistImageDetailInfo::initView
 */
void PlaylistImageDetailInfo_forQobuz::initView(){
    this->setImage("");
    this->label_title->setText("");
    this->label_creatorName->setText("");
    this->elidedLabel_description->setText("");

    this->setFavorite(false);
    this->setMyData(false);
}

/**
 * @brief PlaylistImageDetailInfo::setMyData
 * @param flagMine
 */
void PlaylistImageDetailInfo_forQobuz::setMyData(bool flagMine){
    // 항상 부모 먼저 호출
    AbstractImageDetailInfo_forQobuz::setMyData(flagMine);

    this->elidedLabel_description->setVisible(!flagMine);           // 설명 부분
    this->btn_addCollect->setVisible(!flagMine);                    // Add Collection 부분
    this->btn_fav_toAdd->setVisible(!flagMine);
    this->btn_fav_toDelete->setVisible(!flagMine);

}



/**
 * @brief QOBUZ 앨범 적용 관련
 * @param data_album
 */
void PlaylistImageDetailInfo_forQobuz::setData_fromQobuzData(const qobuz::PlaylistItemData &data_playlist){

    //this->setImageSizeAndCorner(384, 186, 4);

    // Playlist 모드로 배치한다.
    this->label_artist->setVisible(false);
    this->label_creatorName->setVisible(true);
    this->elidedLabel_description->setVisible(true);

    // 데이터를 UI에 세팅한다.
    //this->setImage(data_playlist.image);
    this->setPlaylistImage(data_playlist.image);
    this->label_title->setText(data_playlist.title);
    this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->setTextDescription_withAdaptedWidgh(data_playlist.description);


    this->label_creatorName->setText(data_playlist.ownerName);

    this->widget_spaceWithTextAndBtn->setFixedHeight(20);           // 텍스트와 버튼아이콘 사이의 간격

    if(!data_playlist.lastUpdated.isEmpty())//cheon210603
        this->label_streamStartDate->setText(QString("%1").arg(data_playlist.lastUpdated).remove(10,18));//cheontidalhires
    //if(!data_playlist.audioQuality.isEmpty())
    //    this->label_hires->setText(QString("%1").arg(data_album.audioQuality));//cheon210603
    if(data_playlist.popularity != 0){
        if(data_playlist.popularity <= 20){
            this->label_popularity->setText("★");//cheon210603
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheon210603
        }
        else if(data_playlist.popularity <= 40){
            this->label_popularity->setText("★★");//cheon210603
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheon210603
        }
        else if(data_playlist.popularity <= 60){
            this->label_popularity->setText("★★★");//cheon210603
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheon210603
        }
        else if(data_playlist.popularity <= 80){
            this->label_popularity->setText("★★★★");//cheontidalhires
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheontidalhires
        }
        else {
            this->label_popularity->setText("★★★★★");//cheon210603
           // this->label_popularity->setText(QString("%1").arg(data_album.popularity));//cheon210603
        }
    }

    this->data_key = data_playlist.id;
    if(data_playlist.flagCreatedByUser){
        this->setMyData(true);
    }
    else{
        this->setMyData(false);
        this->setFavorite( this->arr_myFavoriteIds.contains(data_playlist.id) );       // 기본값
    }

}



/**
 * @brief PlaylistImageDetailInfo::setFavoritesIds
 * @param p_arr_myFavoriteIds
 */
void PlaylistImageDetailInfo_forQobuz::setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds){

    AbstractImageDetailInfo_forQobuz::setFavoritesIds_forQobuz(p_arr_myFavoriteIds);

    if(!this->label_title->text().isEmpty() && !this->flagMyData){
        // 데이터가 세팅된 경우, User created playlist 아닌 경우
        this->setFavorite( this->arr_myFavoriteIds.contains(data_key) );
    }

}

void PlaylistImageDetailInfo_forQobuz::setFavorites_forQobuz(const bool flag){

    //AbstractImageDetailInfo_forQobuz::setFavoritesIds_forQobuz(p_arr_myFavoriteIds);

    this->setFavorite(flag);

}


/**
 * @brief My playlist 의 편집모드에서 사용하기 위함
 */
void PlaylistImageDetailInfo_forQobuz::set_display_forEditMode(){
    this->setVisible_playBtn(false);
    this->setVisible_optionBtn(false);
}
