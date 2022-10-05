#ifndef TIDALMYARTISTALL_H
#define TIDALMYARTISTALL_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemArtist.h"

#include "bugs/BugsChooseFilterOpt.h"
#include "widget/FilterWidget.h"

#include <QCoreApplication>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal의 My Artist 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class TidalMyArtistAll : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalMyArtistAll(QWidget *parent = nullptr);
        ~TidalMyArtistAll();

        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;
        void slot_applyResult_myFavoriteArtists(const QList<tidal::ArtistItemData>& list_data, const bool) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:
        // 초기 UI화면 구성
        void setUIControl_artists();
        void setUiControl_filter();

        void request_more_artistData();
        void request_more_artistDraw();

        void initAll();

    private:
        FlowLayout *flowLayout_artists;

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        // Data
        tidal::ItemArtist *listArtist_all[99999];

        QList<tidal::ArtistItemData> *list_artist;

        int artist_widget_cnt = 0;

        int next_offset = 0;
        int artist_total_cnt = 0;
        int artist_draw_cnt = 0;

        bool flagReqMore_artist = false;
        bool flag_lastPage_artist = false;

        bool flag_flow_draw = false;
        bool flag_artist_draw = false;
    };

};
#endif // TIDALMYARTISTALL_H
