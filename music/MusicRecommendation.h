#ifndef MUSICRECOMMENDATION_H
#define MUSICRECOMMENDATION_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ItemAlbum_rosehome.h"

#include <QCheckBox>
#include <QSettings>
#include <QCoreApplication>


namespace music {

    class MusicRecommend : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit MusicRecommend(QWidget *parent = nullptr);
        ~MusicRecommend();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        //void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;

        void slot_applyResult_albums(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

        void resizeEvent(QResizeEvent *event) override;

    private slots:
        // about clicks
        void slot_clickBtn_Filter();
        void slot_clickBtn_Refresh();
        void slot_clickBtn_Filter_close();
        void slot_clickBtn_Filter_choose();
        void slot_clickBtn_Filter_apply();
        void slot_clickBtn_Filter_cancel();

    private:
        void setUIControl_button();
        void setUIControl_filter();
        void setUIControl_albums();

        void request_more_albumData();
        void request_more_albumDraw();

        void initAll();

        int getAlbumDataFromDBforTotalCount();
        QJsonArray getDataForPlayMusic(const int);

    private:
        Linker *linker;

        QSettings *settings;

        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        roseHome::ItemAlbum_rosehome *listAlbum_all[600];

        QList<roseHome::AlbumItemData> *list_album;

        // Filtering
        QLabel *lb_file_type;
        QLabel *lb_bit_depth;
        QLabel *lb_sample_rate;
        QLabel *lb_recomm_count;

        bool flag_file_type = false;
        bool flag_bit_depth = false;
        bool flag_sample_rate = false;

        bool flag_change_filter = false;

        int before_file_type = 0;
        int before_bit_depth = 0;
        int before_sample_rate = 0;

        QWidget *widget_btnFilter;

        QWidget *widget_select_filter;
        FlowLayout *flow_select_filter;

        QWidget *widget_filter;                 ///< filter
        QPushButton *btn_filter_ico;            ///< filter icon
        QPushButton *btn_refresh;               ///< refresh button
        QPushButton *btn_filter_clear;          ///< clear button

        QCheckBox *checkbox_file_type[10];
        QCheckBox *checkbox_bit_depth[10];
        QCheckBox *checkbox_sample_rate[10];

        QVariant selected_optFilter;
        QVariant selected_optIdx;

        QString curr_page;

        QString rocommCount;

        QString btnStyle_normal;
        QString btnStyle_selected;

        QStringList fileTypeList;
        QStringList bitDepthList;
        QStringList samplingList;

        QJsonArray jsonArr_Album;

        int album_widget_width = 0;
        int album_widget_margin = 0;

        int album_widget_cnt = 0;

        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };
};
#endif // MUSICRECOMMENDATION_H
