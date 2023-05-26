#ifndef ABSTRACTMAINCONTENT_H
#define ABSTRACTMAINCONTENT_H

#include "home/topmenubar.h"
#include "home/topnavibar.h"

#include "music/proc_addtackinplaylist.h"

#include "rosetube/proc_addrosetubeinplaylist.h"

#include "setting/leftmenubar.h"

#include "widget/customlineedit.h"//c220730
#include "widget/myqwidget.h"//c220729
#include "widget/optionpopup.h"
#include "widget/playfullscreenrelation.h"
#include "widget/rosetubebanner.h"
#include "widget/searchremotebar.h"

#include <QStackedWidget>
#include <QWidget>


/**
 * @brief The AbstractMainContent class 각 메인메뉴의 메인페이지의 최상위 클래스
 * @details 크게 3개 위젯으로 화면 구성 되어있음 (네비Bar,메뉴Bar,StackWidget) @n
 *      옵션팝업 위젯 가지고 있음
 */
class AbstractMainContent : public MyQWidget
{
    Q_OBJECT
public:
    explicit AbstractMainContent(QWidget *parent = nullptr);
    ~AbstractMainContent();

    void showhideRemoteWidget(bool flagShow);
    void resetCurrPlayData(const QJsonObject &p_jsonData);
    void changeCurrHdmiData(const bool flag);

    // 각 메뉴의 Main 클래스에서 search 검색어 처리하는 함수. 각 클래스에서 재정의
    virtual void process_searchWord(const QString&){ }


signals:
    void changedSubMenu(QString p_subMenuCode);
    void signal_getCurrPlayData();
    void showRemoteWidget(bool);
    void signal_changeMyInfo();
    void signal_mustChangeMainMenuAndMovePage(int p_mainMenuCode, const QJsonObject &p_data);       // added by sunnyfish

public slots:
    void goToMoveNewOrderPage(const QJsonObject &p_data);

protected slots:
    virtual void slot_overrideSigalSearch(bool);//c220730
    void slot_get_dragDropText();//c220730
    void slot_dropEvent_hide_show(bool show);//c220730
    void slot_returnPressed(const QString p_text);//c220730

    void goToBackPage();
    void goToNextPage();
    //void goToMoveNewOrderPage(const QJsonObject &p_data); // public으로 변경함.. HomeMain에서 사용하는부분이 생겨서
    void goToMoveNewOrderPage_subStep(const QJsonObject &p_data);

    virtual void slot_linker_connectedDevice(){   }                 ///< 자식 처리 필요한 경우 재정의 할것

    //void changedVolumnSlide(int p_value);

    // [virtual][SLOT] 앨범 hover 위 more/play/fav 아이콘을 클릭시 처리
    //void slot_clickedHoverItem(const QString &p_code, const QJsonObject &p_jsonData);
    void slot_clickedHoverItem(QString p_code, QJsonObject p_jsonData);
    virtual void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private slots:


    void slot_playlistClickedMusic(const int &p_playlistno);
    void slot_newPlaylistNameEditedMusic(QString);
    void slot_playlistClickedRosetube(const int &p_playlistno);
    void slot_newPlaylistNameEditedRosetube(QString);
    void slot_showBG(const QPixmap &pixmap);
    void slot_mustChangeMainMenuAndMovePage(int p_mainMenuCode, const QJsonObject &p_data);       // added by sunnyfish

    void slot_redirectUrl(const QString url);   //j220903 twitter

protected:
    CustomLineEdit *le_search_back;//c220730
    void make_CustomLineEdit();//c220730

    Linker *linker;

    int currMainMenuCode=-1;
    SearchRemoteBar *searchRemoteBar;//searchtextcopy
    int indexPageOrdering_curr = -1;        ///< indexPageOrdering_curr 순서대로 Page 저장된 것 중에서 현재 보고있는 Page Index
    QList<QString> *list_pageCodeOrdering;  ///< list_pageCodeOrdering 순서대로 Page 저장하기 위한 pageCode 값 리스트
    QList<QJsonObject> *list_pageDataOrdering;

    QPixmap *pixmap_roseTubeBG;
    bool flagSetRoseTubeBG = false;

    // ---------------------------------
    //  플레이리스트 담기
    // ---------------------------------
    Proc_AddTackInPlayList *proc_add_music;
    Proc_AddRosetubeInPlayList *proc_add_rosetube;

    QLabel *lb_BG;
    QLabel *lb_BGOpacity;

    // ---------------------------------
    // 로즈튜브 배너 :: 구독채널에서 사용
    // ---------------------------------
    QLabel *lb_roseTubeBG;
    QWidget *widget_roseTubeBG;

    TopNaviBar *topNaviBar;
    TopMenuBar *topMenuBar;

    QVBoxLayout *vl_total;
    QWidget *wg_total;
    QStackedWidget *stackedWidget_content;
   // SearchRemoteBar *searchRemoteBar;
    QList<OptionPopup*> list_popup;


    // 공통 관련 페이지 ----------------------
    PlayFullScreenRelation *playFullScreenRelation;     ///< 재생화면 (연관 컨텐츠 있는)

    QString getUrlResponse(QString tmp_str);//c220818
    void requestFavoriteForRadio(const bool &p_flagIsFavor, const QJsonObject &p_jsonData);
    //void requestFavoriteForMyPlayList(const int &p_playListNo, const bool &p_flagAdd);
    void requestFavoriteForMyPlayList(const int &p_playListNo);
    void setResultOfFavoriteForRadio(const bool &p_flagIsFavor, const QJsonObject &p_jsonObject);
    void requestFavoriteAdd(QJsonObject p_jsonObject, DataPopup *p_dataPopup, QString p_albumImg);

    /**
     * @brief AbstractMainContent::movePageOnly [virtual] 페이지 이동 (기존 list_pageCodeOrdering 있는 것 중에서 이동)
     * @details back/next 버튼으로 이동하는 경우 순수 페이지 이동만 서술되어야함. 즉 어떤 StackWidget을 보일지를 서술되어야한다.
     * (goTOMoveNewOrderPage함수 내에서 호출된다)
     * @param p_pageCode
     * @param p_data
     */
    virtual void movePageOnly(QString p_pageCode, const QJsonObject &p_data);

    // 로즈튜브 배경 위젯 추가하면서.. TopNaviBar의 부모가 메인윈도우가 아닌 빈위젯으로 설정하였기 때문에 리사이즈 필요함
    void showEvent(QShowEvent *event) override {

        Q_UNUSED(event);

        // resizeEvent만 해도 되는데.. resize호출 시점이 최초 show된 이후 호출안되므로... showEvent override 받았음
        this->topNaviBar->setFixedWidth(this->widget_roseTubeBG->size().width());
    }

    void resizeEvent(QResizeEvent *event) override {

        Q_UNUSED(event);

        // 리사이즈 될때마다 사이즈 재조정해야함
        this->topNaviBar->setFixedWidth(this->widget_roseTubeBG->size().width());

        if(this->flagSetRoseTubeBG){
            // 배경Height가 변동
            //QPixmap tmp_pixmap = this->pixmap_roseTubeBG->scaled(this->widget_roseTubeBG->size().width(), 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            //this->lb_roseTubeBG->setPixmap(tmp_pixmap);
            this->lb_roseTubeBG->resize(this->widget_roseTubeBG->size());
        }

        //lb_BG->hide();
        //lb_BGOpacity->hide();
    }

    virtual void showPlayScreenBG();
    virtual void hidePlayScreenBG();

private:
    void setInit();
    void setUIControlBasic();
    void setUIPlayScreenBG();

    QString redirectUrl_before = "";    //j220903 twitter
};

#endif // ABSTRACTMAINCONTENT_H
