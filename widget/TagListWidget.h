#ifndef TAGLISTWIDGET_H
#define TAGLISTWIDGET_H

#include "widget/flowlayout.h"
#include <QWidget>


/**
 * @brief 태그형식의 여러개 버튼이 있는 Widget. 각 버튼은 클릭이 가능한 구조임
 * @details
 *      실제로 태그에 대한 ID, Name에 대한 데이터를 가지고 있지는 않는다. UI만 처리함.@n
 *      사용하는 곳에서 signal_clickedTags(int index) 에 대하여 index 로 데이터를 처리하는 것으로.@n
 *      혹시나 Bugs 이외에도 다른 곳에서 사용할 수 있도록, Data는 제거하고 View 구조만 두었음.@n
 *      Bugs 에서 현재 사용중.
 */
class TagListWidget : public QWidget
{
    Q_OBJECT;
public:
    explicit TagListWidget(QWidget *parent = nullptr);

    void add_tag(QString tag_nm);
    void add_tag_list(QList<QString> list_tag_nm);
    void clear_tags();

signals:
    void signal_clickedTags(int tag_index);


private slots:
    void slot_btnTagClicked();

private:
    // about function
    void setUIControl_basic();

    // UI
    FlowLayout *flowLayout_main;

    // Data
    int num_tags = 0;

};

#endif // TAGLISTWIDGET_H
