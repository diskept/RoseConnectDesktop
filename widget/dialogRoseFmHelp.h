#ifndef DIALOGROSEFMHELP_H
#define DIALOGROSEFMHELP_H

#include "common/linker.h"

#include <QDialog>
#include <QLabel>

/**
 * @brief The DialogRoseFmHelp class : RoseFM Help Dialog
 * @details Rose Connect 에서는 alert Dialog 없음.
 * @note 확인만 보이는 alert Dialog 도 지원되게 수정하기위해 enum DlgConfirmType 추가함
 */
class DialogRoseFmHelp : public QDialog
{
    Q_OBJECT
public:
    explicit DialogRoseFmHelp(QWidget *parent = nullptr);
    ~DialogRoseFmHelp();

    void setTitle(const QString &p_title);
    void setText(const QString &p_text);

private slots:
    void slot_confirm();

private:
    QLabel *lb_title;
    QLabel *lb_text;
    QPushButton *btn_cancel;

    Linker *linker;

    void setInit();
    void setUIControl();
};

#endif // DIALOGROSEFMHELP_H
