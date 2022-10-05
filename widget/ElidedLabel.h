#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QWidget>
#include <QFrame>

/**
 * @brief 텍스트 여러줄에 대해서 elided 지원을 하기 위한 Customizing 라벨 클래스.
 * @details Qt Doc 사이트를 참고하였음, Text를 Painting 할 때, ContentsMargin이 고려되어 painting 되도록 하였음. @n
 *      resizing 에 대해서도 처리되도록 추가 구현함. @n
 *      참고 Qt Doc - @ref https://doc.qt.io/qt-5/qtwidgets-widgets-elidedlabel-example.html
 */
class ElidedLabel : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool isElided READ isElided)

public:
    explicit ElidedLabel(const QString &text, QWidget *parent = nullptr);

    void setText(const QString &text);
    const QString & text() const { return content; }
    bool isElided() const { return elided; }

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void elisionChanged(bool elided);

private:
    bool elided;
    QString content;
};

#endif // ELIDEDLABEL_H
