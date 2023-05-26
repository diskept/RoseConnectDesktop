#ifndef DOWNLOADPROGRESS_H
#define DOWNLOADPROGRESS_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

/**
 * @brief DB 파일 다운로드 프로그레스 위젯
 */
class DownloadProgress : public QWidget
{
     Q_OBJECT
public:
    explicit DownloadProgress(qint64 p_max, QWidget *parent = nullptr);
    ~DownloadProgress();

    void hideText();
    void setMaxValue(const qint64 &p_maxValue);
    void setCurrentValue(const qint64 &p_currentValue, const qint64 &p_totalValue);
    void showRefreshBtn();
signals:
    void signal_progressCompleted();
    void signal_clickedRefresh();

private slots:

private:
    void setInit();
    void setUIControl();

    //QMovie *movieProgress;
    QLabel *lb_icon;
    QLabel *lb_text;
    QPushButton *btn_refresh;
    QProgressBar *progressBar;
    qint64 currentValue;
    qint64 maxValue;

    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;


};

#endif // DOWNLOADPROGRESS_H
