#ifndef ROSERADIO_HOME_H
#define ROSERADIO_HOME_H

#include "roseRadio/ItemRectangle.h"
#include "roseRadio/ItemSquare.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "common/linker.h"

#include "login/dialoglogin.h"
#include "widget/myqwidget.h"//c220727


namespace roseRadio {

    /**
     * @brief "RoseRadioHome > "홈" 화면
     */
    class RoseRadioHome : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:

        explicit RoseRadioHome(QWidget *parent = nullptr);
        ~RoseRadioHome();

        void setActivePage() override;

    protected slots:

        void slot_responseHttp(const int&, const QJsonObject&) override;

    private slots:
        void slot_time_out();
        void slot_hide_msg();

        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
        void slot_change_device_state(const QString&);

        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_Featured(const QJsonArray&, const int, const bool);
        void slot_applyResult_Recently(const QJsonArray&, const int, const bool);
        void slot_applyResult_Favorite(const QJsonArray&, const int, const bool);
        void slot_applyResult_Popular(const QJsonArray&, const int, const bool);
        void slot_applyResult_Local(const QJsonArray&, const int, const bool);

        void slot_applyResult_Favorite_state(const QJsonObject&);
        void slot_applyResult_Favorite_reload(const QJsonArray&, const int, const bool);

        void slot_clickedItemRoseRadio(const int, const int, const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio);

    private :
        // 초기 UI화면 구성
        void setUIControl_requestRose();

        void setUIControl_appendWidget_rose();

        QWidget* setUIControl_LoginBefore();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);

    private:
        Linker *linker;

        Dialog::DialogLogin *dlg_rose_login;

        QWidget *widget_main_contents;
        QWidget *widget_login_contents;
        QStackedWidget *stackedWidget_Contents;

        QVBoxLayout *box_rose_contents;
        QWidget *widget_rose_contents;

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        roseRadio::ItemRectangle *home_featured[15];
        roseRadio::ItemSquare *home_recently[15];
        roseRadio::ItemSquare *home_favorite[15];
        roseRadio::ItemSquare *home_popular[15];
        roseRadio::ItemSquare *home_local[15];

        QVBoxLayout *vBox_featured;
        QVBoxLayout *vBox_recently;
        QVBoxLayout *vBox_favorite;
        QVBoxLayout *vBox_popular;
        QVBoxLayout *vBox_local;

        QHBoxLayout *hBox_featured;
        QHBoxLayout *hBox_recently;
        QHBoxLayout *hBox_favorite;
        QHBoxLayout *hBox_popular;
        QHBoxLayout *hBox_local;


        QWidget *widget_featured;
        QWidget *widget_recently;
        QWidget *widget_favorite;
        QWidget *widget_popular;
        QWidget *widget_local;

        QJsonArray jsonArr_featured;
        QJsonArray jsonArr_recently;
        QJsonArray jsonArr_favorite;
        QJsonArray jsonArr_popular;
        QJsonArray jsonArr_local;

        bool flag_featured[2] = {false, false};
        bool flag_recently[2] = {false, false};
        bool flag_favorite[2] = {false, false};
        bool flag_popular[2] = {false, false};
        bool flag_local[2] = {false, false};

        int fav_section = 0;
        int fav_index = 0;
        int fav_id = 0;
        bool fav_flag = false;
    };
};


#endif // ROSERADIO_HOME_H
