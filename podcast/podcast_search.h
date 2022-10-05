#ifndef PODCAST_SEARCH_H
#define PODCAST_SEARCH_H

#include <QWidget>
#include <QScrollArea>
#include <common/linker.h>

#include <widget/flowlayout.h>

/**
 * @brief 팟캐스트 검색 결과 화면 @n
 *      검색결과 채널 목록으로 구성
 */
class Podcast_Search : public QWidget
{
    Q_OBJECT
public:
    explicit Podcast_Search(QWidget *parent = nullptr);
    void search(const QString &p_text);

signals:
    void signal_changedMenuName(const QString &p_menuName);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private:

    Linker *linker;
    QString strSearchWord;

    QWidget *widgetEmpty;
    FlowLayout *flowLayout;
    QScrollArea *scrollArea_content;

    void setInit();
    void setUIControl();
    void showEmptyLabel(bool);
    void requestChannel();
    void appendChannel(const QJsonObject &jsonData);
    void setResultOfChannelList(const QJsonObject &p_jsonObject);
};

#endif // PODCAST_SEARCH_H
