#ifndef QOBUZSEARCHARTISTALL_H
#define QOBUZSEARCHARTISTALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemArtist_forQobuz.h"

#include "common/common_struct.h"

#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief 검색결과 - Artist 전체 목록 클래스
     */
    class QobuzSearchArtistAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzSearchArtistAll(QWidget *parent = nullptr);
        ~QobuzSearchArtistAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_artists(const QList<qobuz::ArtistItemData>&, const bool) override;
        void slot_clickedItemArtist(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        // 초기 UI화면 구성
        void setUIControl_artists();

        void request_more_artistData();
        void request_more_artistDraw();

    private:
        common::SearchData data_search;

        QLabel *label_mainTitle;
        FlowLayout *flowLayout_artists;

        // Data
        qobuz::ItemArtist_qobuz *listArtist_all[99999];

        QList<qobuz::ArtistItemData> *list_artist;

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
#endif // QOBUZSEARCHARTISTALL_H
