#ifndef QOBUZSEARCHVIDELALL_H
#define QOBUZSEARCHVIDELALL_H

#include "AbstractQobuzSubWidget.h"
#include "common/common_struct.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief 검색결과 - Video 전체 목록 클래스
     */
    class QobuzSearchVidelAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzSearchVidelAll(QWidget *parent = nullptr);
        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_videos(const QList<qobuz::VideoItemData>& list_data, const bool flag_lastPage) override;
        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:

        common::SearchData data_search;

        bool flagReqMore_video = false;
        bool flag_lastPage_video = false;
        void request_more_videoData();

        QLabel *label_mainTitle;
        FlowLayout *flowLayout_video;
        QList<qobuz::VideoItemData> *list_video;


    };
}


#endif // QOBUZSEARCHVIDELALL_H
