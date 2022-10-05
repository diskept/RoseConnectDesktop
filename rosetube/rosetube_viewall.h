#ifndef ROSETUBE_VIEWALL_H
#define ROSETUBE_VIEWALL_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <common/linker.h>
#include <widget/flowlayout.h>

/**
 * @brief 로즈튜브 View All @n
 *      각 항목별 로즈튜브 영상 목록으로 구성
 */
class RoseTube_ViewAll : public QWidget
{
    Q_OBJECT
public:

    explicit RoseTube_ViewAll(QWidget *parent = nullptr);
    void setData(const QJsonObject &p_jsonObject);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedPlayAll();
    void slot_clickedPlayShuffle();
    void slot_myPlaylistChanged();

private:

    Linker *linker;

    QWidget *widget_option;
    QWidget *wg_ScrollArea;
    QScrollArea *scrollArea;
    FlowLayout *flowLayout;

    int exploreIndex=-1;
    QString type;
    QString title;
    bool flagReqMore = false;
    int totalCount = 0;
    int current_count = 0;

    void setInit();
    void setUIControl();

    void requestData();
    void requestRecentPlay();
    void requestMyPlayList();
    void requestFriendPlayList();
    void requestRosetubeBest(int list_no);
    void requestRecommendList();
    void requestRecommendGenre();
    void requestEXPLORE();
    void requestRoseCommon();
    void requestFavorTracks();
    void requestPlayAll(bool p_flagShuffle=false);

    void appendRecentPlay(const QJsonObject &p_jsonObject);
    void appendMyPlayList(const QJsonObject &p_jsonObject);
    void appendFriendPlayList(const QJsonObject &p_jsonObject);
    void appendRosetubeBest(const QJsonObject &p_jsonObject);
    void appendRecommendList(const QJsonObject &p_jsonObject);
    void appendRecommendGenre(const QJsonObject &p_jsonObject);
    void appendEXPLORE_RoseTube(const QJsonObject &p_jsonObject);
    void appendRoseCommon(const QJsonObject &p_jsonObject);

    void setResultOfRecentPlay(const QJsonObject &p_jsonObject);
    void setResultOfMyPlayList(const QJsonObject &p_jsonObject);
    void setResultOfFriendPlayList(const QJsonObject &p_jsonObject);
    void setResultOfRosetubeBest(const QJsonObject &p_jsonObject);
    void setResultOfRecommendList(const QJsonObject &p_jsonObject);
    void setResultOfRecommendGenre(const QJsonObject &p_jsonObject);
    void setResultOfEXPLORE(const QJsonObject &p_jsonObject);
    void setResultOfRoseCommon(const QJsonObject &p_jsonObject);

    void wheelEvent(QWheelEvent *event) override;
};

#endif // ROSETUBE_VIEWALL_H
