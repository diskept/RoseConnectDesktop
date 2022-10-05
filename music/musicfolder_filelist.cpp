#include "musicfolder_filelist.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/gscommon.h"
#include <QDebug>
#include <QJsonObject>
#include <QPixmap>
#include "thread/mediaplayerthread.h"
#include "widget/toastmsg.h"
#include "login/dialogladd_network.h"
#include "login/dialogadd_favoritefolder.h"
#include <QDateTime>
#include <QUuid>
#include <QMetaObject>
#include <QJsonObject>
#include <QJsonArray>


/**
 * @brief MusicFolder_FileList::MusicFolder_FileList : 음악-폴더-네트워크 파일 목록 화면
 * @param parent
 * @note MediaFileTree 항상 생성하는 구조로 변경 @n
 *      QFileSystemModel 에서 파일이 많은 폴더 중복해서 호출하는 경우 Lock 발생
 */
MusicFolder_FileList::MusicFolder_FileList(QWidget *parent, bool p_flagIsMusic)
    : QWidget(parent)
    , flagIsMusic(p_flagIsMusic)
{

    setInit();
    setUIControl();
}


MusicFolder_FileList::~MusicFolder_FileList(){

    this->deleteLater();
}


/**
 * @brief MusicFolder_FileList::setInit : 초기 세팅
 */
void MusicFolder_FileList::setInit(){

}


/**
 * @brief MusicFolder_FileList::setUIControl : UI 세팅
 */
void MusicFolder_FileList::setUIControl(){

    topDirPath = new Folder_TopDirPath;
    mediaFileTree = nullptr;

    boxContents = new QVBoxLayout;
    boxContents->setSpacing(0);
    boxContents->setContentsMargins(0,0,0,0);
    boxContents->setAlignment(Qt::AlignTop);
    boxContents->addWidget(topDirPath);
    //boxContents->addWidget(mediaFileTree);

    setLayout(boxContents);

    // 커넥트
    connect(topDirPath, SIGNAL(signal_changedDirPath(QString, QString)), this, SLOT(slot_changedRootPath(QString, QString)));
    //connect(mediaFileTree, SIGNAL(signal_clickedDirName(QString, QString)), this, SLOT(slot_changeTopDirName(QString, QString)));
}


/**
 * @brief MusicFolder_FileList::setData : 데이터 세팅
 * @param p_rootName QString 루트 디렉토리명
 * @param p_rootPath QString 루트 디렉토리 경로
 */
void MusicFolder_FileList::setData(const QString &p_rootName, const QString &p_smbPath){

    topDirPath->clearDirName();

    if(mediaFileTree != nullptr){
        boxContents->removeWidget(mediaFileTree);
        mediaFileTree->deleteLater();
    }
    mediaFileTree = new MediaFileTree(this,flagIsMusic);
    boxContents->addWidget(mediaFileTree);

    connect(mediaFileTree, SIGNAL(signal_clickedDirName(QString, QString)), this, SLOT(slot_changeTopDirName(QString, QString)));

    mediaFileTree->setData(p_rootName, p_smbPath);
}



/**
 * @brief MusicFolder_FileList::slot_changedRootPath : [슬롯] Root Path 변경
 * @details Path 변경 처리<br>
 * 목록 초기화, FieSystemModel RootPath 변경
 * @param p_dirName
 * @param p_dirPath
 */
void MusicFolder_FileList::slot_changedRootPath(const QString &p_dirName, const QString &p_dirPath){

    if(mediaFileTree != nullptr){
        boxContents->removeWidget(mediaFileTree);
        mediaFileTree->deleteLater();
    }
    mediaFileTree = new MediaFileTree(this,flagIsMusic);
    boxContents->addWidget(mediaFileTree);

    connect(mediaFileTree, SIGNAL(signal_clickedDirName(QString, QString)), this, SLOT(slot_changeTopDirName(QString, QString)));

    mediaFileTree->slot_changedRootPath(p_dirName, p_dirPath);
}


/**
 * @brief MusicFolder_FileList::slot_changeTopDirName [SLOT] 상단 디렉토리명 변경
 * @param p_dirName
 * @param p_dirPath
 */
void MusicFolder_FileList::slot_changeTopDirName(QString p_dirName, QString p_dirPath){

    this->topDirPath->addDirName(p_dirName, p_dirPath);
}
