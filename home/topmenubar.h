#ifndef TOPMENUBAR_H
#define TOPMENUBAR_H

#include "topmenusubitem.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QLabel>

#include <common/linker.h>
#include <QApplication>
#include <QCoreApplication>

/**
 * @brief The TopMenuBar class 상단 메인메뉴명Text와 연관된 서브메뉴명 보이는 위젯
 * @note 앨범상세 화면에서 서브메뉴명만 보이는 스타일과 비슷한 UI가 있어서 flagShowMainTitle 를 이용하여 메인메뉴명Text는 가린채 위젯 사용함
 */
class TopMenuBar : public QWidget
{
    Q_OBJECT
public:
    /// 어떤 스타일의 메뉴로 보일지 enum
    enum ShowMenuStyle {
        TopSubMenuBar,    ///< 상단 서브메뉴바 (기본스타일)
        AlbumDetail,      ///< 앨범상세 페이지의 Tab 스타일
        FriendManager,    ///< 친구관리 페이지의 Tab 스타일
        FriendSubmenubar,  ///< 친구관리sub 화면용//cheon211018
        ListSubMenuBar,
        CDplayHome //c220724
    };
    explicit TopMenuBar(ShowMenuStyle p_showMenuStyle = ShowMenuStyle::TopSubMenuBar, QWidget *parent = nullptr);
    //explicit TopMenuBar(bool p_flagShowMainTitle=true, QWidget *parent = nullptr);

    void setDataTopMenuBar(QJsonObject p_jsonObject_main, QJsonArray *p_jsonArray_sub = nullptr );
    void setSelectedSubMenu(QString p_subMenuCode);
    void setSelectedSubMenuNoSignal(QString p_subMenuCode);
    int getCurrMainMenuCode(){ return this->mainMenuCode; }
    void setBGTransparent(bool p_flagTransparent);
    //QLabel *get_addUIControl_mainTitle(const QString mainTitle, QPushButton *a, QPushButton *b, QPushButton *c, QPushButton *d, QPushButton *e);//cheon211008
    //void addUIControl_dividedLine_onMain();//cheon211008

signals:
    void changedSubMenu(const QJsonObject &p_data);
    void signal_clickedSameSubMenu(QString p_subMenuCode);


public slots:
    //void slot_clickedMusic();//cheon211108
    //void slot_clickedBugs();//cheon211008-serach
    //void slot_clickedTidal();//cheon211008-serach
    //void slot_clickedQobuz();//cheon211008-serach
    //void slot_clickedRoseTube();//cheon211008-serach

    void clickedSubMenu(QString p_subMenuCode);    
    void slot_changedMenuNameText(const QString &p_menuName);       ///< 오직 메뉴명 타이틀 텍스트 수정한다.
    //void slot_changedMenuNameText_search(const QString &p_menuName, const QString &menu);       ///< 오직 메뉴명 타이틀 텍스트 수정한다.//cheon211008
    void slot_changedSubMenuNameText(const QString &p_subMenuName); ///< 오직 현재선택된 서브메뉴명 텍스트를 수정한다.

private:
    /*
    /// 메인메뉴명 타이틀을 보이게할지 말지 여부 설정
    bool flagShowMainTitle = "";
    */
    int menu_width_sum = 0;//c230323
    Linker *linker;//cheon211008
    QString title_org;                          ///< 타이틀텍스트 (메인메뉴명)
    ShowMenuStyle showMenuStyle;            ///< 어떤 스타일로 보일지 값
    int mainMenuCode = 0;                   ///< mainMenuCode 메인메뉴의 code
    QString curr_subMenuCode;               ///< curr_subMenuCode 현재 선택된 서브메뉴의 code
    QList<TopMenuSubItem*> list_subMenuItem;    ///< list_subMenuItem 서브메뉴 객체 리스트

    QLabel *lb_mainName;
    QHBoxLayout *hlayout_menu, *hl_filter;//cheon211008
    QHBoxLayout *hlayout_menuSub;
    QVBoxLayout *vlayout_menuSearch;//cheon211008
    QHBoxLayout *hlayout_etc;
    QWidget *widget_topMenuBar;
    QWidget *line_bottom_1;
    QWidget *line_bottom_2;
    QWidget *widget_btnFilter;//cheon211008

    void resizeEvent(QResizeEvent *event) override;
};

#endif // TOPMENUBAR_H
