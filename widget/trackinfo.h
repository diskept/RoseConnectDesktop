#ifndef TRACKINFO_H
#define TRACKINFO_H

#include "clickablelabel.h"

#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include <data/datapopup.h>
#include <common/filedownloader.h>
#include <common/linker.h>
#include "widget/optionpopup.h"

/**
 * @brief The TrackInfo class 아티스트 상세화면에서 사용하는 노래 한개의 Track 정보 위젯 [View용]
 */
class TrackInfo : public QPushButton
{
    Q_OBJECT
public:
    explicit TrackInfo(OptionPopup::TypeMenu p_typeMenu, QWidget *parent = nullptr);
    ~TrackInfo();

    void setDataTrackInfo(DataPopup *p_data, QString p_albumImg);

signals:
    void signal_clickedAddPlaylist();

public slots:
    void slot_loadImage();
    void slot_showOptionPopup();

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_requestTrackFavorite();
    void slot_clickedPlay();

private:
    Linker *linker;
    DataPopup *dataPopup;
    QString albumImg = "";
    /// 앨범에 있는 노래 곡 수
    QString numsongs = "";
    /// 좋아요 여부
    bool isFavorite = false;

    OptionPopup::TypeMenu typeMenu_forOptionPopup;

    FileDownloader *fileDownLoader;
    QLabel *lb_albumImg;
    QLabel *lb_title;
    QLabel *lb_artist;
    QLabel *lb_albumName;
    QLabel *lb_duration;
    QPushButton *btn_list_fav_icon;

    void requestIsfavorite();
    void imageDownloading();
    QJsonObject getJsonObject_TrackData();

    bool flagNeedDraw = true;

    void paintEvent(QPaintEvent *event) override{
        Q_UNUSED(event);

        if(flagNeedDraw){
            flagNeedDraw = false;
            imageDownloading();
        }
    }
};

#endif // TRACKINFO_H
