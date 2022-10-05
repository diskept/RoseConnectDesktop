#ifndef TOPNAVIBAR_H
#define TOPNAVIBAR_H

#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include <widget/searchremotebar.h>

/**
 * @brief 상단 페이지 이동 QWidget
 * @note
 *      Prev, Next 클릭시 마다 @sa clickedBack() , @sa clickedNext() 시그널 발생
 *      @sa SearchRemoteBar 검색입력및 리모컨 버튼 QWidget 관리
 */
class TopNaviBar : public QWidget
{
    Q_OBJECT
public:
    explicit TopNaviBar(QWidget *parent = nullptr);
    void setEnabledBtnBack(bool p_flagEnabled);
    void setEnabledBtnNext(bool p_flagEnabled);
    void hideRemoveWidget();
    void showRemoveWidget();
    void search_text_copy();//searchtextcopy
    void search_text_clear();


signals:
    void clickedBack();
    void clickedNext();

    void clickedRemote(bool);

protected:
    //void mousePressEvent(QMouseEvent *event);

private slots:
    void slot_searchBarClear();//c220628
    void slot_search_text_clear();//c220726
    void slot_search_text_copy();//c220726
    void clickedBackBtn();
    void clickedNextBtn();

private:
    Linker *linker;//c220628
    bool flagEnabledBack = false;
    bool flagEnabledNext = false;
    //bool flagSelectedRemote = false;
    QPushButton *btn_back;
    QPushButton *btn_next;
    //QLineEdit *le_search;
    //QPushButton *btn_remote;

    SearchRemoteBar *searchRemoteBar;

    void resizeEvent(QResizeEvent *event) override;
};

#endif // TOPNAVIBAR_H
