#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QHBoxLayout>
#include <QWidget>


/**
 * @brief 목록 상단에 필터 버튼(깔때기 아이콘)과 함께, 현재 어떤 필터링 옵션이 선택되었는지 화면에 보여주는 커스텀 위젯.
 * @details 필터 아이콘이 클릭이 되었다는 signal과 @sa signal_clickedFilter() 과, @n
 *      현재 어떤 필터가 선택되었는지 사용자에게 화면 노출을 위하 @sa add_filter(QString,QString) 함수가 주된 기능임. @n
 *      Tidal, Bugs 에서 사용중
 */
class FilterWidget : public QWidget
{
    Q_OBJECT;
public:
    explicit FilterWidget(QWidget *parent = nullptr);

    void add_filter(QString code, QString name);
    void clear_filter();

signals:
    void signal_clickedFilter();


private slots:
    void slot_btn_filterClicked(bool checked);

private:
    void setUIControl_basic();

    // UI
    QHBoxLayout *hBox_viewFiltered;

    // Data
    QList<QString> *list_code;
    QList<QString> *list_name;


};

#endif // FILTERWIDGET_H
