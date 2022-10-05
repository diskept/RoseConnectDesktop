#ifndef MUSICFOLDER_H
#define MUSICFOLDER_H

#include "musicfolder_usb.h"
#include "musicfolder_usbfilelist.h"
#include "musicfolder_filelist.h"
#include "common/linker.h"

#include <home/abstractpage.h>
#include <home/topmenubar.h>

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QScrollArea>
#include <QJsonObject>


/**
 * @brief The MusicFolder class 음악 > 폴더 메뉴 화면
 * @details 크게 2개의 위젯을 StackWidget으로 관리하고 있다. 2개의 위젯은 다음과 같이 구성되어있다.
 * 하나는 탐색(USB)관련 화면의 위젯이고 다른 하나는 즐겨찾기관련 화면의 위젯이다.
 * @note 즐겨찾기 각각 버튼 클릭할때 마다 페이지가 변경되었다는 시그널을 발생시킨다.
 * 탐색(USB) depth가 늘어날때마다 페이지가 변경되었다는 시그널을 발생시킨다.
 */
class MusicFolder : public AbstractPage
{
    Q_OBJECT

public:
    explicit MusicFolder(QWidget *parent, bool flagIsMusic = true);
    ~MusicFolder();

    void setDataABSPage(QJsonObject p_data) override;
    void hide_topBtnFavorite();
    void setUIControl_USB();

signals:
    void signal_clickedFavoriteFolderEdit(const QJsonArray &p_jsonArrFavor);

public slots:
    void slot_changedSubTab_favorite();             ///< 즐겨찾기 버튼 클릭시 페이지 변경
    void slot_changedSubTabUI(const QJsonObject &p_data) override;
    void slot_goToInitiPageFolder(QString p_subMenuCode);

private slots:
    void slot_clickedBtnEditFavorite();         ///< 즐겨찾기 편집 아이콘 클릭시
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_openNetworkFolderFileList(const QString &p_contentStep, const QString &p_rootName, const QString &p_path);
    void slot_openUSBFileList(const QString &p_contentStep, const QJsonObject &p_jsonObject);
    void slot_favoriteFolderChanged();

private :

    bool flagIsMusic;
    Linker *linker;

    QJsonArray jsonArrFavor;

    QScrollArea *scrollFavorite;
    QPushButton *btn_favEdit;
    QHBoxLayout *hl_btnFavorite;
    QHBoxLayout *hl_favoriteGroup;
    QWidget *widget_favoriteGroup;

    QPushButton *btn_sort_total;
    QPushButton *btn_sort_artist;
    QPushButton *btn_sort_composer;
    QVBoxLayout *vl_songs;
    QVBoxLayout *vl_total;

    QStackedWidget *stackedwidget;
    MusicFolder_USB *music_usb;
    MusicFolder_FileList *music_file;
    MusicFolder_USBFileList *music_usbFile;

    void resizeEvent(QResizeEvent* event) override;
    void setInit();
    void setUIControl();
    void setUIFarovite(QWidget *parent);
    void setResultOfFavoriteList(QJsonObject p_jsonObject);

    void setUIControl_favorite(QString p_favoriteType, QString p_favName, QString p_albumFavPath);
    void setUIControl_usbFileList(QString ,QJsonObject);
    void requestBtnListFavorite();
    void setGeometryFavoriteUI();



    virtual QJsonObject getDataJson() override;
};

#endif // MUSICFOLDER_H
