#ifndef ROSETUBEBANNER_H
#define ROSETUBEBANNER_H

#include <QLabel>

/**
 * @brief 로즈튜브 배너 @n
 * @note 로즈튜브 채널 화면에서만 보인다.
 */
class RoseTubeBanner : public QLabel
{
    Q_OBJECT
public:
    explicit RoseTubeBanner(QWidget *parent = nullptr);

    QSize sizeHint() const override;

private:
    int heightForWidth(int) const override;
    void resizeEvent(QResizeEvent *event) override;

    const int bannerWidth = 1060;
    const int bannerHeight = 175;

};

#endif // ROSETUBEBANNER_H
