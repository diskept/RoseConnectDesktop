#ifndef SUGGESTIONSBAR_H
#define SUGGESTIONSBAR_H

#include <QWidget>
#include <QJsonArray>
#include <QVBoxLayout>
#include <QListWidget>
#include <delegate/suggestiondelegate.h>
#include <common/linker.h>

/**
 * @brief 검색 제안어 목록
 * @note Google suggestion search 결과 QWidget @n
 *      @sa Linker::signal_search() 를 통해 App에 검색 요청
 */
class SuggestionsBar :  public QWidget
{
    Q_OBJECT
public:
    explicit SuggestionsBar(QWidget *parent = nullptr);
    void setSuggestions(QJsonArray p_jsonArray);
    void setSuggestions_old();//c220620
    int getSuggestionsCount();


private slots:

    void slot_delegateClicked(const int &p_index, const int &p_btnType);//c220629
    void slot_suggestionClicked(QListWidgetItem*);
    void slot_suggestionBarDown(QString text);//c220904_1

    void slot_suggestionBarUp(QString text);//c220904_1



private:

    struct search_textData {//c220629
        int index = 0;
        int idx = 0;
        QString search_text = "";



    };
    QList<search_textData> searchList;//c220929
    Linker *linker;
    SuggestionDelegate *delegate;

    QVBoxLayout *box_suggestions;
    QListWidget *listWidget;
    int positionSel = -1;//c220904_1

    void setInit();
    void setUIControl();
    void proc_selOldDBDel(int idx_del);//c220629
    void proc_selOldDBSel(int idx_del);//c220629
};

#endif // SUGGESTIONSBAR_H
