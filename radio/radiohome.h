#ifndef RADIOHOME_H
#define RADIOHOME_H

#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "common/linker.h"

#include "data/dataradio.h"

#include "widget/flowlayout.h"

#include <QWidget>


namespace radio {

    /**
     * @brief 라디오 > 홈(즐겨찾기) @n
     *      사용자가 즐겨찾기한 라디오 채널 목록으로 구성
     */
    class RadioHome : public roseHome::AbstractRoseHomeSubWidget
    {
        Q_OBJECT
    public:
        explicit RadioHome(QWidget *parent = nullptr);
        ~RadioHome();

        void requestData();
        void show_topBtnControl();
        void hide_topBtnControl();

    protected slots:
        void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject) override;

    private slots:
        void slot_clickedHoverItem(const QString &p_code);
        void slot_changedFavorChannel(const bool &p_flagChangedInFavor);
        void slot_clickedRefresh();
        void slot_clickedAddChannel();

    private:
        void setInit();
        void setUIControl();
        void clearChannelList();
        void setTopUIControl(QWidget *parent);
        void setResultOfChannelList(const QJsonObject &p_jsonObject);
        void appendRadioChannel(const QJsonObject &p_jsonObject);
        void resizeEvent(QResizeEvent *event) override;

    private:
        Linker *linker;

        QWidget *widget_topContrl;

        // 관리 필요한 Layout UI
        FlowLayout *flowLayout_radio;

        QList<DataRadio*> dataList;

        int radio_widget_width = 0;
        int radio_widget_margin = 0;
    };
};

#endif // RADIOHOME_H
