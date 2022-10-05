#ifndef ROSERADIO_EXPLORE_H
#define ROSERADIO_EXPLORE_H

#include "roseRadio/ItemExplore.h"

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "common/linker.h"

#include "login/dialoglogin.h"

#include <QCoreApplication>


namespace roseRadio {

    /**
     * @brief "RoseRadioHome > "탐색" 화면
     */
    class RoseRadioExplore : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:

        explicit RoseRadioExplore(QWidget *parent = nullptr);
        ~RoseRadioExplore();

        void setActivePage() override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_hide_msg();

        void slot_showLoginPage();
        void slot_getMyInfo_loginAfter();
        void slot_change_device_state(const QString&);

        void slot_applyResult_Genre(const QJsonArray&, const int, const bool);

        void slot_clickedItemGenre(const int section, const int index, const roseRadio::ItemExplore::ClickMode_radio clickMode);

    private :
        // 초기 UI화면 구성
        void request_more_genreData();
        void request_more_genreDraw();

        void setUIControl_appendWidget_Genre();

        QWidget* setUIControl_LoginBefore();

    private:
        Linker *linker;

        Dialog::DialogLogin *dlg_rose_login;

        QWidget *widget_main_contents;
        QWidget *widget_login_contents;
        QStackedWidget *stackedWidget_Contents;

        FlowLayout *flow_explore_contents;
        QWidget *widget_explore_contents;

        roseRadio::ItemExplore *explore_genre[9999];

        QJsonArray jsonArr_Genre;

        int next_offset = 0;
        int genre_total_cnt = 0;
        int genre_draw_cnt = 0;

        bool flagReqMore_Genre = false;
        bool flag_lastPage_Genre = false;

        bool flag_flow_draw = false;
        bool flag_genre_draw = false;
    };
};
#endif // ROSERADIO_EXPLORE_H
