#ifndef ABSTRACTIMAGEDETAILINFO_H
#define ABSTRACTIMAGEDETAILINFO_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QDialog>//cheonartist
#include <QTextEdit>//cheonartist
#include <common/filedownloader.h>
#include "widget/ElidedLabel.h"

/**
 * @brief Album, Artest, Playlist 디테일 등의 화면에서 상단에 보이는 상세정보를 표현하는 위젯의 최상위 클래스
 * @details 하위 클래스는 @sa AlbumImageDetailInfo, @sa PlaylistImageDetailInfo, @sa PDAlbumImageDetailInfo 등으로 구성됨
 *
 */
class AbstractImageDetailInfo : public QWidget
{
    Q_OBJECT
public:

    /// 내부 버튼에 대한 클릭 이벤트
    enum BtnClickMode {
        AddCollection = 1           ///< "플레이리스트 담기" 아이콘 클릭
        , Favorite_toAdd            ///< "즐겨찾기 추가" 하트 아이콘 클릭 (하트 Off -> On)
        , Favorite_toDelete         ///< "즐겨찾기 삭제" 하트 아이콘 클릭 (하트 On -> Off)
        , Share
        , More                      ///< "더보기" 아이콘 클릭 (OptMorePopup 띄워야함)
        , PlayAll                   ///< "전체재생" 클릭
        , PlayShuffle               ///< "셔플재생" 클릭
    };

    Q_ENUM(BtnClickMode);

    explicit AbstractImageDetailInfo(QWidget *parent = nullptr);
    ~AbstractImageDetailInfo();

    void setFavorite(bool flagFavorite);
    virtual void initView();                    ///< 화면을 초기화한다. http 요청~응답결과 사이 시간동안 이전 데이터 보이는게 싫으니.
    virtual void setMyData(bool flagMine);      ///< 필요한 경우 재정의해서 사용
    virtual void setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds){ this->arr_myFavoriteIds = p_arr_myFavoriteIds; }
    virtual void setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds){ this->arr_myFavoriteIds = p_arr_myFavoriteIds; }

    void setPossiblePlay(bool flagCanPlay);

    void resizeEvent(QResizeEvent *event) override;

public:
    QWidget *sample;//cheonartist
    QLabel *label_artText;//cheonartist


signals :
    void signal_clicked(const AbstractImageDetailInfo::BtnClickMode);


private slots:
    void slot_fileDownload_loadImage();
    void slot_btnClicked_mode();
    void slot_hide_pushButton_clicked();//cheonartist
    void slot_show_pushButton_clicked();//cheonartist

protected slots:
    virtual void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&){ }       ///< 자식 클래스마다 override


protected:

    void setVisible_optionBtn(bool flagVisiable);
    void setVisible_playBtn(bool flagVisiable);


    QVBoxLayout *vBox_albumInfo;

    QLabel *label_title;
    QLabel *label_genre;//cheon210606
    ElidedLabel *elidedLabel_description;
    QLabel *label_creatorName;
    QLabel *label_artist;
    //QLabel *label_profile_url;//cheon210606

    QTextEdit* textEdit;//cheonartist

    QPushButton *label_artistInfo;//cheonartist
    QLabel *label_hires;//cheontidalhirespopularity
    QLabel *label_hires_img;//cheon0812-hires
    QLabel *label_streamStartDate;//cheontidalhires
    QLabel *label_popularity;//cheontidalhires
    QLabel *label_numberOfTracks;//cheon210604
    QLabel *label_descr;//cheon210604
    QLabel *label_track_count;//cheon210604
    QLabel *label_upd_dt;//cheon210604

    QWidget *widget_spaceWithTextAndBtn;            ///< 상단의 텍스트 정보과 아이콘 버튼 사이의 스페이스 조절을 위함
    QPushButton *btn_addCollect;
    QPushButton *btn_fav_toAdd;
    QPushButton *btn_fav_toDelete;
    QPushButton *btn_share; //c220818
    QPushButton *btn_more;




    void setImage(QString imagePath);
    void setImageSizeAndCorner(int width, int height, int cornerRadius);

    void setTextDescription_withAdaptedWidgh(QString text);
    QString getNormalText(QString text);

    bool flagMyData = false;            ///< My Data 상세인지의 여부
    int image_width;
    int image_height;
    int image_cornerRadius;


    QVariant data_key;                       ///< 각각의 컨텐츠의 id
    QVariantList arr_myFavoriteIds;         ///< 본인 관련 파트에 대한 Favorite Id 모음

private:
    void setUIControl_basic();
    void setUIControl_btnAddFavMore();
    void setUIControl_btnPlays();
    void setUIControl_btnPlays_cannot();

    void paint_imageBig(QPixmap &pixmap);

    void setUIArtist_detailInfoPop();//cheonartist
   // void setUIPlaylist_detailInfoPop();//cheon2106033




    // UI
    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QPixmap *pixmap_albumImg;
    QLabel *label_imageBig;

    QWidget *widget_optBtn;

    QWidget *widget_playBtns;
    QWidget *widget_playBtns_cannot;

    bool flagFavorite = false;          ///< 즐겨찾기 여부. true - 하트 ON 상태

};

class AbstractImageDetailInfo_forQobuz : public QWidget
{
    Q_OBJECT
public:

    /// 내부 버튼에 대한 클릭 이벤트
    enum BtnClickMode {
        AddCollection = 1           ///< "플레이리스트 담기" 아이콘 클릭
        , Favorite_toAdd            ///< "즐겨찾기 추가" 하트 아이콘 클릭 (하트 Off -> On)
        , Favorite_toDelete         ///< "즐겨찾기 삭제" 하트 아이콘 클릭 (하트 On -> Off)
        , More                      ///< "더보기" 아이콘 클릭 (OptMorePopup 띄워야함)
        , PlayAll                   ///< "전체재생" 클릭
        , PlayShuffle               ///< "셔플재생" 클릭
    };

    Q_ENUM(BtnClickMode);

    explicit AbstractImageDetailInfo_forQobuz(QWidget *parent = nullptr);
    ~AbstractImageDetailInfo_forQobuz();

    void setFavorite(bool flagFavorite);
    virtual void initView();                    ///< 화면을 초기화한다. http 요청~응답결과 사이 시간동안 이전 데이터 보이는게 싫으니.
    virtual void setMyData(bool flagMine);      ///< 필요한 경우 재정의해서 사용
    virtual void setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds){ this->arr_myFavoriteIds = p_arr_myFavoriteIds; }
    virtual void setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds){ this->arr_myFavoriteIds = p_arr_myFavoriteIds; }

    void setPossiblePlay(bool flagCanPlay);

    void resizeEvent(QResizeEvent *event) override;

public:
    QTextEdit* textEdit;//cheonartist
    QLabel *label_artText;//cheonartist

signals :
    void signal_clicked(const AbstractImageDetailInfo_forQobuz::BtnClickMode);


private slots:
    void slot_fileDownload_loadImage();
    void slot_btnClicked_mode();
    void slot_hide_pushButton_clicked();//cheonartist
    void slot_show_pushButton_clicked();//cheonartist

protected slots:
    virtual void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&){ }       ///< 자식 클래스마다 override

protected:

    void setVisible_optionBtn(bool flagVisiable);
    void setVisible_playBtn(bool flagVisiable);

    QLabel *label_hires;//cheontidalhires
    QLabel *label_streamStartDate;//cheontidalhires
    QLabel *label_popularity;//cheontidalhires
    //QLabel *label_profile_url;//cheon210606

    QLabel *label_numberOfTracks;//cheon210604

    QVBoxLayout *vBox_albumInfo;

    QLabel *label_title;
    QLabel *label_genre;//cheon210606
    ElidedLabel *elidedLabel_description;
    QLabel *label_creatorName;
    QLabel *label_artist;

    QWidget *widget_spaceWithTextAndBtn;            ///< 상단의 텍스트 정보과 아이콘 버튼 사이의 스페이스 조절을 위함
    QPushButton *btn_addCollect;
    QPushButton *btn_fav_toAdd;
    QPushButton *btn_fav_toDelete;
    QPushButton *btn_more;




    void setImage(QString imagePath);
    void setPlaylistImage(QString imagePath);
    void setImageSizeAndCorner(int width, int height, int cornerRadius);

    void setTextDescription_withAdaptedWidgh(QString text);
    QString getNormalText(QString text);

    bool flagMyData = false;            ///< My Data 상세인지의 여부
    int image_width;
    int image_height;
    int image_cornerRadius;


    QVariant data_key;                       ///< 각각의 컨텐츠의 id
    QVariantList arr_myFavoriteIds;         ///< 본인 관련 파트에 대한 Favorite Id 모음

private:
    void setUIControl_basic();
    void setUIControl_btnAddFavMore();
    void setUIControl_btnPlays();
    void setUIControl_btnPlays_cannot();

    void paint_imageBig(QPixmap &pixmap);
    void paint_imageractangle(QPixmap &pixmap);

    void setUIArtist_detailInfoPop();//cheonartist
   // void setUIPlaylist_detailInfoPop();//cheon210603

    QWidget *sample;//cheonartist
    QPushButton *label_artistInfo;//cheonartist

    // UI
    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QPixmap *pixmap_albumImg;
    QPixmap *pixmap_playlistImg;
    QPixmap *pixmap_artistImg;
    QLabel *label_imageBig;

    QWidget *widget_optBtn;

    QWidget *widget_playBtns;
    QWidget *widget_playBtns_cannot;

    bool flagFavorite = false;          ///< 즐겨찾기 여부. true - 하트 ON 상태
    bool flagPlaylist = false;

};

#endif // ABSTRACTIMAGEDETAILINFO_H
