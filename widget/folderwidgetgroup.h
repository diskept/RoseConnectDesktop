#ifndef FOLDERWIDGETGROUP_H
#define FOLDERWIDGETGROUP_H

#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

const QString FOLDER_USB_DIR_CODE_INIT = "init";
const QString FOLDER_USB_DIR_CODE_DRIVER = "driver";
const QString FOLDER_USB_DIR_CODE_NETWORK = "network";
const QString FOLDER_USB_DIR_CODE_CLOUD = "cloud";

/**
 * @brief The Folder_TopDirName class 상단 Dir 이름 클릭가능한 클래스 (폴더 아이콘과 폴더이름)
 * @note [음악>폴더], [비디오>폴더] 에서 사용
 */
class Folder_TopDirName : public QWidget
{
    Q_OBJECT
public:
    explicit Folder_TopDirName(QString p_dirName, QString p_dirPath, QWidget *parent = nullptr);
signals:
    void signal_clicked(QString p_dirName, QString p_dirPath);
private:
    QString dirName;
    QString dirPath;
    void mousePressEvent(QMouseEvent *event) override;
};














/**
 * @brief The Folder_TopDirPath class 상단 Dir 경로
 * @note [음악>폴더], [비디오>폴더] 에서 사용
 */
class Folder_TopDirPath : public QWidget
{
    Q_OBJECT
public:
    explicit Folder_TopDirPath(QWidget *parent = nullptr);
    ~Folder_TopDirPath();
    void addDirName(QString p_dirName, QString p_dirPath);
    void setRootDirName(QString p_dirName, QString p_dirPath);
    void clearDirName();
signals:
    void signal_changedDirPath(QString p_dirName, QString p_dirPath);
private slots:
    void slot_clickedDirName(QString p_dirName, QString p_dirPath);
private:
    QList<QHBoxLayout*> list_hlDirName;
    QList<Folder_TopDirName*> list_folderDirName;
    QHBoxLayout *hl_total;
    QWidget *widget_topTitle;
    QVBoxLayout *vl_total;
};


















/**
 * @brief The Folder_ClickableRow class 탐색 첫 화면의 클릭가능한 Row 클래스
 * @note [음악>폴더], [비디오>폴더] 에서 사용
 */
class Folder_ClickableRow : public QWidget
{
    Q_OBJECT
public:
    explicit Folder_ClickableRow(QString p_iconPath, QString p_text, QWidget *parent = nullptr);

signals:
    void signal_clicked();
private:


    void mousePressEvent(QMouseEvent *event) override;
};


















class FolderNetwork_ClickableRow : public QWidget
{
    Q_OBJECT
public:
    explicit FolderNetwork_ClickableRow(QString p_iconPath, QString p_text, QWidget *parent = nullptr);
    void setImage(QString p_iconPath);
signals:
    void signal_clicked();
    void signal_clickedRemove();
private:
    QLabel *lb_icon;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // FOLDERWIDGETGROUP_H
