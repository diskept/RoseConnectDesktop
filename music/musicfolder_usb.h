#ifndef MUSICFOLDER_USB_H
#define MUSICFOLDER_USB_H

#include <widget/folderwidgetgroup.h>
#include <widget/mediafiletree.h>
#include <music/musicfolder_network.h>
#include <music/musicfolder_colud.h>
#include <common/linker.h>
#include <QStackedWidget>
#include <QWidget>
#include <data/datanetworkfolder.h>
#include <QProcess>
#include <QScrollArea>
#include "widget/dirfilelistpage.h"//c220609
#include "thread/desktopmediaplayerthread.h"//c220609
#include "widget/dialogconfirm_macshare.h"//c220913_1

/**
 * @brief 음악 > 폴더 첫 화면 @n
 *      기본 메뉴 네트워크, 클라우드, 네트워크 폴더 추가 @n
 *      로즈기기에 연결된 USB 목록 그리고 등록된 네트워크 파일 목록으로 구성
 */
class MusicFolder_USB : public QWidget
{
    Q_OBJECT

public:
    explicit MusicFolder_USB(QWidget *parent = nullptr, bool flagIsMusic = true);
    ~MusicFolder_USB();
    void setData();
    void goInitPage();

signals:
    void clickedUSB(const QString &p_contentStep, const QJsonObject &p_jsonData);
    void clickedNetworkFolder(const QString &p_contentStep, const QString &p_rootName, const QString &p_albumPath);

    void clickedLocalNetworkFolder(const QString &p_contentStep, const QString &p_rootName, const QString &p_albumPath);//cheon210617-file

private slots:


    void slot_process_smb_ok(QString ip, QString id, QString pw, QString folderName);//c220913_1
    void slot_searchNetworkFolder();//c220906_1
    void slot_fileInfo_filePath_stop();//c220609
    void slot_fileInfo_filePath_play(QString index);//c220609
    void slot_clickedRow();
    void slot_requestNetworkFolderList();
    void slot_changeTopDirName(QString p_dirName, QString p_dirPath);
    void slot_changedRootPath(QString p_dirName, QString p_dirPath);
    void slot_clickedNetworkHost(const QString &p_ip, const QString &p_hostName);
    void slot_clickedCloudHost(const QString &cloudPath, const QString &p_cloudName);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedRemoveNetwork();
    void slot_clickedNetworkFolder();

    void slot_clickedRemoveLocalNetwork();//cheon210617-file

    void slot_requestLocalNetworkFolderList();//cheon210617-file
    void slot_clickedLocalNetworkFolder();//cheon210617-file
    void slot_localnetworkIPConnected();//cheon210617-file
    void slot_localerrorSocket();//cheon210617-file


    void slot_networkIPConnected();
    void slot_errorSocket();
    void slot_timePlayValue();//c220609
    void slot_responseHttp_volume(const int &p_id, const QJsonObject &p_jsonObject);//c220609


private:


    bool isPlaying = false;//c220609
    QTimer *timer;//c220609
    //QMediaPlayer *music_player;//c220609
    DirFileListPage *page_dirFileList;//c220609

    bool flagIsMusic;

    Linker *linker;

    QVBoxLayout *vl_list;
    QVBoxLayout *vl_usb;
    QVBoxLayout *vl_networkList;
    QVBoxLayout *vl_localList;//cheon210617-file
    QVBoxLayout *vl_total;

    /// 상단 dir Path
    Folder_TopDirPath *topDirPath;

    QStackedWidget *stackedWidget_page;
    /// Page : 초기 탐색 페이지
    //QWidget *page_init;
    QScrollArea *page_init;

    /// Page : dir과 file 정보
    MediaFileTree *page_fileTree;

    MusicFolder_Network *page_network;
    MusicFolder_Colud *page_cloud;

    /// 네트워크 폴더 데이터 리스트
    QList<DataNetworkFolder*> *listNetworkFolder;
    QList<DataNetworkFolder*> *listUsb;

    QVBoxLayout *vl_localnetworkList;//cheon210617-file
    QList<DataNetworkFolder*> *listLocalNetworkFolder;//cheon210617-file
    DialogConfirm_MacShare *dlgConfirmFinder;//c220913_1

    int listNetworkFolder_cnt = 0;//c220908_1

    void setInit();
    void setUIControl();

    void setResultOfNetworkFolderList(const QJsonObject &p_jsonObject);
    void appendNetworkFolderUI(const int &p_index);
    void requestRemoveNetworkFolder(const int &p_index);
    void setResultOfNetworkFolderRemove(const QJsonObject &p_jsonObject);
    bool checkAccessAbleFile(const QString &p_ip, const QString &p_path);
    void openNetworkDriveForWindow(const int &p_smbPath);
    void reCheckNetworkDriveOpened( const int &p_index, const QString &p_ip,  const QString &p_path);
    void dialog_comfirmNetworkfolder_forFinder();//cheon210831-network
    void dialog_comfirmNetworkfolder_forFinder_add();//cheon210831-network


    void setResultOfLocalNetworkFolderList(const QJsonObject &p_jsonObject);//cheon210617-file
    void appendLocalNetworkFolderUI(const int &p_index);//cheon210617-file
    void requestRemoveLocalNetworkFolder(const int &p_index);//cheon210617-file
    void setResultOfLocalNetworkFolderRemove(const QJsonObject &p_jsonObject);//cheon210617-file
    void openLocalNetworkDriveForWindow(const int &p_smbPath);//cheon210617-file
    void reCheckLocalNetworkDriveOpened( const int &p_index, const QString &p_ip,  const QString &p_path);//cheon210617-file

};

#endif // MUSICFOLDER_USB_H
