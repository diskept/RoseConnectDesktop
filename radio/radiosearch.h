#ifndef RADIOSEARCH_H
#define RADIOSEARCH_H

#include <QScrollArea>
#include <QWidget>

#include <common/linker.h>

#include <widget/flowlayout.h>

#include <data/dataradio.h>

/**
 * @brief 라디오 검색결과 화면 @n
 *      라디오 겸색결과 채널 목록으로 구성
 */
class RadioSearch : public QWidget
{
    Q_OBJECT
public:
    explicit RadioSearch(QWidget *parent = nullptr);
    void search(const QString &p_text);

signals:
    void signal_changedMenuName(const QString &p_menuName);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void slot_clickedHoverItem(const QString &p_code);

private :

    Linker *linker;
    QString strSearchWord;
    QList<QString> favorUrlList;
    QList<DataRadio*> dataList;

    QWidget *widgetEmpty;
    FlowLayout *flowLayout;
    QScrollArea *scrollArea_content;

    void setInit();
    void setUIControl();
    void showEmptyLabel(bool);
    void requestChannel();
    void requestFavoriteList();
    void appendChannel(const QJsonObject &jsonData);
    void setResultOfFavoriteList(const QJsonObject &p_jsonObject);
    void setResultOfChannelList(const QJsonObject &p_jsonObject);
};

#endif // RADIOSEARCH_H
