#ifndef SEARCHREMOTEBAR_H
#define SEARCHREMOTEBAR_H

#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QLabel>//c220715
#include <QVBoxLayout>
#include "common/linker.h"
#include "widget/customlineedit.h"
#include "widget/clickablelabel.h"

/**
 * @brief The SearchRemoteBar class 공통 상단 - 검색 및 리모컨
 */
class SearchRemoteBar : public QWidget
{
    Q_OBJECT
public:
    explicit SearchRemoteBar(QWidget *parent = nullptr);
    ~SearchRemoteBar();
    void setInitUIRemoteBtn();
    void setSelectedRemoteBtnStyle();
    void resetFixedWidth(int p_width);
    void setsearchtext(QString a);//searchtextcopy
    void searchbarClear(){
            //le_search->setText("");
            //le_search->clear();//c220726
            le_search->clearFocus();
        };//c220628


signals:
    void clickedRemote(bool);

private slots:
    void slot_dragDrop_search_singleshot();//c220726
    void slot_dragDrop_clear_singleshot();//c220726
    void slot_get_dragDropText();//c220726
    void slot_clear_dragDropText();//c220726
    void clickedRemoteBtn();
    void clickedRemoteBtn_del();//c220715
    void slot_textChanged(QString);
    void slot_returnPressed(const QString p_text);
    void slot_downPressed(const QString p_text);//c220904_1
    void slot_upPressed(const QString p_text);//c220904_1
    void slot_sugestionbarOld_sel(const QString p_text);
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);

    void slot_redirectUrl(const QString url);   //j220903 twitter
    void slot_searchCopy(QString a);//c220904_1

private:

    Linker *linker;

    QPixmap pixmapDelete;//c220715
    bool flagSelectedRemote = false;
    CustomLineEdit *le_search;
    QPushButton *btn_remote;
    ClickableLabel *btn_del;//c220715

    QJsonObject make_requestPlayYouTubeJson(const QJsonObject &jsonTrackData);
    void requestPlayAll(bool p_flagShuffle);//c220726
    void resizeEvent(QResizeEvent* event) override;;//c220715
    bool IsKorean(QChar ch);
    void setInit();
    void setUIControl();
    QString getUrlResponse(QString tmp_str);//c220818

    QString redirectUrl_before = "";    //j220903 twitter

};

#endif // SEARCHREMOTEBAR_H
