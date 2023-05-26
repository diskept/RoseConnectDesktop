#ifndef MUSICALBUM_H
#define MUSICALBUM_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ItemAlbum_rosehome.h"

#include <QCheckBox>
#include <QSettings>
#include <QCoreApplication>


namespace music {
    class MusicAlbum : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit MusicAlbum(QWidget *parent = nullptr);
        ~MusicAlbum();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
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
        roseHome::ItemAlbum_rosehome *listAlbum_all[999999];

        QList<roseHome::AlbumItemData> *list_album;

        // Filtering
        QLabel *lb_order_type;
        QLabel *lb_file_type;
        QLabel *lb_bit_depth;
        QLabel *lb_sample_rate;
        QLabel *lb_total_count;

        bool flag_order_type = false;
        bool flag_file_type = false;
        bool flag_bit_depth = false;
        bool flag_sample_rate = false;

        bool flag_change_filter = false;

        int before_order_type = 0;
        int before_file_type = 0;
        int before_bit_depth = 0;
        int before_sample_rate = 0;

        QWidget *widget_btnFilter;

        QWidget *widget_select_filter;
        FlowLayout *flow_select_filter;

        QWidget *widget_filter;                 ///< filter
        QPushButton *btn_filter_ico;            ///< filter icon
        QPushButton *btn_filter_clear;          ///< clear button

        QCheckBox *checkbox_order_type[10];
        QCheckBox *checkbox_file_type[10];
        QCheckBox *checkbox_bit_depth[10];
        QCheckBox *checkbox_sample_rate[10];

        QVariant selected_optFilter;
        QVariant selected_optIdx;

        QString curr_page;

        QString albumCount;

        QString btnStyle_normal;
        QString btnStyle_selected;

        QStringList orderTypeList;
        QStringList fileTypeList;
        QStringList bitDepthList;
        QStringList samplingList;

        QJsonArray jsonArr_Album;

        int album_widget_width = 0;
        int album_widget_margin = 0;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };
};





//#include "home/sectionbottomSlider.h"

//#include "common/linker.h"

//#include "widget/clickablelabel.h"//c220415
//#include "widget/customlineedit.h"//cheon210527
//#include "widget/framemusicalbum.h"
//#include "widget/flowlayout.h"

//#include <QScrollArea>
//#include <QVBoxLayout>
//#include <QWidget>
//#include <QLabel>
//#include <QPushButton>
//#include <QSettings>
//#include <QTextEdit>//cheon210717-search
//#include <QElapsedTimer>//cheon210608-test
//#include <QTimer>//cheon210608-test
//#include <QMutex>


/**
 * @brief 음악-앨범 @n
 *      플레이가능한 앨범 목록 화면
 *
 * @note
 *      @sa FrameMusicAlbum 목록으로 구성
 */
/*class MusicAlbum : public QWidget
{
    Q_OBJECT
public:
    explicit MusicAlbum(QWidget *parent = nullptr);
    ~MusicAlbum();

    void setDataFromDB();
    void setDataFromDB_Alpa(const QString &p_arg);
    QString go_page_text;//cheon210527


signals:

private slots:
    void slot_changedSliderBar(int p_value);
    void slot_sliderbarValue();
    void slot_sliderbarValue_check();
    void slot_sliderPressed();
    void slot_sliderReleased();
    void slot_filter_apply();
    void slot_filter_cancel();
    void slot_clickedBtnFilter();
    void slot_clickedBtnSer();
    //void slot_clicked_search_ch();//c220415
    void slot_clicked_search_eng_ch();//c220415
    void slot_clicked_search_han_ch();//c220415
    void slot_clearButton_setStyleSheet();
    void slot_clicked_search_eng_ch_clear();//c220415
    void slot_changedBtnFilter();
    //void slot_returnPressed(const QString p_text);//cheon210717-search
    void slot_returnPressed();//cheon210717-search
    //void slot_textChanged(QString);//cheon210717-search
    void slot_clickedChange();//cheon210717-search    

    void slot_returnPressed_str();//cheon210717-search
    //void slot_textChanged(QString);//cheon210717-search
    void slot_clickedChange_str();//cheon210717-search
    //void slot_clickedChange_alpa();//c0204

    void OnTimerCallbackFunc();//cheon210608-test

private:

    void setInit();
    void setUIControl();
    void setFIlterUIControl();
    void setFIlterUIControl_cancel();
    void setUIlistBtnSer_boxControl();
    void getAlbumDataFromDB();
    QString searchcharactorSqlWhere();
    QString getSetWhereFilter();
    void getAlbumDataFromDB_view();//c220323
    void getAlbumDataFromDB_view_only();
    //void getAlbumDataFromDB_view_tmp();
    void getAlbumDataFromDB_pre_view();//c220323
    void getAlbumDataFromDB_pre();
    void goto_getAlbumDataFromDB(int p_page);//cheon210527
    void goto_getAlbumDataFromDB_view();
    void goto_setDataFromDB(int p_page);

    int getAlbumDataFromDBforTotalCount();
    void wheelEvent(QWheelEvent *event) override;
    void run_flagcheck();//cheon210608-test

    void resizeEvent(QResizeEvent *event) override;//c230222

private:
    QWidget *line_bottom_33;
    int tmp_cal_page = 1;

    int term_i = 0;
    ClickableLabel *btn_serClear;
    QWidget *widget_total_ser;
    QWidget *widget_btnFilter;
    sectionbottomSlider *slider_playbar;
    Linker *linker;
    bool slot_sliderValue_flag = false;
    int pre_LmtWidth = 0;
    int curr_LmtWidth = 0;
    bool slot_clickedBtnFilter_filter_flag = false;
    bool pre_mouse_drag_flag = false;//c220323
    QVariantList data;//c220323
    QString strArg_1 ="";
    int CNT = 0;
    bool NreloadFlag = false;//cheon210608-test
    bool PreloadFlag = false;//cheon210608-test
    QThread *thread1;//cheon210608-test
    QTimer *workerTrigger1;//cheon210608-test
    QElapsedTimer Etimer1;//cheon210608-test
    bool flagReqMore = false;//cheon210607-test
    FlowLayout *flowLayout, *flowLayout_tmp;
    QSettings *settings;
    QWidget *widget_scrollArea;
    QScrollArea *scrollArea;
    QVBoxLayout *vl_total;
    QWidget *widget_filter;

    QString btnStyle_normal;
    QString btnStyle_selected;
    QStringList fileTypeList;
    QStringList bitDepthList;
    QStringList samplingList;
    QStringList orderList;
    QStringList starList;
    QStringList hanList, engList;
    FrameMusicAlbum *wg_album_tmp;

    QHBoxLayout *hl_filter, *hl_eng_ser, *hl_han_ser;

    QLabel *lb_han_type[14];
    QLabel *lb_eng_type[26];

    QLabel *lb_file_type;
    QLabel *lb_bit_depth;
    QLabel *lb_sample_rate;
    QLabel *lb_star_point;
    QLabel *lb_order;

    bool flag_file_type = false;
    bool flag_bit_depth = false;
    bool flag_sample_rate = false;
    bool flag_star_point = false;
    bool flag_order = false;

    bool flag_change_filter = false;

    int before_file_type = 0;
    int before_bit_depth = 0;
    int before_sample_rate = 0;
    int before_star_point = 0;
    int before_order = 0;

    QPushButton *btn_file_type[10];
    QPushButton *btn_bit_depth[10];
    QPushButton *btn_sample_rate[10];
    QPushButton *btn_star_point[10];
    QPushButton *btn_order[10];

    QVariant selected_optFilter;
    QVariant selected_optIdx;

    QLabel* createFilterLabel(const QString &p_label);
    //QPushButton* createFilterBtn(const QString &p_text, bool flagSelected=false);
    QPushButton* createFilterBtn(const QString p_filter, const int idx, const QString &p_text, bool flagSelected=false);

    //CustomLineEdit *le_lisearch;//cheon210717-search
    QLineEdit *le_lisearch, *le_lisearch_str;//cheon210717-search

    QLabel *albumtCnt, *albumNone;
    QLabel *albumPage;
    int currSliderPosition = 0, curr_p_value = 1, cnt_size = 0;
    bool blocking_flag = true;
    int LMT_CNT = 0;
    int pre_page_cnt_tmp = 0;
    bool pre_flag =false, slot_sliderbarValue2_flag =false;;

private:
    QMutex mtx;
    bool flag_page_first = false;

    int pre_lmt_cnt = 0;
    int OffsetPoint = 0, currOffsetPoint = 0;
    int OffsetPoint_Pre = 0;
    bool mouse_wheel_flag = false;
    int total_page_cnt = 0;
    int curr_page_cnt = 0;
    int next_page_cnt = 0;
    int total_db_cnt = 0;

    int scroll_pos_currunt = 0;
    int scroll_pos_before = 0;

    int currunt_album_page = 0;
    int before_album_page = 1;
    QTimer *timer;
};*/

#endif // MUSICALBUM_H
