#include "sectionbottomSlider.h"

#include <QLabel>
#include <QMouseEvent>
#include <QSlider>
#include <QWidget>

sectionbottomSlider::sectionbottomSlider(QWidget* parent)

   : QSlider(parent)
{

    linker = Linker::getInstance();//c220521

}
sectionbottomSlider::sectionbottomSlider(Qt::Orientation  orientation, QWidget *parent)

   : QSlider(orientation, parent)
{

    linker = Linker::getInstance();//c220521

}
sectionbottomSlider::~sectionbottomSlider()
{
}

void sectionbottomSlider::mousePressEvent ( QMouseEvent * e )//c220521
{
    //Pass mouse events to the parent control; otherwise, other mouse events will fail
        QSlider::mousePressEvent(e);
        //percentage

        //e->pos();
        double p = (double)e->pos().x()/(double)width();
        int val = p*(maximum()-minimum())+minimum();
        qDebug() << "sectionbottomSlider::mousePressEvent-- val= " << val;
        //setValue(val);

        //emit linker->signal_sliderclick(val);


}
void sectionbottomSlider::mouseReleaseEvent ( QMouseEvent * e )//c220521
{
    //Pass mouse events to the parent control; otherwise, other mouse events will fail
        QSlider::mouseReleaseEvent(e);
        //percentage

        //e->pos();
        double p = (double)e->pos().x()/(double)width();
        int val = p*(maximum()-minimum())+minimum();
        qDebug() << "sectionbottomSlider::mousePressEvent-- val= " << val;
        //setValue(val);

        //emit linker->signal_sliderclick(val);//c221007_1
        emit signal_sliderclick(val);//c221007_1


}

