#ifndef DIALOGCONFIRM_MACSHARE_H
#define DIALOGCONFIRM_MACSHARE_H

#include <QDialog>
#include <QLabel>
#include <QListWidget>//c220913_1
#include <delegate/sharefolderdelegate.h>//c220913_1
#include "common/linker.h"//c220913_1

/**
 * @brief The DialogConfirm class : Confirm Dialog
 * @details Rose Connector 에서는 alert Dialog 없음.
 * @note 확인만 보이는 alert Dialog 도 지원되게 수정하기위해 enum DlgConfirmType 추가함
 */
class DialogConfirm_MacShare : public QDialog//c220913_1
{
    Q_OBJECT
public:
    enum DlgConfirmType {
        OKCancel,       ///< 일반적인 Confirm 창 (확인,취소 버튼 존재. 상단 X 닫기버튼 있음)
        OK              ///< 확인버튼만 존재 (상단 X 닫기버튼도 없음. only 수락만 가능하게끔)
    };
    explicit DialogConfirm_MacShare(QWidget *parent = nullptr, DlgConfirmType p_confirmType=DlgConfirmType::OKCancel);

    void setTitle(const QString &p_title);
    void setText(const QString &p_text);
    void setLayoutMy(QLayout *a);//c220709
    void setLayoutMy_withText(QLayout *a);//c220804
    void setTextHeight(const int &h);//c220511
    void setAlignment(Qt::Alignment);//c220511
    void setAlertMode();
    void yellow_setStyleSheet();//cheon211203
    void setSuggestions(QJsonObject p_jsonArray);//c220913_1

private slots://c220913
    void slot_itemClicked();//c220913
    void slot_delegateClicked(const int &p_index, const int &p_btnType);//c220913_1

private:
    Linker *linker;//c220913_1
    SharefolderDelegate *delegate;//c220913_1
    QListWidget *listWidget;//c220913_1
    DlgConfirmType confirmType;
    QLabel *lb_title;
    QLabel *lb_text;
    QPushButton *btn_cancel;
    QWidget *wg_total;//cheon211203
    QWidget *wg_body;//c220709

    void setInit();
    void setUIControl();
};

#endif // DIALOGCONFIRM_MACSHARE_H
