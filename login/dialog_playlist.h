#ifndef DIALOG_PLAYLIST_H
#define DIALOG_PLAYLIST_H


#include <QDialog>
#include <QVBoxLayout>

namespace Dialog {

/**
 * @brief 플레이리스트 선택 Dlg @n
 *      신규 플레이리스트, 구 플레이리스트 선택 가능, 신규 플레이리스트 선택 시 @sa Dialog_NewPlaylist Show @n
 *      @sa Dialog_Playlist::PlayListType enum 으로 음악,로즈튜브 구분 @n
 */
class Dialog_Playlist : public QDialog
{
    Q_OBJECT
public:

    enum PlayListType{
        YOUTUBE,
        MUSIC
    };

    explicit Dialog_Playlist(PlayListType playlistType, QWidget *parent = nullptr);
    void requestMyPlayList();

signals:
    void signal_clickedPlaylist(const int &p_playlistno);

private slots:
    //void slot_clickedOk();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedPlaylist();

private :

    const int DLG_WIDTH = 600;
    const int DLG_HEIGHT = 500;

    const int HTTP_NETWORK_MY_PLAYLIST_YOUTUBE = 94;
    const int HTTP_NETWORK_MY_PLYLIST_MUSIC = 93;

    PlayListType playListType;

    QVBoxLayout *box_playList;

    void setInit();
    void setUIControl();
    void setInitPlayList();
    void setResultOfMyPlayList_Music(const QJsonObject &p_jsonObject);
    void setResultOfMyPlayList_Youtube(const QJsonObject &p_jsonObject);

    void appendPlayListUI(int p_playlistNo, QString p_name, QString p_thumbPath, int p_numContents);

};
};

#endif // DIALOG_PLAYLIST_H
