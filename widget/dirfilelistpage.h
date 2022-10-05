#ifndef DIRFILELISTPAGE_H
#define DIRFILELISTPAGE_H

#include "clickablelabel.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListView>

#include <QLabel>

#include <QtMultimedia/QMediaPlayer>
#include "common/linker.h"
#include "common/sqlitehelper.h"

/**
 * @brief The ClickableDirRow class DirFileListPage클래스에서 사용하는 클릭가능한 Dir 의 Row 클래스
 * @note 미사용
 */
class ClickableDirRow : public QWidget
{
    Q_OBJECT
public:
    explicit ClickableDirRow(QString p_iconPath, QString p_name, QString p_path, QWidget *parent = nullptr);
signals:
    void signal_clicked(QString p_dirName, QString p_dirPath);
private:
    void mousePressEvent(QMouseEvent *event) override;
    QString dirName;
    QString dirPath;
};

/**
 * @brief The LocalFileRow class DirFileListPage에서 사용하는 File 의 Row 클래스
 * @note QFileInfo 의 메타정보(QMediaPlayer)를 이용하여 썸네일 과 파일정보 가져와서 세팅한다.
 */
class LocalFileRow : public QWidget //c220609
{
    Q_OBJECT
public:
    explicit LocalFileRow(QFileInfo p_fileInfo, QWidget *parent = nullptr);
    //~LocalFileRow();

signals:
    void signal_clicked(QString index);
private slots:
    void slot_showOptionPopup();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    void mousePressEvent(QMouseEvent *event) override;
    QString dirNamem, fileInfo_filePath;
    QString dirPath;
    QMediaPlayer *m_player;
    QFileInfo fileInfo;

    QLabel *lb_albumImg;
    QLabel *lb_title;
    QLabel *lb_artist;
    QLabel *lb_albumName;
    QLabel *lb_duration;

};


class ListFileView : public QListView
{
    Q_OBJECT
public:
    explicit ListFileView(QWidget *parent = nullptr);
signals:
    void signal_setUI(const QModelIndex &index);
public slots:
    void setRootIndex(const QModelIndex &index) override;
};

/**
 * @brief The DirFileListPage class Dir명과 파일명이 보이는 페이지
 * @note (음악>폴더 와 비디오>폴더 에서 사용함)
 * QTreeView와 QListView 는 데이터 추출용도로만 사용하고 실제 화면에는 세팅하지 않는다.
 */
class DirFileListPage : public QWidget
{
    Q_OBJECT
public:
    enum ValiedFileType{
        /// 오디오 파일
        Audio,
        /// 비디오 파일
        Video
    };
    explicit DirFileListPage(ValiedFileType p_fileType, QWidget *parent = nullptr);
    void setInitDriver();
    void setInitNetworkDriver(const QString &p_hostName, const QString &p_path);
signals:
    void signal_clickedDirName(QString p_dirName, QString p_dirPath);
    void signal_fileInfo_filePath_play(QString p);//c220609

public slots:

    void slot_playlist_reflesh();
    void slot_clickedDirName(QString p_dirName, QString p_dirPath);
    void slot_clickedFileName(QString p_fileInfo_filePath);//c220609

private slots:

    void slot_directoryLoadedFile(const QString &path);
    void slot_setUI(const QModelIndex &index);

private:
    Linker *linker;
    int curr_playIndex = -1, pre_playIndex = -1;
    //LocalFileRow *localFileRow;
    QString curr_fileInfo_filePath;
    QTreeView *treeView;
    //QListView *listView;
    ListFileView *listView;
    QFileSystemModel *dirmodel;
    QFileSystemModel *filemodel;
    QModelIndex currentQModelIndex;//c220704

    QVBoxLayout *vl_list;
    QList<QString> fileList;

    int select_table(QString str_table);
    void insert_table(int row, QString tmp_filePath, QString str_table);
    void dt_playlist_insertDB(int index, QString tmp_filePath);
};

#endif // DIRFILELISTPAGE_H
