#ifndef QOBUZMYVIDEOALL_H
#define QOBUZMYVIDEOALL_H

#include "AbstractQobuzSubWidget.h"
#include "bugs/BugsChooseFilterOpt.h"
#include "widget/FilterWidget.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz의 My Video 전체목록 화면 ("좋아요" 선택한 목록 전체)
     */
    class QobuzMyVideoAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzMyVideoAll(QWidget *parent = nullptr);
        ~QobuzMyVideoAll();
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

        void resizeEvent(QResizeEvent *event) override;         // filter Box 사이즈 조절을 위해

    protected slots:
        void slot_applyResult_myFavoriteVideos(const QList<qobuz::VideoItemData>& list_data, const bool flag_lastPage) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_filterClicked();
        void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:

        void setUIControl_video();

        void setUiControl_filter();

        void initAll();

        // Filtering
        QList<bugs::FilterOptionData> list_filterOpt_sort;           ///< 필터링 박스에 띄울 데이터. group 1개밖에 없음
        QVariant selected_filterCode;                               ///< 현재 화면에서 선택된 필터링 코드
        FilterWidget *filterWidget;
        bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;         // nullptr 초기 필수

        bool flagReqMore_video = false;
        bool flag_lastPage_video = false;
        void request_more_videoData();

        // UI
        FlowLayout *flowLayout_video;
        QList<qobuz::VideoItemData> *list_video;


    };
}

#endif // QOBUZMYVIDEOALL_H
