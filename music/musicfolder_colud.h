#ifndef MUSICFOLDER_COLUD_H
#define MUSICFOLDER_COLUD_H

#include "common/linker.h"

#include "data/datacloud.h"

#include "delegate/cloudListDelegate.h"

#include "widget/OptMorePopup.h"
#include "widget/folderwidgetgroup.h"

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>


/**
 * @brief 음악 > 폴더 > 클라우드 @n
 *      클라우드 추가, 등록된 클라우드 목록으로 구성
 *
 * @note
 *      클라우드 상세 미구현 ==> API 확인 필요(Android App에서 음원재생 불가, 폴더 클릭시 같은 폴더 무한 이동)
 */
class MusicFolder_Colud : public QWidget
{
    Q_OBJECT
public:
    explicit MusicFolder_Colud(QWidget *parent = nullptr, bool flagIsMusic = true);
    ~MusicFolder_Colud();

    void requestCloudlist(bool flagRefresh = false);
    void reqeustCloudlist_changePath(QString cloudName, QString cloudPath);

signals:
    void signal_clickedHost(QString, QString);
    void signal_clickedDirName(QString, QString);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_clickedRow();
    void slot_clickedRemove();

    void slot_delegateClicked(const bool &isFile, const int &p_index, const int &p_btnType);

    void slot_clickedPlayAll();
    void slot_clickedPlayShuffle();

    // about OptMorePopup
    void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int);

private:
    void setInit();
    void setUIControl();
    void setInitMediaSrcData();

    void clearCloudList();
    void appendCloudUI(const int &p_index);
    void appendCloudUIFolder(const int &p_index);
    void appendCloudUIFile(const int &p_index);

    void requestRemoveCloud(const int &p_index);

    void setResultOfCloudList(const QJsonObject &p_json);

    void showDlgOfSelectCloudType();
    void showDlgOfAddCloud(bool p_flagIsFTP = true);

    void showOptionPopup(int p_index);

    QJsonArray getPlayMusicInfoArray(const int &p_index, const OptMorePopup::ClickMode clickMode = OptMorePopup::ClickMode::Play_RightNow);
    void playMusic(QJsonArray jsonArrMusic, int playType, bool flagPlayShuffle=false);

private:
    Linker *linker;

    Folder_ClickableRow *folder_addCloud;

    QWidget *widget_cloud;
    QVBoxLayout *box_cloudList;

    cloudListDelegate *delegate_cloud;
    QListWidget *listWidget;

    QList<DataCloud*> *listCloud;

    QPushButton *btn_play;
    QPushButton *btn_shuffle;

    QJsonArray jsonArr_tracks_toPlay;
    QJsonArray jsonArr_folder_step;

    QString cloudPath_buf = "";
    QString cloudName_buf = "";

    bool flagIsMusic = false;
    bool flagDirChange = false;
};

#endif // MUSICFOLDER_COLUD_H
