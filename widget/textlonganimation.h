#ifndef TEXTLONGANIMATION_H
#define TEXTLONGANIMATION_H

//#include <QBasicTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>


/**
 * @brief The TextLongAnimation class 전체재생화면에서 텍스트가 긴 경우 Scrollbar로 넘기는것같은 Animation효과
 */
class TextLongAnimation : public QWidget
{
    Q_OBJECT
public:
    explicit TextLongAnimation(int p_textAreaWidthMax, int p_textAreaWidthMin, int p_textAreaHeight, QWidget *parent = nullptr);
    void setText(const QString &newText);

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    QLabel *lb_text;
    QVBoxLayout *vl_total;

    const int INIT_POINT_X = -123456;   ///< 세팅전후를 구분하기 위해.. 값을 저렇게 세팅하였음
    //QBasicTimer timer;

    QString text;

    int startPosX = INIT_POINT_X;       ///< 텍스트 움직이는 시작 위치
    int textAreaWidth = 0;              ///< 텍스트가 보여질 영역의 Width
};





class LabelLongAnimation : public QWidget
{
    Q_OBJECT
public:
    explicit LabelLongAnimation(int p_textAreaWidthMax, int p_textAreaWidthMin, int p_textAreaHeight, QWidget *parent = nullptr);
    ~LabelLongAnimation();

    void setText(const QString &newText);

private slots:
    void tmr_move_text();

private:
    QTimer *timer;

    QLabel *lb_text;
    QWidget *widget_total;
    QVBoxLayout *vBox_total;

    QString text;

    int startPosX = 0;
    int textAreaWidth = 0;
    int text_length = 0;

    bool falg_connect = false;
};

#endif // TEXTLONGANIMATION_H
