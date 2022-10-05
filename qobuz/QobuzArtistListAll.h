#ifndef QOBUZARTISTLISTALL_H
#define QOBUZARTISTLISTALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemArtist_forQobuz.h"

#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief Qobuz의 Airtist 전체보기 화면 클래스
     */
    class QobuzArtistListAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzArtistListAll(QWidget *parent = nullptr);
        ~QobuzArtistListAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

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
        QLabel *label_mainTitle;
        FlowLayout *flowLayout_artists;

        // Data
        qobuz::ItemArtist_qobuz *listArtist_all[99999];

        QList<qobuz::ArtistItemData> *list_artist;

        QString api_subPath;

        int artist_id;

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

#endif // QOBUZARTISTLISTALL_H
