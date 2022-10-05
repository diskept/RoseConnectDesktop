#ifndef ABSTRACTPAGE_H
#define ABSTRACTPAGE_H

#include <QWidget>

/**
 * @brief The AbstractPage class 컨텐츠 위젯을 페이지 단위로 관리하기 위한 클래스@n
 *      각 페이지 코드, 서브 페이지를 관리하며 페이지 변경에 대한 signal & slot 작성
 */
class AbstractPage : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractPage(QWidget *parent = nullptr);
    virtual void setDataABSPage(QJsonObject p_data);

signals:
    /// 부모에게 페이지가 변경되었다고 히스토리 관리하라고 알려줘야한다. (Main에서 이전/다음 버튼 컨트롤하기 위함)
    void clickedSubTab(const QJsonObject &p_data);

protected slots:
    /// 내부 페이지 변경되었을때 슬롯 함수
    virtual void slot_changedSubTabUI(const QJsonObject &p_data);

protected:
    /// AbstractMainContent가 보여야할 위젯의 코드
    QString pageCode = "";
    /// 현재 클래스에서 step관리 하는 경우 코드값 (없으면 step이 존재하지 않는다는 뜻임)
    QString contentStep = "";

    virtual QJsonObject getDataJson();

};

#endif // ABSTRACTPAGE_H
