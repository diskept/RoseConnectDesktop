#ifndef SETTINGMYPAGE_H
#define SETTINGMYPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <qstackedwidget.h>
#include <qlineedit.h>

#include <common/filedownloader.h>
#include <common/linker.h>

#include "roseHome/rosehome_struct.h"

#include <widget/myphotoedit.h>

#include <widget/flowlayout.h>
#include "setting_struct.h"
#include <QFile>

/**
 * @brief The SettingAppMenu class 설정 > 마이페이지 @n
 *      내 정보 관리및 로그아웃, 음악/로즈튜브 마이플레이리스트 확인 관리 가능
 */
class SettingMyPage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingMyPage(QWidget *parent = nullptr);
    ~SettingMyPage();

    void setDataMyPage();

signals:
    void signal_changeMyInfoData();
    void signal_clickedViewAll_music(QString p_viewAllMode);
    void signal_clickedViewAll_rosetube(const QJsonObject &p_jsonObject);
    void signal_uploadedMemberImage();


private slots:
    void slot_getMyInfo_profileImageDelete();
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_applyResult_myMusicPlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);
    void slot_applyResult_myRosetubePlaylist(const QList<roseHome::PlaylistItemData>&, const QJsonArray&, const bool);

    void slot_showLoginPage();
    void slot_getMyInfo_loginAfter();
    void slot_getMyInfo_AfterMemberImageUploaded();
    void slot_loadImageMainProfile();
    void slot_loadImagePhotos();
    void slot_changePageEdit();
    void slot_logout();
    void slot_saveMyInfo();
    void slot_deleteMyInfo();
    void slot_editImage();
    void slot_duplicateNickName();
    void slot_addPhoto();
    void slot_addPhoto_member_Profile();
    void uploadphotoImgToFile(const QString &p_playUrl);
    void uploadphotoImgToFile_member_Profile(const QString &p_playUrl);
    void slot_deletePhoto(QString p_imgPath);
    void slot_clickedViewAll_music();
    void slot_clickedViewAll_rosetube();

    void slot_myPlaylistChanged();
    void slot_change_device_state(const QString&);

    void slot_networkManagerFinished();
    void slot_replyFinished();

private:

    QString del_imgURL_main;
    int member_id;
    QFile *fileUpload;
    Linker *linker;

    setting::ProfileInfo data_profileInfo;  ///< 데이터 struct

    QWidget *widget_loginBefore;
    QWidget *widget_loginAfter;
    QStackedWidget *stackWidget_loginAfter;
    QLabel *lb_profile;
    QLabel *lb_profile_grade;
    QLabel *lb_nickName;
    QLabel *lb_comment;    
    QHBoxLayout *hl_photo;

    QString tmp_password;

    // 음악 플레이리스트
    QLabel *lb_emptyMyPlayList_music;
    QHBoxLayout *boxMyPlayList_music;
    QStackedWidget *stackMyPlayList_music;

    // 로즈튜브 플레이 리스트
    QLabel *lb_emptyMyPlay_rosetube;
    QHBoxLayout *boxMyPlay_rosetube;
    QStackedWidget *stackMyPlay_rosetube;
    //QWidget *wg_myPlay;


    QLabel *lb_profileEdit;
    QLabel *lb_email;
    QLineEdit *le_nickName;
    QLineEdit *le_comment;
    QLineEdit *le_pw;
    QLabel *lb_title_regPhoto;
    QWidget *widget_flow;
    FlowLayout *flowlayout_photos;
    QVBoxLayout *vl_total_withoutMyInfo;
    QList<MyPhotoEdit*> list_myPhotoEdit;

    void setUIControl();
    QWidget* setUIControl_loginBefore();
    QWidget* setUIControl_loginAfter();
    QWidget* setUIControl_loginAfter_myInfoView();
    QWidget* setUIControl_loginAfter_myInfoEdit();
    QWidget* setUIControl_playListMusic();
    QWidget* setUIControl_playListRoseTube();
    void setResponseProfile_memberImageUploaded(QJsonObject p_jsonObject);
    void setResponseProfile_profileImageUploaded(QJsonObject p_jsonObject);
    void setResponseProfile(QJsonObject p_jsonObject);
    void setResponseNickNameCheck(QJsonObject p_jsonObject);
    void setResponseDeletePhoto(QJsonObject p_jsonObject);
    void appendPlayListMusic(const QJsonObject &p_json);
    void appendPlayListRosetube(const QJsonObject &p_json);
    void requestSetRoseUserInfo(QString p_userName, QString p_email, QString p_accesskey);
    void requestMusicMyPlaylist();
    void requestRosetubeMyPlaylist();
    void setLogout();
    QPushButton* getUIAddPhotoWidget();
    void logoutDBClear();
};

#endif // SETTINGMYPAGE_H
