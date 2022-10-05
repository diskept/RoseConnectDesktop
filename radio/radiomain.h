#ifndef RADIOMAIN_H
#define RADIOMAIN_H

#include "home/abstractmaincontent.h"

#include "radio/radiohome.h"
#include "radio/radiocountry.h"
#include "radio/radiouser.h"
#include "radio/radioedit.h"
#include "radio/radiosearch.h"

#include <QWidget>
#include <QSettings>


namespace radio {

    /**
     * @brief 라디오 메인메뉴의 총괄 @n
     *      라디오 대항목 관련 모든 QWidget 관리
     */
    class RadioMain : public AbstractMainContent
    {
        Q_OBJECT
    public:
        explicit RadioMain(QWidget *parent = nullptr);
        ~RadioMain();

    signals:

    private slots:
        void slot_reqNewPage(const QJsonObject &p_jsonData);
        void slot_search(const QString &p_text);
        void slot_showRadioCountryComboBox(QString p_subMenuCode);

        void slot_dragEnterEvent_restore();//c220826_1

    protected:
        void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

    protected slots:
        void slot_overrideSigalSearch(bool b) override;//c220730
        void slot_dragEnterEvent_hide_show(bool show);//c220730

    private:
        void setInit();
        void setUIControl();

    private :
        // ★ Linker 변수 이미 있으니 추가하지마세요.. protected에 있어요

        RadioHome *radioHome=nullptr;
        RadioCountry *radioCountry=nullptr;
        RadioUser *radioUser=nullptr;
        RadioEdit *radioEdit=nullptr;
        RadioSearch *radioSearch=nullptr;

        QSettings *settings;

        QWidget *curr_widget;//c220730
    };
};

#endif // RADIOMAIN_H
