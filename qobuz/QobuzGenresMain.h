#ifndef QOBUZGENRESMAIN_H
#define QOBUZGENRESMAIN_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "widget/flowlayout.h"

#include "qobuz/qobuz_struct.h"
#include "AbstractQobuzSubWidget.h"
#include "delegate/QobuzTrackDelegate.h"
#include "delegate/QobuzAlbumDelegate.h"


#define print_qobuz_func() qDebug() << "[QOBUZ][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace qobuz {


    /**
     * @brief Qobuz > 탐색 > Genres > 장르선택 (장르 하나 선택했을 때의 메인 화면) 클래스
     */
    class QobuzGenresMain : public AbstractQobuzSubWidget
    {
        Q_OBJECT
    public:
        explicit QobuzGenresMain(QWidget *parent = nullptr);
        ~QobuzGenresMain();

        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void setActivePage() override;                                          ///< 페이지 초기활성화 관련. (페이지 초기활성화시 호출)

    protected slots:
        void slot_clickBtn_subTitle_side(const int) override;

        void slot_clickedItemAlbum_inList(const int, const tidal::AbstractTidalTrackDelegate::ClickMode) override;
        void slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode) override;
        void slot_thumbnailDownloaded_fotListWidget_track() override;

        void slot_applyResult_classic(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_idealDiscography(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_stillTrending(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_pressAwards(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);
        void slot_applyResult_qobuzIssime(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);

        void slot_applyResult_topRelease(const QList<qobuz::AlbumItemData>&, const QJsonArray&, const bool);

        // about OptMorePopup
        void slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode, const int, const int) override;


    private:
        // 초기 UI화면 구성
        void setUIControl_univertalClassic();
        void setUIControl_idealDiscography();
        void setUIControl_stillTranding();
        void setUIControl_pressAwards();
        void setUIcontrol_qobuzIssime();
        void setUIcontrol_topRelease();

        void setUIcontrol_All_contents(const QString, const bool);

//        void resizeEvent(QResizeEvent *event) override;

        QLabel *label_mainTitle;                ///< main titel for specific genre's name

        QHBoxLayout *hBox_classic;              ///< suggested new albums part
        QHBoxLayout *hBox_idealDiscography;     ///< ideal discography part
        QHBoxLayout *hBox_stillTranding;        ///< still tranding albums part
        QHBoxLayout *hBox_pressAwards;          ///< press awards albums part
        QHBoxLayout *hBox_qobuzIssime;          ///< qobuz issime albums part

        QListWidget *listWidget_toprelease;     ///< top release albums part

        QobuzAlbumDelegate *delegate_album;


        // Data
        int genre_cnt = 0;
        QString genre_ids = "";

        QList<qobuz::AlbumItemData> *list_classic;
        QList<qobuz::AlbumItemData> *list_idealDiscography;
        QList<qobuz::AlbumItemData> *list_still_trending;
        QList<qobuz::AlbumItemData> *list_press_awards;
        QList<qobuz::AlbumItemData> *list_qobuz_issime;

        QList<qobuz::AlbumItemData> *list_top_release;

        QJsonArray jsonArr_tracks_toPlay;      ///< Track 전체를 재생하기 위함

        bool flag_ideal[2] = {false, false};
        bool flag_still[2] = {false, false};
        bool flag_press[2] = {false, false};
        bool flag_qobuz[2] = {false, false};
        bool flag_top[2] = {false, false};
    };

};

#endif // QOBUZGENRESMAIN_H
