#ifndef ROSEHOMEROSETUBELISTALL_H
#define ROSEHOMEROSETUBELISTALL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "rosetube/ItemTrack_rosetube.h"

#include <QCoreApplication>


namespace roseHome {
    /**
     * @brief RoseHome의 플레이리스트 전체보기 화면 클래스
     */
    class RoseHomeRosetubeListAll : public AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RoseHomeRosetubeListAll(QWidget *parent = nullptr);
        ~RoseHomeRosetubeListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_responseHttp(const int&, const QJsonObject&) override;

        void slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

        void resizeEvent(QResizeEvent *event) override;

    private slots:
        void slot_applyResult_rosetube(const QJsonArray&, const int&, const bool);
        void slot_applyResult_rosetubelistCheck(const QJsonArray&, const int&, const bool);

        void slot_btnClicked_Delete();

        void slot_applyResult_getRating_track(const QJsonArray&);

    private:
        // 초기 UI화면 구성
        void setUIControl_rosetube();

        void request_more_rosetubeData();
        void request_more_rosetubeDraw();

        void initAll();

    private:
        // 관리 필요한 Layout UI
        QWidget *widget_mainTitle;

        QLabel *label_mainTitle;
        QPushButton *btn_mainTitle;
        QLabel *label_delete_icon;
        QLabel *label_delete;

        FlowLayout *flowLayout_rosetube;    ///< rosetube's items

        // Data
        rosetube::ItemTrack_rosetube *recently_rosetube[1100];

        QJsonArray playlist_Arr;

        PageInfo_PlaylistAllView current_pageInfo;

        int playlist_widget_width = 0;
        int playlist_widget_margin = 0;

        int playlist_widget_cnt = 0;

        int next_offset = 0;
        int playlist_total_cnt = 0;
        int playlist_draw_cnt = 0;

        // getting more data
        bool flagReqMore_playlist = false;
        bool flag_lastPage_playlist = false;

        bool flag_flow_draw = false;
        bool flag_playlist_draw = false;

        int flag_fav_idx = 0;
        int flag_fav_star = 0;
        int flag_fav_type = 0;
    };
};

#endif // ROSEHOMEROSETUBELISTALL_H
