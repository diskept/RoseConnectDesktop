#ifndef CATEGORYSELECTOR_H
#define CATEGORYSELECTOR_H

#include <QWidget>
#include "widget/flowlayout.h"
#include "home/abstractpage.h"

class CategorySelector : public QWidget
{
    Q_OBJECT
public:


    explicit CategorySelector(QWidget *parent = nullptr);
    void setData(const QJsonObject &p_jsonObject);

signals:
    void signal_clickedViewAll(const QJsonObject &p_jsonObject);

public slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedItem();

private:

    void setInit();
    void setUIControl();
    void setDataFromAPI();
    void appendCategoryUI(const QJsonArray &p_jsonArray);
    void setResultOfCategory(const QJsonObject &p_jsonObject);


    int index;
    QString type;

    FlowLayout *flowLayout;
};

#endif // CATEGORYSELECTOR_H
