#ifndef BUGSTHEMESCHOOSE_H
#define BUGSTHEMESCHOOSE_H

#include <QWidget>
#include "AbstractBugsSubWidget.h"
#include "widget/SegmentTabWidget.h"
#include "BugsChooseFilterOpt.h"
#include "bugs/BugsChooseFilterOpt.h"
#include "bugs_struct.h"

namespace bugs {

    /**
     * @brief 벅스 > 테마 선택 화면 클래스
     */
    class BugsThemesChoose : public AbstractBugsSubWidget
    {
        Q_OBJECT
    public:
        explicit BugsThemesChoose(QWidget *parent = nullptr);
        void setJsonObject_forData(const QJsonObject& jsonObj) override;        ///< 페이지 Show 요청 시, 데이터 전달받는 용도
        void applyLoginSuccessState() override;

    protected:
        void setUIControl_logined() override;


    private slots:
        void slot_segmentTag_clicked(int clickedIndex);
        void slot_applyResult_tags_in_theme(const QList<bugs::BugsTag>& list_tag);
        void slot_tagBtnClicked();

    private:
        SegmentTabWidget *segmentTabWidget;
        bugs::FilterOptionData data_filterOpt;          // 현재 선택한 Theme Code 정보가 들어있음
        FlowLayout *flowLayout_tags;

        void setUIControl_themesTab();

        QList<bugs::BugsTag> *list_tag;

    };

};

#endif // BUGSTHEMESCHOOSE_H
