#ifndef BUGSCHOOSEFILTEROPT_H
#define BUGSCHOOSEFILTEROPT_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include "bugs/bugs_struct.h"

namespace bugs {

    /**
     * @brief 필터 옵션 데이터 세팅용
     */
    struct FilterOptionData {
        QVariant opt_code;
        QString opt_name;
    };


    /**
     * @brief 목록 필터링 옵션 선택하는 Filter Option 박스 Widget (Tidal, Bugs 사용)
     */
    class BugsChooseFilterOpt : public QWidget
    {
        Q_OBJECT
    public:

        /**
         * @brief The FilterOptSize enum
         */
        enum class FilterOptSize{
            Normal
            , Small                     ///< 벅스에서 옵션이 너무 많아서, 최소 사이즈에서 화면 넘침
        };
        Q_ENUM(FilterOptSize);

        explicit BugsChooseFilterOpt(QWidget *parent = nullptr);
        explicit BugsChooseFilterOpt(FilterOptSize optSize, QWidget *parent = nullptr);

        void addFilterGroup(QString groupName, const QList<FilterOptionData> list_optData, QVariant selected_code);
        void clearFilter();

        void showFilterBox(const QPoint point_filterBox, const QSize size_parentSize);
        void hideFilterBox();

        QSize update_fixedSize() const;
        QVariant get_selectedCode();

        void mousePressEvent(QMouseEvent *event) override;

    signals:
        void signal_clicked_filterOpt(QVariant clicked_optCode, QString clicked_optName);           ///< 선택한 필터링 옵션의 코드와 Name을 알려줌


    private slots:
        void slot_clicked_filterOptDataBtn();
        void slot_mousePressed_noData_widget(const Qt::MouseButton);


    private:

        // functions
        void setUIControl_basic();
        QLabel* make_headerLabel(QString headerText);
        QPushButton* make_fiterButton(QString btnText, int width, int height, QString style_normal, QString style_selected, bool flagSelected);

        // function about resize the FilterOptBox
        void update_fixedSize_afterAddGroup(const int maxCnt_forAddGroup);

        FilterOptSize m_filterOptSize;

        // UI
        QHBoxLayout *hBox_filterBox_main;           ///< 필터링 박스 위젯에서 사용하는 메인 레이아웃
        QWidget *widget_filterBox;                  ///< 실제 화면에 보이는 필터링 박스 위젯


        // Data
        int cnt_optGroup = 0;           ///< 사이즈 계산을 위해 필요함 (가로 계산)
        int max_optData = 0;            ///< 사이즈 계산을 위해 필요함 (세로 계산)

        QVariant selected_optCode;          // 단일 코드 옵션에 대해서

    };

};

#endif // BUGSCHOOSEFILTEROPT_H
