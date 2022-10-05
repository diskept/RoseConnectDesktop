#ifndef STACKEDWIDGET_AR_H
#define STACKEDWIDGET_AR_H

#include <QVBoxLayout>
#include <QWidget>

// 현재는 미사용중인 클래스임 ---------------------- maded by sunnyfish, commented by 미선

/**
 * @brief QStackedWidget 처럼 동작하되, 자식 Widget에 따라서 auto resizing 되도록 함
 * @details QStackedWidget은 Widget -> Widget 구조로 되어 있어서 size policy에 따라 sizing 처리되지 않는 것 같다. @n
 * 그래서 Widget > Layout > Widget 구조가 되도록, 내부에 Layout을 두어 관리하는 구조로 만들도록 함. @n
 * 아직, QStackedWidget 클래스의 함수들을 모두 커버하도록 만들지는 않았음. 필요할 때 documentaion 참고해서 넣으려고. 우선 급하니까. @n
 * 참고링크 @ref https://doc.qt.io/qt-5/qstackedwidget.html  (QStackedWidget Docs.)
 */
class StackedWidget_AR : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(int currentIndex READ currentIndex)

public:
    explicit StackedWidget_AR(QWidget *parent = nullptr);

    // Auto Resizing을 위해 - 추가사항
    void setAutoResize(bool flagAuto);
    QSize sizeHint() const override;


    // QStackedWidget 에서 지원하는 function들 처럼 구성
    int addWidget(QWidget *child_widget);
    int count() const;
    int currentIndex() const;
    QWidget* currentWidget() const;
    int indexOf(QWidget *widget) const;
    int insertWidget(int index, QWidget *widget);
    void removeWidget(QWidget *widget);
    QWidget *widget(int index) const;


public slots:
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *widget);


signals:
    void currentChanged(int index);
    void widgetRemoved(int index);


protected:
    virtual bool event(QEvent *event) override;


private slots:
    void slot_currentChanged(int index);

private:
    bool flagAutoResize;                ///< 기본값 true
    int m_currentIndex;                 ///< 현재 보이는 자식 Widget의 index

    QList<QWidget *> list_childWidget;      ///< 자식 관리용도

    QVBoxLayout *vBox_main;                 ///< 자식을 넣고 관리하는 Layout

};

#endif // STACKEDWIDGET_AR_H
