#ifndef ROSERADIO_LANGUAGEVIEWALL_H
#define ROSERADIO_LANGUAGEVIEWALL_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include <QCoreApplication>


namespace roseRadio {


    class RoseRadioLanguageViewAll : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT

    public:
        explicit RoseRadioLanguageViewAll(QWidget * parent = nullptr);
        ~RoseRadioLanguageViewAll();

        void setJsonObject_forData(const QJsonObject&) override;
        void setActivePage() override;

    protected:
        void proc_wheelEvent_to_getMoreData() override;

    private slots:
        void slot_applyResult_Language(const QJsonArray&, const int, const bool);

        void slot_clickedItemLanguage();

    private:
        void setUIControl_Language();

        void request_more_LanguageData();
        void request_more_LanguageDraw();

        QWidget* setLanguage_pushButton(const int index, const QString name);

    private:
        // 관리 필요한 Layout UI
        QLabel *label_mainTitle;
        QWidget *widget_language;
        FlowLayout *flowLayout_language;

        QString apiPath = "";
        QString title = "";
        QString listType = "";
        int languageId = 0;

        QList<roseRadio::RoseRadioLanguageInfo> *list_languageInfo;
        QJsonArray jsonArr_language;

        int next_offset = 0;
        int language_total_cnt = 0;
        int language_draw_cnt = 0;

        bool flagReqMore_language = false;
        bool flag_lastPage_language = false;

        bool flag_flow_draw = false;
        bool flag_language_draw = false;
    };
};
#endif // ROSERADIO_LANGUAGEVIEWALL_H
