#ifndef MUSICCATEGORY_H
#define MUSICCATEGORY_H

#include "musiclist_group.h"
#include "musiclist_one.h"

#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QWidget>
#include <QSettings>

#include <home/abstractpage.h>

/**
 * @brief 음악 > 분류 메뉴 (AbstractPage 상속)
 * @details 크게 4개의 위젯을 StackWidget으로 관리하고 있다. 4개의 위젯은 다음과 같이 구성되어있다. @n
 * 1) @sa MusicList_One 전체 곡 위젯 @sa MusicList_Group  장르, 아티스트, 작곡가
 * @note
 *      전체곡/장르/아티스트/작곡가 버튼을 클릭할때마다 페이지가 변경되었다는 시그널을 발생시킨다.
 */
class MusicCategory : public AbstractPage
{
    Q_OBJECT
public:
    explicit MusicCategory(QWidget *parent = nullptr);
    ~MusicCategory();
    void setDataABSPage(QJsonObject p_data) override;

signals:

private slots:
    void slot_changedSubTabUI();
    void slot_changedSubTabUI(const QJsonObject &p_data) override;
    void slot_clickedBtnFilter();
    void slot_changedBtnFilter();

private :
    QLabel *label;
    QHBoxLayout *hl_filterAndBtn;
    QPushButton *btn_sort_audio;
    QPushButton *btn_sort_kind;
    QPushButton *btn_sort_artist;
    QPushButton *btn_sort_composer;
    QVBoxLayout *vl_total;
    QWidget *widget_filter;

    QStackedWidget *stackedwidget;

    MusicList_One *musicList_audio;     ///< 곡 위젯
    MusicList_Group *musicList_kind;    ///< 장르 위젯
    MusicList_Group *musicList_artist;  ///< 아티스트 위젯
    MusicList_Group *musicList_composer;    ///< 작곡가 위젯

    QSettings *settings;


    QString btnStyle_normal;
    QString btnStyle_selected;
    QStringList fileTypeList;
    QStringList bitDepthList;
    QStringList samplingList;
    QStringList orderList;
    QStringList starList;

    QHBoxLayout *hl_filter;

    QLabel *lb_file_type;
    QLabel *lb_bit_depth;
    QLabel *lb_sample_rate;
    QLabel *lb_star_point;
    QLabel *lb_order;

    bool flag_file_type = false;
    bool flag_bit_depth = false;
    bool flag_sample_rate = false;
    bool flag_star_point = false;
    bool flag_order = false;

    QPushButton *btn_filter;
    QPushButton *btn_file_type[10];
    QPushButton *btn_bit_depth[10];
    QPushButton *btn_sample_rate[10];
    QPushButton *btn_star_point[10];
    QPushButton *btn_order[10];

    QVariant selected_optFilter;
    QVariant selected_optIdx;

    void setInit();
    void setUIControl();
    void setUISortType(QString p_type);
    void setFIlterUIControl();
    QLabel* createFilterLabel(const QString &p_label);
    QPushButton* createFilterBtn(const QString p_filter, const int idx, const QString &p_text, bool flagSelected=false);

    virtual QJsonObject getDataJson() override;

};

#endif // MUSICCATEGORY_H
