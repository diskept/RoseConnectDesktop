#ifndef REMOTEBTNITEM_H
#define REMOTEBTNITEM_H

#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
/**
 * @brief The RemoteBtnItem class 리모컨 위젯에서 사용하는 리모컨 버튼 아이템
 */
class RemoteBtnItem : public QWidget
{
    Q_OBJECT
public:
    explicit RemoteBtnItem(QWidget *parent = nullptr);
    ~RemoteBtnItem();

    void setData(QString p_url, QString p_text = nullptr);
    void setLabel(QString p_text = nullptr);
    void setCircle(int p_diameter);
    void setRectangle(int p_width, int p_height);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *lb_text;
    QLabel *lb_img;
    QWidget *widget_total;
    QPixmap *pixmap_icon;
    QVBoxLayout *vl_total;

};


#endif // REMOTEBTNITEM_H
