#ifndef COMPOSERINFO_H
#define COMPOSERINFO_H

#include <QWidget>
#include "widget/optionpopup.h"
#include "delegate/filedelegate.h"
#include <widget/customlistwidget.h>
#include "proc_addtackinplaylist.h"


/**
 * @brief 작곡가 상세 화면 @n
 *      작곡가의 곡, 앨범 모음
 */
class ComposerInfo : public QWidget
{
    Q_OBJECT
public:
    explicit ComposerInfo(QWidget *parent = nullptr);
    ~ComposerInfo();

    void setDataComposerInfo(QJsonObject p_data);

signals:
    void signal_changedMenuName(const QString &p_menuName);

public slots:
    void slot_loadImage();
private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_playAll();
    void slot_playShuffle();
    void slot_playlistClicked(const int &p_playlistno);
    void slot_newPlaylistNameEdited(QString);
    void slot_delegateClicked(const int &p_index, const int &p_btnType);
    void slot_thumbnailDownloaded();

private :
    // >>>>>>>>>> DATA >>>>>>>>>>>
    OptionPopup::TypeMenu type_optionMenu;  ///< 옵션팝업을 어느곳에서 띄우는지(부모가 누군지) 유형
    QString albumImg;                   ///< 앨범 이미지 경로
    QString composer;                     ///< 작곡가
    QString infoText;                   ///< 장르 설명
    QList<DataPopup*> list_audioInfo;   ///< 오디오 정보 리스트 (바로듣기 api관련 된 것때문에 여러정보를 가지고있음)
    // <<<<<<<<<< DATA <<<<<<<<<<<

    Linker *linker;

    QPixmap *pixmap_albumImg_default;
    FileDownloader *fileDownLoader;
    QPixmap *pixmap_albumImg;
    QLabel *lb_albumImg;
    QLabel *lb_genre;
    QLabel *lb_infoText;

    FileDelegate *delegate;
    CustomListWidget *listWidget;
    QLabel *lb_tracks;

    Proc_AddTackInPlayList *proc_add;

    void setInit();
    void setUIControl();
    void requestShowOptionPopup(const int &p_index);
    void setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor);
    void changeTrackFavoritesUI(const int &p_index);
    void requestGetTrackFavorites(const int &p_index);
    void requestSetTrackFavorites(const int &p_index);
    QJsonObject getJsonObject_OptionPopup(const int &p_index);
    QJsonArray getJsonArr_TrackData(const int &p_index);
    QJsonObject getJsonObject_TrackData(const int &p_index);
    void setResultOfGetFavorite(const int &p_index, const QJsonObject &p_jsonData);
    void setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData);

};

#endif // COMPOSERINFO_H
