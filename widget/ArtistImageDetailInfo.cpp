#include "ArtistImageDetailInfo.h"
#include <QRegularExpression>

#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


/**
 * @brief 생성자
 * @param parent
 */
ArtistImageDetailInfo::ArtistImageDetailInfo(QWidget *parent) : AbstractImageDetailInfo(parent){
    this->setImageSizeAndCorner(300, 300, 150);
}


/**
 * @brief ArtistImageDetailInfo::initView
 */
void ArtistImageDetailInfo::initView(){
    this->setImage("");
    this->label_title->setText("");
    this->elidedLabel_description->setText("");
    this->setFavorite(false);

    this->artist_bioText = "";
}

/**
 * @brief Bugs용 initView
 */
void ArtistImageDetailInfo::initView_forBugs(){
    this->setImage("");
    this->label_title->setText("");
    this->elidedLabel_description->setText("");
    this->setFavorite(false);
//    this->setMyData(false);

    this->artist_bioText = "";

    // 사용하지 않음
    this->btn_addCollect->setVisible(false);
}


/**
 * @brief TIDAL Artist 적용 관련
 * @param data_artist
 */
void ArtistImageDetailInfo::setData_fromTidalData(const tidal::ArtistItemData &data_artist){


    this->label_artText->setText(QString("Artist : %1").arg(data_artist.name));//cheonartist
    // Artist 모드로 배치한다.
    this->label_artistInfo->setVisible(false); //cheonartistmoreinfo

    this->label_artist->setVisible(false);
    this->label_creatorName->setVisible(false);
    this->elidedLabel_description->setVisible(true);
    this->elidedLabel_description->setText("");
    this->elidedLabel_description->setFixedHeight(66);

    this->btn_addCollect->setVisible(false);                // 항상 안보임
    this->btn_more->setVisible(false);                      // artist detail 에서는 more 버튼 없음

    // 데이터를 UI에 세팅한다.
    this->setImage(data_artist.image);
    this->label_title->setText(data_artist.name);
    this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->widget_spaceWithTextAndBtn->setFixedHeight(20);           // 텍스트와 버튼아이콘 사이의 간격

    this->data_key = data_artist.id;
    this->setFavorite( this->arr_myFavoriteIds.contains(data_artist.id) );       // 기본값

    this->setFavorite(false);       // 기본값

}

/**
 * @brief QOBUZ Artist 적용 관련
 * @param data_artist
 */
void ArtistImageDetailInfo::setData_fromQobuzData(const qobuz::ArtistItemData &data_artist){

    // Artist 모드로 배치한다.
    this->label_artist->setVisible(false);
    this->label_creatorName->setVisible(false);
    this->elidedLabel_description->setVisible(true);
    this->elidedLabel_description->setText("");
    this->elidedLabel_description->setFixedHeight(66);

    this->btn_addCollect->setVisible(false);                // 항상 안보임
    this->btn_more->setVisible(false);                      // artist detail 에서는 more 버튼 없음

    // 데이터를 UI에 세팅한다.
    //this->setImage(data_artist.image);
    this->setImage(data_artist.image_medium);
    this->label_title->setText(data_artist.name);
    this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->widget_spaceWithTextAndBtn->setFixedHeight(20);           // 텍스트와 버튼아이콘 사이의 간격

    this->data_key = data_artist.id;
    this->setFavorite( this->arr_myFavoriteIds.contains(data_artist.id) );       // 기본값

    this->setFavorite(false);       // 기본값

}

/**
 * @brief Bugs Artist 적용 관련
 * @param data_artist
 */
void ArtistImageDetailInfo::setData_fromBugsData(const bugs::ArtistItemData &data_artist){

    this->label_artistInfo->setVisible(false); //cheonartistmoreinfo
    // Artist 모드로 배치한다.
    this->label_artist->setVisible(false);
    this->label_creatorName->setVisible(false);
    this->elidedLabel_description->setVisible(false);
    this->elidedLabel_description->setText("");

    this->btn_addCollect->setVisible(false);                // 항상 안보임
    this->btn_more->setVisible(false);                      // artist detail 에서는 more 버튼 없음

    //if(!data_artist.profile_url.isEmpty())//cheon210606
    //this->label_profile_url->setText(QString("%1").arg(data_artist.profile_url));//cheon210606
    QString genre_dtr ="";//cheon210606
    foreach(QString n, data_artist.list_genre_nm){//cheon210606
        genre_dtr += QString("%1/").arg(n);//cheon210606
    }
    //qDebug() <<"\n\ngenre_dtr :" <<  genre_dtr;

    if(!genre_dtr.isEmpty())//cheon210606
        this->label_genre->setText(QString("%1").arg(genre_dtr));//cheon210606

    // 데이터를 UI에 세팅한다.
    this->setImage(data_artist.image);
    this->label_title->setText(data_artist.artist_nm);
    this->widget_spaceWithTextAndBtn->setFixedHeight(20);           // 텍스트와 버튼아이콘 사이의 간격

    this->data_key = data_artist.artist_id;
    this->setFavorite( data_artist.likes_yn );       // 기본값

}


/**
 * @brief Artist의 간략 신상정보 텍스트를 화면에 보이도록 업데이트 한다.
 * @param bioText
 */
void ArtistImageDetailInfo::updateDate_artist_bioText(const QString &bioText){
    //print_debug();
    this->artist_bioText = this->getNormalText(bioText);  //cheon0514
    //qDebug()<< "bioText : " << this->artist_bioText;

    if(this->artist_bioText.isEmpty()){
        this->elidedLabel_description->setVisible(false);
        //this->widget_spaceWithTextAndBtn->setFixedHeight(70);           // 텍스트와 버튼아이콘 사이의 간격
    }
    else{
        //print_debug();
        QPixmap pixmap_shuffle(":/images//album_more_icon.png");//artistmoreinfo
        this->label_artistInfo->setText(tr("Artist More Info"));//artistmoreinfo
        this->label_artistInfo->setCursor(Qt::PointingHandCursor);//cheonartistmoreinfo
        this->label_artistInfo->setIcon(pixmap_shuffle);//artistmoreinfo
        this->label_artistInfo->setIconSize(pixmap_shuffle.rect().size());//artistmoreinfo

        this->textEdit->setText(this->artist_bioText);//cheonartist
        this->label_artistInfo->setVisible(true);//artistmoreinfo
        this->elidedLabel_description->setVisible(true);
        this->setTextDescription_withAdaptedWidgh(this->artist_bioText);
    }
}


/**
 * @brief ArtistImageDetailInfo::setFavoritesIds
 * @param p_arr_myFavoriteIds
 */
void ArtistImageDetailInfo::setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds){

    AbstractImageDetailInfo::setFavoritesIds_forTidal(p_arr_myFavoriteIds);

    if(!this->label_title->text().isEmpty()){
        // 데이터가 세팅된 경우
        this->setFavorite( this->arr_myFavoriteIds.contains(data_key) );
    }
}

/**
 * @brief ArtistImageDetailInfo::setFavoritesIds
 * @param p_arr_myFavoriteIds
 */
void ArtistImageDetailInfo::setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds){

    AbstractImageDetailInfo::setFavoritesIds_forQobuz(p_arr_myFavoriteIds);

    if(!this->label_title->text().isEmpty()){
        // 데이터가 세팅된 경우
        this->setFavorite( this->arr_myFavoriteIds.contains(data_key) );
    }
}

void ArtistImageDetailInfo::setFavorites_forQobuz(const bool flag){

    this->setFavorite(flag);

}
