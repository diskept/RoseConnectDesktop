#ifndef MUSICSEARCH_H
#define MUSICSEARCH_H

#include "proc_addtackinplaylist.h"

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QScrollArea>
#include <QLabel>//cheon211115-01
#include <common/linker.h>

#include <delegate/filedelegate.h>

#include <data/datapopup.h>

/**
 * @brief 음악-검색결과 화면 @n
 *      음악 검색결과를 트랙, 앨범, 아티스트 항목으로 Show @n
 *      각 항목의 최소 갯수만 보여주며, 각 항목 View All 클릭 시 전체 항목 Show
 */
class MusicSearch : public QWidget
{
    Q_OBJECT
public:
    explicit MusicSearch(QWidget *parent = nullptr);
    void search(const QString &p_text);
    void musicSearch(const QString &p_text);//cheon211008

signals:
    void signal_clickedViewAll(const QString &p_searchText, const QString &p_viewAllMode);
    void signal_changedMenuName(const QString &p_menuName,const QString &menu );

private slots:
    void slot_delegateClicked(const int &p_index, const int &p_btnType);
    void slot_thumbnailDownloaded();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_playlistClicked(const int &p_playlistno);
    void slot_newPlaylistNameEdited(QString);
    void slot_clickedViewAll();

private :
    Linker *linker;
    QString strSearchWord;

    QVBoxLayout *boxContents;

    Proc_AddTackInPlayList *proc_addPlaylist;

    // empty
    QWidget *widgetEmpty;

    // contents
    QScrollArea *scrollArea_content;

    // Tracks
    FileDelegate *delegateTrack;
    QListWidget *listWidgetTrack;
    QWidget *widgetTrack;
    QList<DataPopup*> *listDataTrack;

    // Album
    QHBoxLayout *boxAlbum;
    QWidget *widgetAlbum;

    // Artist
    QHBoxLayout *boxArtist;
    QWidget *widgetArtist;

    QLabel *labelSubTitle_1;//cheon211115-01
    QLabel *labelSubTitle_2;//cheon211115-01
    QLabel *labelSubTitle_3;//cheon211115-01

    void setInit();
    void setUIControl();
    void setTrackUI();
    void setAlbumUI();
    void setArtistUI();

    void setTrack();
    void setAlbum();
    void setArtist();
    bool checkSearchResult();
    void showEmptyLabel(bool);
    QJsonArray searchTrack();
    QJsonArray searchAlbum();
    QJsonArray searchArtist();
    void requestPlayMusic(const int &p_index);
    void requestShowOptionPopup(const int &p_index);
    QJsonObject getJsonObject_OptionPopup(const int &p_index);
    QJsonArray getJsonArr_TrackData(const int &p_index);
    QJsonObject getJsonObject_TrackData(const int &p_index);
    void setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor);
    void changeTrackFavoritesUI(const int &p_index);
    void requestGetTrackFavorites(const int &p_index);
    void requestSetTrackFavorites(const int &p_index);
    void setResultOfGetFavorite(const int &p_index, const QJsonObject &p_jsonData);
    void setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData);

};

#endif // MUSICSEARCH_H
