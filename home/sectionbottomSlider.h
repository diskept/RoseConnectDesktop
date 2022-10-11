#ifndef SECTIONBOTTOMSLIDER_H
#define SECTIONBOTTOMSLIDER_H

#include "common/filedownloader.h"
#include "common/linker.h"

#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QSlider>
#include <QWidget>


#define print_home_func() qDebug() << "[HOME][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";

/**
 * @brief The SectionBottom class 하단 재생바 @n
 * @note
 *      전체 영역 클릭시 @sa PlayFullScreenRelation 전체 재생페이지로 이동 @n
 *      음원 타입에 따라 shuffle 아이콘 비활성
 */
class sectionbottomSlider : public QSlider
{
    Q_OBJECT


public:
    explicit sectionbottomSlider(QWidget *parent = 0);
    explicit sectionbottomSlider(Qt::Orientation orientation, QWidget *parent = nullptr);

    ~sectionbottomSlider();
protected:
    void mousePressEvent ( QMouseEvent * e ) override;//c220521
    void mouseReleaseEvent ( QMouseEvent * e ) override;//c220521

signals:
    void signal_sliderclick(int);//c221007_1

private:
    Linker *linker;//c220521

};

#endif // SECTIONBOTTOMSLIDER_H
