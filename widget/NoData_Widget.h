#ifndef NODATA_WIDGET_H
#define NODATA_WIDGET_H

#include <QLabel>
#include <QWidget>

/**
 * @brief 데이터가 없음을 표현하는 Widget.
 * @details Bugs, Tidal 에서 사용
 */
class NoData_Widget : public QWidget
{
    Q_OBJECT
public:

    enum NoData_Message{
        Unknown                     ///< 모름. 일반 타입
        , FilterOptionBox           ///< 필터링 옵션 박스에 옵션 정보가 없을 때
        , Album_NoData
        , Playlist_NoData
        , Track_NoData              ///< 트랙 목록에 데이터 없을 때
        , Artist_NoData
        , PD_Album_NoData
        , MyAlbum_NoData
        , MyPlaylist_NoData
        , Video_NoData
        , Rosetube_NoData
        , Subscription_NoData
        , SearchResult_NoData
        , CDplay_NoData//c220721
        , Radio_NoData

    };
    Q_ENUM(NoData_Message);


    explicit NoData_Widget(const NoData_Message, QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *event) override;
    void setText_forNoData(const NoData_Message);


signals :
    void signal_mousePressed(const Qt::MouseButton);

private:
    // functions
    void setUIControl();

    // UI
    QLabel *label_msg;

};

#endif // NODATA_WIDGET_H
