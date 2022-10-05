#ifndef SEGMENTTABWIDGET_H
#define SEGMENTTABWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>


/**
 * @brief 목록에서 필터링 탭 위젯 클래스.
 */
class SegmentTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SegmentTabWidget(QWidget *parent = nullptr);

    void setList_tabName(QList<QString>& list_name);
    void set_selectedIndex(int index);
    void set_fixedTabBtnSize(const QSize&);


signals:
    void signal_clicked(int tabIndex);

private slots:
    void slot_clickBtns();

private:
    void setUIControl();
    void applyBtnStyle();

    QHBoxLayout *hBox_main;
    int max_index;
    int selected_index;

    QList<QPushButton*> *list_btn;


};

#endif // SEGMENTTABWIDGET_H
