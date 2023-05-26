#ifndef MUSICARTISTALL_H
#define MUSICARTISTALL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ItemArtist_rosehome.h"

#include <QCoreApplication>


namespace music {

    class ArtistListAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit ArtistListAll(QWidget *parent = nullptr);
        ~ArtistListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_artists(const QList<roseHome::ArtistItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;

    protected:
        //void proc_wheelEvent_to_getMoreData() override;

        void resizeEvent(QResizeEvent *event) override;

    private slots:
        //void slot_filterClicked();
        //void slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName);

    private:
        void setUIControl_artists();
        void setUIControl_filter();

        void initAll();

    private:
        Linker *linker;

        QLabel *label_mainTitle;
        FlowLayout *flowLayout_artists;    ///< artist's items

        // Data
        roseHome::ItemArtist_rosehome *listArtist_all[1000];

        QList<roseHome::ArtistItemData> *list_artist;

        // Filtering
        //QList<bugs::FilterOptionData> list_filterOpt_sort;
        //QVariant selected_filterCode;
        //FilterWidget *filterWidget;
        //bugs::BugsChooseFilterOpt *chooseFilterOpt = nullptr;

        QString curr_api_mainTitle;

        QJsonArray jsonArr_Artist;

        int artist_widget_width = 0;
        int artist_widget_margin = 0;

        bool flag_flow_draw = false;
    };
};

#endif // MUSICARTISTALL_H
