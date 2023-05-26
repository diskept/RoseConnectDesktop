#ifndef MUSICALBUMALL_H
#define MUSICALBUMALL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ItemAlbum_rosehome.h"

#include <QCoreApplication>


namespace music {

    class AlbumListAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit AlbumListAll(QWidget *parent = nullptr);
        ~AlbumListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

        void resizeEvent(QResizeEvent *event) override;

    protected slots:
        void slot_applyResult_albums(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_albumlistCheck(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);

        void slot_btnClicked_Delete();

        //void slot_filterClicked();
        //void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:
        void setUIControl_albums();

        void request_more_albumData();
        void request_more_albumDraw();

        void initAll();

    private:
        Linker *linker;

        // 관리 필요한 Layout UI
        QWidget *widget_mainTitle;

        QLabel *label_mainTitle;
        QPushButton *btn_mainTitle;
        QLabel *label_delete_icon;
        QLabel *label_delete;

        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        roseHome::ItemAlbum_rosehome *listAlbum_all[999999];

        QList<roseHome::AlbumItemData> *list_album;

        // Filtering
        //QList<bugs::FilterOptionData> list_filterOpt_sort;
        //QVariant selected_filterCode;
        //FilterWidget *filterWidget;
        //bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;

        QString curr_api_subPath;
        QString curr_api_mainTitle;

        int album_widget_width = 0;
        int album_widget_margin = 0;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };





    class ArtistAlbumListAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit ArtistAlbumListAll(QWidget *parent = nullptr);
        ~ArtistAlbumListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;

        void slot_applyResult_albums(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        //void slot_filterClicked();
        //void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:
        void setUIControl_albums();
        void setDataAlbumFromDB();

        void request_more_albumDraw();

        void initAll();

        QJsonArray getDataForPlayMusic(const int);

    private:
        Linker *linker;

        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        roseHome::ItemAlbum_rosehome *listAlbum_all[999999];

        QList<roseHome::AlbumItemData> *list_artistAlbums;

        // Filtering
        //QList<bugs::FilterOptionData> list_filterOpt_sort;
        //QVariant selected_filterCode;
        //FilterWidget *filterWidget;
        //bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;

        QString curr_api_mainTitle;
        QString artist = "";

        QJsonArray jsonArr_Album;

        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };





    /**
     * @brief History의 앨범 전체보기 화면 클래스
     */
    class AlbumHistoryAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit AlbumHistoryAll(QWidget *parent = nullptr);
        ~AlbumHistoryAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

        void resizeEvent(QResizeEvent *event) override;

    private slots:
        void slot_applyResult_historyAlbum(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool);

    private:
        void setUIControl_albums();

        void request_more_albumData();
        void request_more_albumDraw();

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        roseHome::ItemAlbum_rosehome *listAlbum_all[999999];

        QList<roseHome::AlbumItemData> *list_album;

        roseHome::PageInfo_AlbumAllView data_pageInfo;

        int album_widget_width = 0;
        int album_widget_margin = 0;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };





    class SearchAlbumListAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit SearchAlbumListAll(QWidget *parent = nullptr);
        ~SearchAlbumListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;

        void slot_applyResult_albums(const QList<roseHome::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        //void slot_filterClicked();
        //void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:
        void setUIControl_albums();

        void request_more_albumDraw();

        void initAll();

        QJsonArray getDataForPlayMusic(const int);

    private:
        Linker *linker;

        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;    ///< album's items

        // Data
        roseHome::ItemAlbum_rosehome *listAlbum_all[999999];

        QList<roseHome::AlbumItemData> *list_album;

        // Filtering
        //QList<bugs::FilterOptionData> list_filterOpt_sort;
        //QVariant selected_filterCode;
        //FilterWidget *filterWidget;
        //bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;

        QString curr_api_mainTitle;

        QJsonArray jsonArr_Album;

        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };
};

#endif // MUSICALBUMALL_H
