#ifndef ROSEFM_MAIN_H
#define ROSEFM_MAIN_H

#include "rosefm_home.h"

#include <QWidget>
#include <QMutex>

#include <home/abstractmaincontent.h>



/**
 * @brief 로즈FM 메인메뉴의 총괄 @n
 *      로즈FM 대항목 관련 모든 QWidget 관리
 */
class RoseFmMain : public AbstractMainContent
{
    Q_OBJECT
public:
    explicit RoseFmMain(QWidget *parent = nullptr);
    ~RoseFmMain();

signals:


protected:
    void movePageOnly(QString p_pageCode, const QJsonObject &p_data) override;

private slots:
    void slot_search(const QString &p_text);

private :
    // ★ Linker 변수 이미 있으니 추가하지마세요.. protected에 있어요

    // 메뉴 관련 페이지  ----------------------
    RoseFmHome *roseFmHome = nullptr;       ///< 메뉴 : 로즈FM > 홈

    void setInit();
    void setUIControl();

    QMutex mutex;
};

#endif // ROSEFM_MAIN_H
