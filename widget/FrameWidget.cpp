#include "framewidget.h"

FrameWidget::FrameWidget(Type p_type, QWidget *parent)
    : QWidget(parent)
{
    type = p_type;
}

FrameWidget::~FrameWidget(){

}
