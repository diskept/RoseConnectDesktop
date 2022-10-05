#ifndef QOBUZSEARCHALBUMALL_H
#define QOBUZSEARCHALBUMALL_H

#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ItemAlbum_forQobuz.h"

#include "common/common_struct.h"

#include <QCoreApplication>

#define  print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {

    /**
     * @brief 검색결과 - Album 전체 목록 클래스
     */
    class QobuzSearchAlbumAll : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzSearchAlbumAll(QWidget *parent = nullptr);
        ~QobuzSearchAlbumAll();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;
        void setActivePage() override;

    protected slots:
        void slot_applyResult_albums(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode) override;

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private:
        // 초기 UI화면 구성
        void setUIControl_albums();

        void request_more_albumData();
        void request_more_albumDraw();

    private:
        common::SearchData data_search;

        QLabel *label_mainTitle;
        FlowLayout *flowLayout_albums;

        // Data
        qobuz::ItemAlbum_qobuz *listAlbum_all[999999];

        QList<qobuz::AlbumItemData> *list_album;

        int album_widget_cnt = 0;

        int next_offset = 0;
        int album_total_cnt = 0;
        int album_draw_cnt = 0;

        bool flagReqMore_album = false;
        bool flag_lastPage_album = false;

        bool flag_flow_draw = false;
        bool flag_album_draw = false;
    };

};

#endif // QOBUZSEARCHALBUMALL_H
