#ifndef ABSTRACTCOMMONSUBWIDGET_H
#define ABSTRACTCOMMONSUBWIDGET_H

#include <QListWidget>
#include <QStackedWidget>
#include <QWidget>
#include "delegate/AbstractTidalTrackDelegate.h"
#include "delegate/AbstractEditListDelegate.h"
#include "widget/VerticalScrollArea.h"
#include "widget/flowlayout.h"
#include "widget/OptMorePopup.h"
#include "widget/StackedWidget_AR.h"
#include "common/linker.h"//c220730


namespace common {

    class BtnLabel : public QPushButton//c220816
    {
        Q_OBJECT
    public:
        BtnLabel(QString p_text, QString p_objName, QString p_imgPath_selected, QString p_imgPath_unSelected, QWidget *parent = nullptr);
        void setStyleSelected(bool p_flagSelected);
        void setStyleText(QString a);
        void setText(QString p_text);
        void setImgPath(QString p_imgPath_selected, QString p_imgPath_unSelected);

    private:
        QLabel *lb_image;
        QLabel *lb_text;
        //QString imgPath_selected;
        //QString imgPath_unSelected;

        QPixmap *pixmap_selected;
        QPixmap *pixmap_unSelected;

    };

    /**
     * @brief Tidal, Bugs, Qobuz에서 각 SubPage 클래스에 대한 최상위 클래스임
     * @details
     *      주로 공통 Layout (VBoxlayout, HBoxLayout, FlowLayout, ScrollArea, StackedWidget(Layout) 등) 들에 대하여,
     *      아이템 사이즈, 아이템 간의 거리, 레이아웃 설정 등 UI, UX 부분을 담당하는 멤버변수, 멤버함수 들로 구성함. @n
     *      상속받은 변수, 함수들은 Child Class에서 그대로 사용하거나, 특이한 Layout인 경우에는 overriding 해서 처리하도록 한다. @n
     *      @sa AbstractMainContent 상속받은 @sa tidal::TidalMain , @sa bugs::BugsMain @sa qobuz::QobuzMain 에서 관리하는 서브페이지들의 최상위 클래스라고 보면 됨
     */
    class AbstractCommonSubWidget : public QWidget
    {
        Q_OBJECT

    public:
        /**
         * @brief Main UI를 어떻게 구성할건지에 대한 옵션
         */
        enum MainUIType {
            VerticalScroll,
            VerticalScroll_filter,
            VerticalScroll_rosefilter,
            VerticalScroll_viewAll,
            VerticalScroll_rosetube,
            VerticalScroll_roseradio,
            NoneScroll,
            NoneScroll_filter
        };
        Q_ENUM(MainUIType);

        enum DevCheckMsg{
            WaitRes_fromCITECH,             ///< CITECH로부터 응답을 기다리고 있음
            NoExist_API,                    ///< API가 없음
            NeededCheckAPI,                 ///< API 체크 필요함
            API_doesNot_Work,               ///< API가 동작을 하지 않음
            NeedDev,                        ///< 개발 해야됨
            Unknown                         ///< Dummy.
        };


        explicit AbstractCommonSubWidget(MainUIType p_mainUIType, QWidget *parent = nullptr);
        ~AbstractCommonSubWidget();

        virtual void setActivePage();  ///< 페이지 초기활성화 관련. (TidalMain에서 페이지 활성시 마다 호출한다.) 필요한 경우 overriding 할 것.
        void scrollTop();              ///< VerticalScroll 모드인 경우에만 유효

        virtual void setJsonObject_forData(const QJsonObject &jsonObj){ Q_UNUSED(jsonObj); };              /** 페이지 활성화 시에, 데이터 넘겨받기 */

        void setConnectSiganl_toMovePage(const QObject *receiver, const char *member);
        void setConnectSiganl_toShouldGoBackward(const QObject *receiver, const char *member);

        void wheelEvent(QWheelEvent*) override;

    signals:
        void signal_logState_change();
        void signal_clickedMovePage(const QJsonObject &p_jsonObject);           ///< 페이지 이동이 필요해~! 라는 시그널. TidalMain의 slot과 연결되어 사용가능. @see setConnectSiganl_toMovePage
        void signal_shouldGoBackward(const QJsonObject&);
        void signal_dropEvent_hide_show(bool);//c220728



    protected slots:


        // 별도로 http 요청이 필요한 경우 slot. 왠만하면 사용하지 않는 것으로.
        virtual void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj);

        // 서브 타이틀의 우측에 위치한 버튼에 대한 처리 slot. 필요한 경우 재정의
        virtual void slot_clickBtn_subTitle_side(const int btnId);

        virtual void slot_clickedMusic();//cheon211008-serach
        virtual void slot_clickedRoseTube();//cheon211008-serach
        virtual void slot_clickedRoseRadio();//cheon211008-serach
        virtual void slot_clickedTidal();//cheon211008-serach
        virtual void slot_clickedBugs();//cheon211008-serach
        virtual void slot_clickedQobuz();//cheon211008-serach

        void slot_singleShot_clickMusic();
        void slot_singleShot_clickRoseTube();
        void slot_singleShot_clickTidal();
        void slot_singleShot_clickQobuz();
        void slot_singleShot_clickRoseRadio();
        void slot_singleShot_clickBugs();
        // OptMorePopup 관련
        virtual void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int);

        virtual void slot_editDelegate_thumbnailDownloaded_fotListWidget(){ }

        void slot_title_link_copy();//c220816
        void slot_twiterShare();//c220816
        void slot_faceBookShare();//c220816

        void slot_applyResult_getShareLink(const QString &link);//c220903_2

    private:

        MainUIType curr_mainUIType;             ///< Main UI를 어떻게 구성할지에 대한.

        // 초기 UI화면 구성
        void setUIControl_basic_noneScroll();
        void setUIControl_basic_verticalScroll();
        void setUIControl_filter_verticalScroll();
        void setUIControl_rosefilter_verticalScroll();


    protected:

        // Edit delegate 관련
        void createList_itemEditDelegate_applyingWithData_showImage(const QList<editDelegate::ItemEditData>& list_data, QListWidget *p_listWidget, const int start_index, bool flagAdjustHeight);
        void applyImageData_itemEditDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<editDelegate::ItemEditData>* p_list_item);


        // about UI
        void addUIControl_mainTitle(const QString mainTitle);
        QLabel* get_addUIControl_mainTitle(const QString mainTitle);
        QLabel* get_addUIControl_mainTitle(const QString mainTitle, QPushButton *a, QPushButton *b, QPushButton *c, QPushButton *d, QPushButton *e, QPushButton *f);//cheon211008-search
        void addUIControl_dividedLine_onMain();

        VerticalScrollArea* get_UIControl_VerticalScrollArea();

        QHBoxLayout* get_addUIControl_hBoxLayout_forPlaylist();
        QHBoxLayout* get_addUIControl_hBoxLayout_forAlbum();
        QHBoxLayout* get_addUIControl_hBoxLayout_forMusic();
        QVBoxLayout* get_addUIControl_vBoxLayout_forTrack();
        QHBoxLayout* get_addUIControl_hBoxLayout_forArtist();
        QHBoxLayout* get_addUIControl_hBoxLayout_forVideo();
        QHBoxLayout* get_addUIControl_hBoxLayout_forPD_Album();
        FlowLayout* get_addUIControl_flowLayout(int hSpace, int vSpace);

        QListWidget* get_addUIControl_ListWidget(tidal::AbstractTidalTrackDelegate *delegate);
        QListWidget* get_addUIControl_ListWidget_forEdit(AbstractEditListDelegate *delegate);


        FlowLayout* get_UIControl_flowLayout(int hSpace, int vSpace);
        QListWidget* get_UIControl_ListWidget(tidal::AbstractTidalTrackDelegate *delegate);

        FlowLayout* get_addUIControl_flowLayout___inStackedWidget(int hSpace, int vSpace, StackedWidget_AR *parent_stackedWidgetAR);
        FlowLayout* get_addUIControl_flowLayout___inStackedWidget(int hSpace, int vSpace, QStackedWidget *parent_stackedWidget);

        QListWidget* get_addUIControl_ListWidget___inStackedWidget(tidal::AbstractTidalTrackDelegate *delegate, StackedWidget_AR *parent_stackedWidgetAR);
        QListWidget* get_addUIControl_ListWidget___inStackedWidget(tidal::AbstractTidalTrackDelegate *delegate, QStackedWidget *parent_stackedWidget);

        void addUIControl_subTitle_common(const QString subTitle);//cheon211015
        void addUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const char *slot_btnClick);
        void addUIControl_subTitle_withSideBtn_common(const QString subTitle, const QString btnText, const int btnId);
        void addUIControl_subTitle_withSideBtn_common_cnt(const QString subTitle, const QString btnText, const int btnId, QLabel *a);
        void addUIControl_needDevCheckMsg(QLayout* layout, const DevCheckMsg p_devCheckMsg);
        void addSpacing_forNewPart();

        // about styling
        QString getStyle_pushButton_optionChoose();


        // wheel event 관련
        virtual void proc_wheelEvent_to_refresh(){ }
        virtual void proc_wheelEvent_to_getMoreData(){ }


        void showToastMsg(const QString message);

        // about OptMorePopup
        void makeObj_optMorePopup(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int index=0, const int section=0, const bool flagForceHide_favorite=false);

        // about Item Widgets
        void hide_itemWidget_fromLayout(QLayout *p_layout, int index, int section);
        void remove_itemWidget_fromLayout(QLayout *p_layout, int index, int section);




        // about HTTP (specific only page)
        virtual void requestHttp(){ };          ///< Http 요청이 필요한 경우, 요런식으로 사용할 것
        virtual void setResult(const QJsonObject &p_jsonObj){ Q_UNUSED(p_jsonObj); }        ///< @see slot_responseHttp 에서 JsonObj 가공 및 이후 처리를 위한 함수들은 요런식으로 사용할 것

        QString pre_btn;
        // data for UI
        bool flagNeedReload;                ///< 페이지를 리로드해야할지에 대한 여부
        QVBoxLayout *box_mainTitle;         ///< 메인 타이틀 들어가는 Layout
        QVBoxLayout *box_searchfilter;      //cheon211008      ///< 메인 타이틀 들어가는 Layout
        QVBoxLayout *box_contents;          ///< 여기 레이아웃에다가 차곡차곡 Widget을 넣으면 됨
        QHBoxLayout *box_filter, *hl_filter;//cheon211008

        VerticalScrollArea *scrollArea_main;        ///< MainUIType = VerticalScroll 일때의 scroll

        QDialog *dialog_box_share;//c220816
        QString shareLink = "";
        void setUIShare();//c220816

        Linker *linker;
        //QDialog *abs_ani_dialog_wait;
        //QMovie *abs_ani_mov;

        QPixmap getDefaultImage_forTrack();
        int checkValid_index(int length_ofData, int index);

        void ContentLoadingwaitingMsgShow(QString msg);//cheontidal
        void ContentLoadingwaitingMsgHide();//cheontidal



    };
};

#endif // ABSTRACTCOMMONSUBWIDGET_H
