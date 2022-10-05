#ifndef VIDEOSEARCH_H
#define VIDEOSEARCH_H

#include <QScrollArea>
#include <QWidget>

#include <common/linker.h>

#include <widget/flowlayout.h>

/**
 * @brief 비디오 검색 결과 @n
 *      검색 결과 영상이 포함된 폴더 목록으로 구성
 */
class VideoSearch : public QWidget
{
    Q_OBJECT
public:
    explicit VideoSearch(QWidget *parent = nullptr);
    ~VideoSearch();

    void search(const QString &p_text);

signals:
    void signal_changedMenuName(const QString &p_menuName);

private slots:
    //void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

private:
    void setInit();
    void setUIControl();
    void showEmptyLabel(bool);
    void setVideo();
    QJsonArray searchVideo();

    void serachMoreVideo();
    void wheelEvent(QWheelEvent *event) override;

private :

    Linker *linker;
    QString strSearchWord;

    QWidget *widgetEmpty;
    FlowLayout *flowLayout;
    QScrollArea *scrollArea_content;

    bool flag_page_first = false;

    int OffsetPoint = 0;

    int scroll_pos_currunt = 0;
    int scroll_pos_before = 0;
};

#endif // VIDEOSEARCH_H
