#ifndef ABSTRACTIMAGEDETAILCONTENTS_RHV_H
#define ABSTRACTIMAGEDETAILCONTENTS_RHV_H

#include "common/filedownloader.h"

#include "widget/flowlayout.h"
#include "widget/textlonganimation.h"

#include <QLabel>
#include <QDialog>
#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>


/**
 * @brief Album, Artest, Playlist 디테일 등의 화면에서 상단에 보이는 상세정보를 표현하는 위젯의 최상위 클래스
 * @details 하위 클래스는
 * @sa AlbumImageDetailInfo, @sa PlaylistImageDetailInfo, @sa PDAlbumImageDetailInfo 등으로 구성됨
 *
 */
class AbstractImageDetailContents_RHV : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Main UI를 어떻게 구성할건지에 대한 옵션
     */
    enum ContentsUIType {
        rose_artist = 1
        , rose_album
        , rose_playlist
        , rose_userplaylist
        , Music_artist
        , Music_album
        , Music_playlist
        , Music_userplaylist
        , Rosetube_userplaylist
        , Tidal_artist
        , Tidal_album
        , Tidal_playlist
        , Tidal_playlistMix
        , Tidal_video
        , Bugs_artist
        , Bugs_album
        , Bugs_pdAlbum
        , Bugs_video
        , Qobuz_artist
        , Qobuz_album
        , Qobuz_playlist
        , Apple_artist
        , Apple_album
        , Apple_playlist
        , CDPlay_album//c220724
        , Video
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
        , Share                     ///< "공유" 아이콘 클릭
        , Info                      ///< "정보보기" 아이콘 클릭 (InfomationPopip 띄워야함)
        , More                      ///< "더보기" 아이콘 클릭 (OptMorePopup 띄워야함)
        , PlayAll                   ///< "전체재생" 클릭
        , PlayShuffle               ///< "셔플재생" 클릭
    };
    Q_ENUM(BtnClickMode);

    explicit AbstractImageDetailContents_RHV(ContentsUIType p_ContentsUIType, QWidget *parent = nullptr);
    ~AbstractImageDetailContents_RHV();

    void initView();

    void setData_fromRoseData(const QJsonObject&);
    void setData_fromMusicData(const QJsonObject&);
    void setData_fromRosetubeData(const QJsonObject&);
    void setData_fromTidalData(const QJsonObject&);
    void setData_fromBugsData(const QJsonObject&);
    void setData_fromQobuzData(const QJsonObject&);
    void setData_fromAppleData(const QJsonObject&);
    void setData_fromCDplayData(const QJsonObject&);//c220724
    void setData_fromVideoData(const QJsonObject&);

    void setData_Resolution(const QJsonObject&);
    void setData_Biography(const QJsonObject&);

    void setFavorite(bool flagFavorite, int star);
    void setPossiblePlay(bool flagCanPlay);
    void setVisible_optionBtn(bool flagVisiable);
    void setVisible_playBtn(bool flagVisiable);
    void setHiRes_Left(int left, int height);
    void setOpen_Height(int height, bool flag);

    int getFavoritesStars();

    void resizeEvent(QResizeEvent *event) override;

public:
    QString detailInfo_shareLink = "";

signals :
    void signal_clicked(const AbstractImageDetailContents_RHV::BtnClickMode);
    void signal_clicked_artistMore();
    void signal_clicked_tag(const int);

protected:
    void setImage(QString imagePath);
    void setPlaylistImage(QString imagePath);
    void setImageSizeAndCorner(int width, int height, int cornerRadius);

    void setTextDescription_withAdaptedWidgh(QString text);
    QString getNormalText(QString text);

protected:
    LabelLongAnimation *label_title;

    QLabel *label_imageHiRes;
    QLabel *label_creatorName;
    QLabel *label_artist;
    QLabel *label_resolution;
    QLabel *label_description;
    QLabel *label_biography;
    QLabel *label_open;

    QPushButton *btn_Label_imageBig;//c230215
    QPushButton *btn_more_artist;
    QPushButton *btn_addCollect;
    QPushButton *btn_fav_toAdd;
    QPushButton *btn_fav_toAddx2;
    QPushButton *btn_fav_toAddx3;
    QPushButton *btn_fav_toDelete;
    QPushButton *btn_thumb_toAdd;
    QPushButton *btn_thumb_toDelete;
    QPushButton *btn_share;
    QPushButton *btn_info;
    QPushButton *btn_more;
    QPushButton *btn_open;

    QWidget *widget_info_main;
    QWidget *widget_info_tag;
    QWidget *widget_Addbtn_Icon;
    QWidget *widget_Addbtn_Play;
    QWidget *widget_Addbtn_PlayCannot;
    QWidget *widget_Addbtn_Open;

    FlowLayout *flowLayout_tag;
    QVBoxLayout *vBox_info;

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

private slots:
    void slot_fileDownload_loadImage();
    void slot_btnClicked_mode();
    void slot_btnClicked_open();
    void slot_btnClicked_artistMore();
    void slot_btnClicked_tag();

    void slot_redirectUrl(const QString url);                   //j220903 twitter
    void slot_applyResult_getShareLink(const QString&);         //j220905 share link

    void slot_imageClick();                                     //c230215
    void slot_downloadThumbImageBig();                          //c230215

private:
    void setUIControl_basic();
    void setUIControl_btnAddFavMore();
    void setUIControl_btnPlays();
    void setUIControl_btnPlays_cannot();

    void request_shareLlink(const QString thumnail, const QString desc, const QString title, const QString id); //j220905 share link

    void paint_imageBig(QPixmap &pixmap);
    void paint_imageractangle(QPixmap &pixmap);

    void downloadThumbImageBig(QString pathImg);                //c230215

private:
    ContentsUIType curr_contentsType;

    // UI
    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QPixmap *pixmap_albumImg;
    QPixmap *pixmap_HiResImg;
    QPixmap *pixmap_playlistImg;
    QPixmap *pixmap_artistImg;
    QLabel *label_imageBig;
    QLabel *label_imageType;

    QWidget *widget_optBtn;

    QString btnStyle_play;
    QString btnStyle_shuffle;

    QList<QString> list_tag_name;

    bool flagFavorite = false;          ///< 즐겨찾기 여부. true - 하트 ON 상태
    bool flagPlaylist = false;

    bool flagOpen = false;

    int heightOpen = 0;

    bool flag_type_image = false;
    QString type_image_path = "";

    QLabel *lb_BigImg;//c230215

    int bigImgWidth = 800;//c230215
    int bigImgHeight = 800;//c230215
};

#endif // ABSTRACTIMAGEDETAILCONTENTS_RHV_H
