#ifndef FRIENDPROFILEITEMROSEHOME_H
#define FRIENDPROFILEITEMROSEHOME_H

#include <QLabel>
#include <QWidget>

/**
 * @brief 친구관리 메뉴에서 사용하는 프로필Row
 */
class FriendProfileItemRoseHome : public QWidget
{
    Q_OBJECT
public:
    enum ProfileBtnType{
      FriendList,   ///< 친구목록
      Receive,      ///< 받은요청
      Send,         ///< 보낸요청
      Find          ///< 친구찾기
    };
    explicit FriendProfileItemRoseHome(ProfileBtnType p_type, QWidget *parent = nullptr);
    void setDataProfileItem(QJsonObject p_jsonObject);

signals:
    void signal_removeItem();

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedUnConnect();   ///< 해제 클릭시
    void slot_clickedReject();      ///< 거절 클릭시
    void slot_clickedAccept();      ///< 수락 클릭시
    void slot_clickedCancel();
    void slot_clickedRequest();
    void slot_loadImageMainProfile();

private:
    ProfileBtnType profileBtnType;

    QLabel *lb_profile;
    QLabel *lb_profile_grade;
    QLabel *lb_username;
    QLabel *lb_bio;
    QLabel *lb_userid;//cheon211018

    void setUIControl();
};

#endif // FRIENDPROFILEITEMROSEHOME_H
