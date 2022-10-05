#ifndef TIDALVIDEO_H
#define TIDALVIDEO_H

#include "tidal/AbstractTidalSubWidget.h"

#include "tidal/ItemVideo.h"

#include "common/common_struct.h"


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    /**
     * @brief Tidal > Video 화면 클래스
     */
    class TidalVideo : public AbstractTidalSubWidget
    {
        Q_OBJECT
    public:
        explicit TidalVideo(QWidget *parent = nullptr);
        ~TidalVideo();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_acceptedDialogLogin() override;

        void slot_clickedItemVideo(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    private slots:
        // about 서브 타이틀의 버튼
        void slot_clickBtn_subTitle_viewAll();

        void slot_applyResult_videos___hiphop(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage);
        void slot_applyResult_videos___rnb(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage);
        void slot_applyResult_videos___country(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage);
        void slot_applyResult_videos___pop(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage);
        void slot_applyResult_videos___latin(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage);
        void slot_applyResult_videos___dance(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage);
        void slot_applyResult_videos___electro(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage);
        void slot_applyResult_videos___indie(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage);

    private:
        void setUIControl_appendWidget();

        QWidget* setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout);
        QHBoxLayout* setUIControl_hBoxLayout_forVideo(QLayout *p_layout);

    private:
        QString page = "";

        QLabel *lb_subTitle[10];
        QPushButton *btnView_all[10];

        tidal::ItemVideo *video_HipHop[15];
        tidal::ItemVideo *video_RNB[15];
        tidal::ItemVideo *video_Country[15];
        tidal::ItemVideo *video_Pop[15];
        tidal::ItemVideo *video_Latin[15];
        tidal::ItemVideo *video_Dance[15];
        tidal::ItemVideo *video_Electro[15];
        tidal::ItemVideo *video_Indie[15];

        QVBoxLayout *box_video_contents;    ///< Tidal video Body Layout
        QWidget *widget_video_contents;

        QWidget *widget_video_HipHop;
        QWidget *widget_video_RNB;
        QWidget *widget_video_Country;
        QWidget *widget_video_Pop;
        QWidget *widget_video_Latin;
        QWidget *widget_video_Dance;
        QWidget *widget_video_Electro;
        QWidget *widget_video_Indie;

        QVBoxLayout *vBox_video_HipHop;
        QVBoxLayout *vBox_video_RNB;
        QVBoxLayout *vBox_video_Country;
        QVBoxLayout *vBox_video_Pop;
        QVBoxLayout *vBox_video_Latin;
        QVBoxLayout *vBox_video_Dance;
        QVBoxLayout *vBox_video_Electro;
        QVBoxLayout *vBox_video_Indie;

        QHBoxLayout *hBox_video_HipHop;
        QHBoxLayout *hBox_video_RNB;
        QHBoxLayout *hBox_video_Country;
        QHBoxLayout *hBox_video_Pop;
        QHBoxLayout *hBox_video_Latin;
        QHBoxLayout *hBox_video_Dance;
        QHBoxLayout *hBox_video_Electro;
        QHBoxLayout *hBox_video_Indie;

        QList<tidal::VideoItemData> *list_video_HipHop;
        QList<tidal::VideoItemData> *list_video_RNB;
        QList<tidal::VideoItemData> *list_video_Country;
        QList<tidal::VideoItemData> *list_video_Pop;
        QList<tidal::VideoItemData> *list_video_Latin;
        QList<tidal::VideoItemData> *list_video_Dance;
        QList<tidal::VideoItemData> *list_video_Electro;
        QList<tidal::VideoItemData> *list_video_Indie;

        bool flag_HipHop[2] = {false, false};
        bool flag_RNB[2] = {false, false};
        bool flag_Country[2] = {false, false};
        bool flag_Pop[2] = {false, false};
        bool flag_Latin[2] = {false, false};
        bool flag_Dance[2] = {false, false};
        bool flag_Electro[2] = {false, false};
        bool flag_Indie[2] = {false, false};
    };
}

#endif // TIDALVIDEO_H
