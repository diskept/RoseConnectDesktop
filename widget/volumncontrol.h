#ifndef VOLUMNCONTROL_H
#define VOLUMNCONTROL_H

#include <QLabel>
#include <QObject>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @brief The VolumnControl class 볼륨 조절 컨트롤
 */
class VolumnControl : public QWidget
{
    Q_OBJECT
public:
    explicit VolumnControl(QWidget *parent = nullptr);
    void setValueVolumn(int p_value);

signals:
    void clickedVolumnSlide(int pvalue);

private slots:
    void slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject);
    void setTextVolumn(int p_value);
    void setValueVolumn();


private:
    QLabel *lb_volumnNum;
    QSlider *slider_vol;
    void setUIControl();
    void showEvent(QShowEvent *event) override;
};

#endif // VOLUMNCONTROL_H
