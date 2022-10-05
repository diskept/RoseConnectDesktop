#ifndef QOBUZVIDEOLISTALL_H
#define QOBUZVIDEOLISTALL_H

#include "AbstractQobuzSubWidget.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz의 Video 전체보기 화면 클래스
     */
    class QobuzVideoListAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzVideoListAll(QWidget *parent = nullptr);
        ~QobuzVideoListAll();
        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_applyResult_videos(const QList<qobuz::VideoItemData>& list_data, const bool flag_lastPage) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:

        void setUIControl_video();

        bool flagReqMore_video = false;
        bool flag_lastPage_video = false;
        void request_more_videoData();

        QLabel *label_mainTitle;
        FlowLayout *flowLayout_video;
        QList<qobuz::VideoItemData> *list_video;

        QString api_subPath;            ///< 타이달 API 요청을 위한 subPath


    };
}

#endif // QOBUZVIDEOLISTALL_H
