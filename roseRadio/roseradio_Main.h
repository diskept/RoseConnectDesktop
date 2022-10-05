#ifndef ROSERADIO_MAIN_H
#define ROSERADIO_MAIN_H

#include "home/abstractmaincontent.h"

#include "roseRadio/roseradio_Home.h"
#include "roseRadio/roseradio_Explore.h"

#include "roseRadio/roseradio_ListViewAll.h"
#include "roseRadio/roseradio_GenreViewAll.h"
#include "roseRadio/roseradio_CountryViewAll.h"
#include "roseRadio/roseradio_RegionViewAll.h"
#include "roseRadio/roseradio_LanguageViewAll.h"

#include "roseRadio/roseradio_CountryDetail.h"

#include "roseRadio/roseradio_SearchMain.h"

#include <QJsonObject>
#include <QWidget>
#include "widget/myqwidget.h"//c220730


namespace roseRadio {

    /**
     * @brief roseRadio Main 클래스. (서브 페이지들을 관할하는
     * @sa AbstractMainContent 자식 클래스)
     */
    class roseRadioMain : public AbstractMainContent
    {
        Q_OBJECT

    public:
        explicit roseRadioMain(QWidget *parent = nullptr);
        ~roseRadioMain();

        void process_searchWord(const QString &p_text) override;

    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

    protected slots:
        void slot_overrideSigalSearch(bool b) override;//c220730
        void slot_dragEnterEvent_hide_show(bool show);//c220730

    private slots:
        void slot_dragEnterEvent_restore();//c220826_1
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;//cheon211015

        void slot_clickedMoveSubPage(const QJsonObject &p_jsonData);

        void slot_search(const QString&);

    private :
        Linker *linker;

        // UI 세팅
        void setUIControl();

        // 데이터 반환
        QJsonObject getJsonObject_forMainTitle();
        QJsonArray getJsonArray_forSubTitles();

        QWidget *curr_widget;//c220730
        // 페이지 이동 처리
        template<typename T_roseRadioSub>
        T_roseRadioSub* procCommon_showSubPage(const bool flagShow_topBar, T_roseRadioSub *sub_widget, const QJsonObject& jsonObj_data);

        // 메뉴 관련 페이지  ---------------------- START
        roseRadio::RoseRadioHome                *sub_home = nullptr;                                    ///< 로즈라디오 > 홈
        roseRadio::RoseRadioExplore             *sub_explore = nullptr;                                 ///< 로즈라디오 > 탐색

        // 공통
        roseRadio::RoseRadioListViewAll         *sub_ListViewAll = nullptr;                             ///< 로즈라디오 > 홈 > View All
        roseRadio::RoseRadioGenreViewAll        *sub_GenreViewAll = nullptr;                            ///< 로즈라디오 > 탐색 > Genre View All
        roseRadio::RoseRadioCountryViewAll      *sub_CountryViewAll = nullptr;                          ///< 로즈라디오 > 탐색 > Country View All
        roseRadio::RoseRadioRegionViewAll       *sub_RegionViewAll = nullptr;                           ///< 로즈라디오 > 탐색 > Region View All
        roseRadio::RoseRadioLanguageViewAll     *sub_LanguageViewAll = nullptr;                         ///< 로즈라디오 > 탐색 > Language View All

        roseRadio::RoseRadioCountryDetail       *sub_CountryDetail = nullptr;                           ///< 로즈라디오 > 탐색 > Country Detail

        // Search
        roseRadio::roseRadioSearchMain          *sub_SearchMain = nullptr;                              ///< 로즈라디오 > 검색 메인

        // 메뉴 관련 페이지  ---------------------- END
    };
};
#endif // ROSERADIO_MAIN_H
