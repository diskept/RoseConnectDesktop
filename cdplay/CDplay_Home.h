#ifndef CDPLAY_HOME_H
#define CDPLAY_HOME_H

#include "cdplay/AbstractCDPlaySubWidget.h"
#include "cdplay/CDplay_struct.h"

#include "common/linker.h"

#include "widget/AbstractAlbumTrackDetailInfo_RHV.h"


namespace CDplay {

    /**
     * @brief "RoseRadioHome > "홈" 화면
     */
    class CDplayHome : public CDplay::AbstractCDPlaySubWidget
    {
        Q_OBJECT
    public:

        explicit CDplayHome(QWidget *parent = nullptr);
        ~CDplayHome();

        void setActivePage() override;

        void show_topBtnControl();
        void hide_topBtnControl();

    protected slots:
        void slot_responseHttp(const int&, const QJsonObject&) override;

        void slot_changedSubTabUI(const QJsonObject &p_data) override;

        // albumImageDetailInfo의 signal 처리를 위해
        void slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode) override;
        void slot_clickedItemTrack_inList(const int idx, const AlbumTrackDetailInfo_RHV::ClickMode clickMode) override;

    private slots:
        void slot_hide_msg();

        void changedOnlyTabUI_notSendSignal(QString p_step);

        // about TopMenuBar Button
        void slot_clicked_topmenuItem();

    private :
        // 초기 UI화면 구성
        void setUIControl_topButton(QWidget *parent);
        void setUIControl_requestCD();

    private:
        Linker *linker;

        QWidget *widget_topContrl;

        QWidget *widget_cd_contents;
        QVBoxLayout *vbox_cd_contents;

        QVBoxLayout *vbox_trackList;
        TopMenuBar *menubar;

        QVBoxLayout *vl_tracks;
        QWidget *widget_tab_tracks;

        QStackedWidget *stackedwidget;

        AbstractImageDetailContents_RHV *album_detail_info;
        AlbumTrackDetailInfo_RHV *album_track_info[50];

        CDplay::CDItemData data_cd;
        QJsonArray jsonArray_to_play;

        bool flag_draw = false;
    };
};


#endif // CDPLAY_HOME_H

