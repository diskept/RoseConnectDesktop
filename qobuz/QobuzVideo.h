#ifndef QOBUZVIDEO_H
#define QOBUZVIDEO_H

#include "AbstractQobuzSubWidget.h"


#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz > Video 화면 클래스
     */
    class QobuzVideo : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzVideo(QWidget *parent = nullptr);
        ~QobuzVideo();
        void setActivePage() override;

    protected slots:
        void slot_clickBtn_subTitle_side(const int) override;

        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;
        void slot_applyResult_videos(const QList<qobuz::VideoItemData>& list_data, const bool flag_lastPage) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    private slots:
//        void slot_applyResult_videos___new(const QList<qobuz::VideoItemData>& list_data);
//        void slot_applyResult_videos___top(const QList<qobuz::VideoItemData>& list_data);

    private:
        void setUIControl_basic();

        QHBoxLayout *hBox_video_new;
        QHBoxLayout *hBox_video_top;

        bool flagInitDraw;
        QList<qobuz::VideoItemData> *list_video_new;
        QList<qobuz::VideoItemData> *list_video_top;

    };
}

#endif // QOBUZVIDEO_H
