#ifndef PROFILEPLAYLISTWIDGET_H
#define PROFILEPLAYLISTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

/** ===================================================================================================
 *              class ProfileRowBtn
 * ===================================================================================================
 */
/**
 * @brief The ProfileRowBtn class 프로필 닉네임 버튼
 */
class ProfileRowBtn : public QPushButton
{
    Q_OBJECT
public:
    explicit ProfileRowBtn(QWidget *parent = nullptr);
    void setDataProfileBtn(QJsonObject p_jsonObject);
    void setMinSize();
    void setMaxSize();
    void setToggle();
    bool getFlagExpaneded(){ return this->flagExpanded; } ///< 접었다 펼쳐졌다 여부 반환
signals:

private slots:
    //void slot_clickedArrowBtn();
    void slot_loadImageMainProfile();

private:
    bool flagExpanded = false;         ///< 접었다 펼쳐졌다 여부

    QLabel *lb_img;
    QLabel *lb_username;
    QPushButton *btn_open;
    QPushButton *btn_close;

    void setUIControl();
};

/** ===================================================================================================
 *              class PlayListRowBtn
 * ===================================================================================================
 */
/**
 * @brief The PlayListRowBtn class 플레이리스트명 Row 버튼
 */
class PlayListRowBtn : public QPushButton
{
    Q_OBJECT
public:
    explicit PlayListRowBtn(QWidget *parent = nullptr);
    void setDataPlayListRowBtn(QJsonObject p_jsonObject);
    void setMinSize();
    void setMaxSize();
    int getPlayListCountTrack(){ return this->dataPlayListRow.totalcount; }
    int getPlayListNoPK(){ return this->dataPlayListRow.no; }
    QString getPlayListType(){ return this->dataPlayListRow.type; }

signals:

private:
    struct DataPlayListRow{
      int no = 0;           ///< 플레이리스트 PK
      QString title = "";   ///< 플레이리스트 명
      QString type = "";    ///< 플레이리스트 type (음악/로즈튜브 등등)
      int totalcount = 0;   ///< 플레이리스트의 트랙 곡 총 수
    };
    DataPlayListRow dataPlayListRow;
    QLabel *lb_icon;
    QLabel *lb_title;
    void setUIControl();

};
/** ===================================================================================================
 *              class ProfilePlayListWidget
 * ===================================================================================================
 */
/**
 * @brief The ProfilePlayListWidget class 프로필 버튼 과 플레이리스트 포함된 클래스
 */
class ProfilePlayListWidget : public QWidget
{
    Q_OBJECT
public:
    enum ProfileWhoType{
        My,     ///< 마이 플레이리스트 프로필
        Friend  ///< 친구 플레이리스트 프로필
    };

    explicit ProfilePlayListWidget(ProfileWhoType p_profileWho,QWidget *parent = nullptr);
    void setDataProfileInfo(QJsonObject p_jsonObject);
    void setMinWidthSize();
    void setMaxWidthSize();
    void setToggle();
    //ProfileWho getProfileWho(){ return this->profileWho; }
    //int getPKNo(){ return this->pkNo;    }              ///< PK 반환

signals:
    void signal_clickedPlayListTitle(const QJsonObject &p_jsonObject);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedProfileRowBtn();
    void slot_clickedPlayListRowBtn();
    void slot_clickedMore();

private:
    ProfileWhoType type_profileWho;     ///< 마이 플레이냐 친구 플레이냐 유형값
    int pkNo;                           ///< 플레이리스트 PK 값
    int totalCount = 0;                 ///< 플레이리스트 총 개수 (더보기 때문에 필요함)
    int cntMoreClicked = 0;                    ///< 더보기 버튼 클릭 횟수

    ProfileRowBtn *profileRowBtn;
    QVBoxLayout *vl_playList;
    QPushButton *btn_list_open_ico;
    QList<PlayListRowBtn*> list_playListRowBtn;

    void setUIControl();
    void setAddPlayListRowBtn(QJsonObject p_jsonObject);

    //void resizeEvent(QResizeEvent *event) override;
};

#endif // PROFILEPLAYLISTWIDGET_H
