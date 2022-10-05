#ifndef CDPLAY_MAIN_H
#define CDPLAY_MAIN_H

#include "home/abstractmaincontent.h"

#include "cdplay/CDplay_Home.h"

#include "widget/myqwidget.h"//c220730

#include <QJsonObject>
#include <QWidget>


namespace CDplay {

    /**
     * @brief CD Play Main 클래스. (서브 페이지들을 관할하는
     * @sa AbstractMainContent 자식 클래스)
     */
    class CDplayMain : public AbstractMainContent
    {
        Q_OBJECT

    public:
        explicit CDplayMain(QWidget *parent = nullptr);
        ~CDplayMain();

        void process_searchWord(const QString &p_text) override;

    protected slots:
        void slot_overrideSigalSearch(bool b) override;//c220730
        void slot_dragEnterEvent_hide_show(bool show);//c220730

    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

    private slots:
        void slot_dragEnterEvent_restore();//c220826_1
        void slot_clickedMoveSubPage(const QJsonObject &p_jsonData);

        void slot_search(const QString&);

    private :
        Linker *linker;

        // UI 세팅
        void setUIControl();

        QWidget *curr_widget;//c220730

        // 데이터 반환
        QJsonObject getJsonObject_forMainTitle();
        QJsonArray getJsonArray_forSubTitles();

        // 페이지 이동 처리
        template<typename T_CDplaySub>
        T_CDplaySub* procCommon_showSubPage(const bool flagShow_topBar, T_CDplaySub *sub_widget, const QJsonObject& jsonObj_data);

        // 메뉴 관련 페이지  ---------------------- START
        CDplay::CDplayHome                *sub_home = nullptr;                                    ///< 濡쒖쫰쇰뵒> 

        // 메뉴 관련 페이지  ---------------------- END
    };
};
#endif // CDPLAY_MAIN_H
