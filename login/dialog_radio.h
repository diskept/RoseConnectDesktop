#ifndef DIALOG_RADIO_H
#define DIALOG_RADIO_H

#include <QDialog>
#include <QKeyEvent>
#include <QRadioButton>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QList>

namespace Dialog{

/**
 * @brief 라디오버튼을 이용한 항목 선택 Dlg
 *
 * @note use 설정-미디어라이브러리 항목 더보기
 */
class Dialog_Radio : public QDialog
{
    Q_OBJECT
public:
    explicit Dialog_Radio(QWidget *parent = nullptr);
    ~Dialog_Radio();

    void setTitle(const QString &p_title);
    void setRadioButtons(const QStringList &p_list);
    int getSelectedIndex() const;

private slots:
    void slot_clickedOk();

private :

    const int DLG_WIDTH = 600;
    const int DLG_HEIGHT = 380;

    QString radio_style;

    QLabel *lb_title;
    QVBoxLayout *box_radio;

    QList<QRadioButton*> *listRadio;

    void setInit();
    void setUIControl();
    void clearRadio();

    void keyPressEvent(QKeyEvent *event) override{
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
            return;
        }
        QDialog::keyPressEvent(event);
    }
};
};

#endif // DIALOG_RADIO_H
