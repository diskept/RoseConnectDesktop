#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>

/**
 * @brief 클릭가능한 QLabel
 */
class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = nullptr);
    ClickableLabel(const QString &text, QWidget *parent = nullptr);
    ~ClickableLabel();

    void setImageResource(QString p_url);

signals:
    void signal_clicked();
    void signal_mouseHovered();            ///< OptionPopup 구버전에서 사용중
    void signal_mouseOut();                ///< hover 이벤트가 있어서 out 이벤트도 추가해둠

protected:
    bool event(QEvent *event) override;

private:
    bool flagMousePressed = false;      ///< 마우스 Click 을 체크하기 위한 변수

};



#endif // CLICKABLELABEL_H
