#ifndef ABSTARACTIMAGEDETAILINFO_RHV_H
#define ABSTARACTIMAGEDETAILINFO_RHV_H

#include "common/filedownloader.h"
#include "home/topmenubar.h"
#include "widget/ElidedLabel.h"

#include <QLabel>
#include <QDialog>
#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>


/**
 * @brief Album, Artest, Playlist 디테일 등의 화면에서 상단에 보이는 상세정보를 표현하는 위젯의 최상위 클래스
 * @details 하위 클래스는 @sa AlbumImageDetailInfo, @sa PlaylistImageDetailInfo, @sa PDAlbumImageDetailInfo 등으로 구성됨
 *
 */
class AbstractImageDetailInfo_RHV : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Main UI를 어떻게 구성할건지에 대한 옵션
     */
    enum ContentsUIType {
        Music_album = 1
        , Music_playlist
        , Music_userplaylist
        , Tidal_album
        , Tidal_playlist
        , Tidal_video
        , Bugs_album
        , Bugs_playlist
        , Bugs_video
        , Qobuz_album
        , Qobuz_playlist
    };
    Q_ENUM(ContentsUIType);

    /// 내부 버튼에 대한 클릭 이벤트
    enum BtnClickMode {
        AddCollection = 1           ///< "플레이리스트 담기" 아이콘 클릭
        , Favorite_toAdd            ///< "즐겨찾기 추가" 하트 아이콘 클릭 (하트 Off -> On)
        , Favorite_toAddx2          ///< "즐겨찾기 추가" 하트 아이콘 클릭 (하트 On -> Onx2)
        , Favorite_toAddx3          ///< "즐겨찾기 추가" 하트 아이콘 클릭 (하트 Onx2 -> Onx3)
        , Favorite_toDelete         ///< "즐겨찾기 삭제" 하트 아이콘 클릭 (하트 Onx3 -> Off)
        , Thumbup_toAdd             ///< "thumbup 추가" thumbup 아이콘 클릭 (하트 Off -> On)
        , Thumbup_toDelete          ///< "thumbup 삭제" thumbup 아이콘 클릭 (하트 On -> Off)
        , Info                      ///< "정보보기" 아이콘 클릭 (InfomationPopip 띄워야함)
        , More                      ///< "더보기" 아이콘 클릭 (OptMorePopup 띄워야함)
        , PlayAll                   ///< "전체재생" 클릭
        , PlayShuffle               ///< "셔플재생" 클릭
    };
    Q_ENUM(BtnClickMode);

    explicit AbstractImageDetailInfo_RHV(ContentsUIType p_ContentsUIType, QWidget *parent = nullptr);
    ~AbstractImageDetailInfo_RHV();

    void setFavorite(bool flagFavorite);
    void setFavorite(bool flagFavorite, int star);
    virtual void initView();                    ///< 화면을 초기화한다. http 요청~응답결과 사이 시간동안 이전 데이터 보이는게 싫으니.
    virtual void setMyData(bool flagMine);      ///< 필요한 경우 재정의해서 사용

    virtual void setFavoritesIds_forTidal(QVariantList p_arr_myFavoriteIds){ this->arr_myFavoriteIds = p_arr_myFavoriteIds; }
    virtual void setFavoritesIds_forBugs(QVariantList p_arr_myFavoriteIds){ this->arr_myFavoriteIds = p_arr_myFavoriteIds; }
    virtual void setFavoritesIds_forQobuz(QVariantList p_arr_myFavoriteIds){ this->arr_myFavoriteIds = p_arr_myFavoriteIds; }

    virtual void setFavoritesIds(bool p_myFavoriteIds, int p_star){ this->myFavoriteIds = p_myFavoriteIds; this->myStarIds = p_star; }
    virtual bool getFavoritesIds(){ return this->myFavoriteIds; }
    virtual int getFavoritesStars(){ return this->myStarIds; }

    void setPossiblePlay(bool flagCanPlay);

    void resizeEvent(QResizeEvent *event) override;

signals :
    void signal_clicked(const AbstractImageDetailInfo_RHV::BtnClickMode);

private slots:
    void slot_fileDownload_loadImage();
    void slot_btnClicked_mode();
    void slot_btnopen_clicked();

protected slots:
    virtual void slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject&){ }       ///< 자식 클래스마다 override

protected:

    void setVisible_optionBtn(bool flagVisiable);
    void setVisible_playBtn(bool flagVisiable);
    void setHiRes_Left(int left, int height);
    void setOpen_Height(int height, bool flag);

    QLabel *label_titleUp;
    QLabel *label_titleDown;
    QLabel *label_imageHiRes;
    QLabel *label_creatorName;
    QLabel *label_artist;
    QLabel *label_resolution;
    QLabel *label_description;
    QLabel *label_open;

    QPushButton *btn_addCollect;
    QPushButton *btn_fav_toAdd;
    QPushButton *btn_fav_toAddx2;
    QPushButton *btn_fav_toAddx3;
    QPushButton *btn_fav_toDelete;
    QPushButton *btn_thumb_toAdd;
    QPushButton *btn_thumb_toDelete;
    QPushButton *btn_info;
    QPushButton *btn_more;    
    QPushButton *btn_open;

    QWidget *widget_info_main;
    QWidget *widget_Addbtn_Icon;
    QWidget *widget_Addbtn_Play;
    QWidget *widget_Addbtn_PlayCannot;
    QWidget *widget_Addbtn_Open;

    QWidget *widget_tab_tracks;
    QWidget *widget_track_main;
    QWidget *widget_tab_credit;

    QStackedWidget *stackedwidget;

    QVBoxLayout *vl_tracks;
    QVBoxLayout *vl_credit;

    QVBoxLayout *vl_track_main;
    QVBoxLayout *vl_same_artist;

    QVBoxLayout *info_hbox;

    void setImage(QString imagePath);
    void setPlaylistImage(QString imagePath);
    void setImageSizeAndCorner(int width, int height, int cornerRadius);

    void setTextDescription_withAdaptedWidgh(QString text);
    QString getNormalText(QString text);

    bool flagMyData = false;            ///< My Data 상세인지의 여부
    bool flagBtnOpen = false;

    int image_width;
    int image_height;
    int image_cornerRadius;

    QString contentStep;

    QVariant data_key;                      ///< 각각의 컨텐츠의 id
    QVariantList arr_myFavoriteIds;         ///< 본인 관련 파트에 대한 Favorite Id 모음

    bool myFavoriteIds = false;
    int myStarIds = 0;

private:

    ContentsUIType curr_contentsType;

    void setUIControl_basic();
    void setUIControl_btnAddFavMore();
    void setUIControl_btnPlays();
    void setUIControl_btnPlays_cannot();

    void paint_imageBig(QPixmap &pixmap);
    void paint_imageractangle(QPixmap &pixmap);

    // UI
    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QPixmap *pixmap_albumImg;
    QPixmap *pixmap_HiResImg;
    QPixmap *pixmap_playlistImg;
    QPixmap *pixmap_artistImg;
    QLabel *label_imageBig;

    QWidget *widget_optBtn;

    TopMenuBar *menubar;

    QString btnStyle;

    bool flagFavorite = false;          ///< 즐겨찾기 여부. true - 하트 ON 상태
    bool flagPlaylist = false;

    bool flagOpen = false;

    int heightOpen = 0;
};

#endif // ABSTARACTIMAGEDETAILINFO_RHV_H
