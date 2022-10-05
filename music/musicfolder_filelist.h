#ifndef MUSICFOLDER_FILELIST_H
#define MUSICFOLDER_FILELIST_H

#include <QWidget>
#include <QFileSystemModel>
#include "widget/folderwidgetgroup.h"
#include "widget/customlistwidget.h"
#include "widget/mediafiletree.h"
#include "delegate/filedelegate.h"
#include "thread/mediaplayerthread.h"
#include <QUuid>
#include <QMediaPlayer>

/**
 * @brief 음악-폴더의 네트워크 파일목록 화면 @n
 *      @sa MusicFolder_FileList 생성자 param bool 값으로 음악,비디오 구분 @n
 *      비디오의 경우 재생 기능만 지원 (플레이리스트담기, 하트, 재생목록 추가  미지원)
 *
 * @note use 네트워크 폴더 파일 목록, 즐겨찾기 폴도 파일 목록
 */
class MusicFolder_FileList : public QWidget
{
    Q_OBJECT
public:
    explicit MusicFolder_FileList(QWidget *parent = nullptr, bool flagIsMusic=true);
    ~MusicFolder_FileList();
    void setData(const QString &p_rootName, const QString &p_rootPath);

private slots:
    void slot_changedRootPath(const QString &p_dirName , const QString &p_dirPath);
    void slot_changeTopDirName(QString p_dirName, QString p_dirPath);

private:

    bool flagIsMusic;

    QVBoxLayout *boxContents;
    Folder_TopDirPath *topDirPath;
    MediaFileTree *mediaFileTree;


    void setInit();
    void setUIControl();

};

#endif // MUSICFOLDER_FILELIST_H
