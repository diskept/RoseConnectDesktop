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

    private slots:
        void slot_applyResult_myPlaylist_rose(const QJsonArray&, const int&, const bool);

    private:
        // 초기 UI화면 구성
        void setUIControl_rosetube();

        void request_more_rosetubeData();
        void request_more_rosetubeDraw();

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_rosetube;    ///< rosetube's items

        // Data
        rosetube::ItemTrack_rosetube *recently_rosetube[999999];

        QJsonArray playlist_Arr;

        QString curr_api_subPath;
        QString curr_api_mainTitle;

        int playlist_widget_cnt = 0;

        int next_offset = 0;
        int playlist_total_cnt = 0;
        int playlist_draw_cnt = 0;

        // getting more data
        bool flagReqMore_playlist = false;
        bool flag_lastPage_playlist = false;

        bool flag_flow_draw = false;
        bool flag_playlist_draw = false;
    };
};

#endif // ROSEHOMEROSETUBELISTALL_H
