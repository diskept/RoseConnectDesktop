#ifndef ONOFFWIDGET_H
#define ONOFFWIDGET_H

#include <QWidget>
#include <QLinearGradient>
#include <QLabel>
#include <QPropertyAnimation>

/**
 * @brief 아이폰 스타일의 on/off 위젯
 */
class OnOffWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(OnOffWidget);

public:
    explicit OnOffWidget(QWidget *parent = nullptr);
    ~OnOffWidget();

    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void setEnabled(bool flagEnable);

    void setDuration(int duration);
    void setValue(bool flagOn);

    bool getCurrentValue() const;


signals:
    void signal_changed(bool flagOn);

private:
    class OnOffCircle;
    class OnOffBackground;
    void refreshUI();

    bool flagOn;
    int  duration;

    QLabel *lb_offText;
    OnOffBackground *wg_bg;
    QLabel *lb_onText;
    OnOffCircle *circle;

    bool flagEnabled;

    QPropertyAnimation* anim_circle;
    QPropertyAnimation* anim_bg;

    void setInit();
    void setUIControl();
};

/**
 * @brief 백그라운드 위젯
 */
class OnOffWidget::OnOffBackground : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(OnOffBackground)

public:
    explicit OnOffBackground(QWidget* parent = nullptr);
    ~OnOffBackground();

    void paintEvent(QPaintEvent* event) override;
};

/**
 * @brief Circle바
 */
class OnOffWidget::OnOffCircle : public QWidget
{
  Q_OBJECT
    Q_DISABLE_COPY(OnOffCircle)

public:
  explicit OnOffCircle(QWidget* parent = nullptr);
  ~OnOffCircle() override;

  void paintEvent(QPaintEvent* event) override;
};
#endif // ONOFFWIDGET_H
