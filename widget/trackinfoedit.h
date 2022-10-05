#ifndef TRACKINFOEDIT_H
#define TRACKINFOEDIT_H

#include "clickablelabel.h"

#include <QLabel>
#include <QWidget>

#include <data/datapopup.h>

#include <common/linker.h>

/**
 * @brief The TrackInfo class 앨범상세화면에서 사용하는 노래 한개의 Track 정보 위젯 (편집모드 존재)
 */
class TrackInfoEdit : public QPushButton
{
    Q_OBJECT
public:
    explicit TrackInfoEdit(QWidget *parent = nullptr);
    void setDataTrackInfoEdit(DataPopup *p_data, QString p_albumImg);
    void setDataTracksInfoEdit(QList<DataPopup*> list_info);

signals:
    void signal_clickedAddPlaylist();

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_showOptionPopup();
    void slot_requestTrackFavorite();
    void slot_clickedPlay();

private:
    Linker *linker;
    QLabel *lb_title;
    QLabel *lb_artist;
    QLabel *lb_duration;
    /// 좋아요 여부
    bool isFavorite = false;

    QString albumImg;
    QPushButton *btn_list_fav_icon;
    DataPopup *dataPopup;
    QList<DataPopup*> list_audioInfo;

    void requestIsfavorite();
    QJsonObject getJsonObject_TrackData();

};

#endif // TRACKINFOEDIT_H
