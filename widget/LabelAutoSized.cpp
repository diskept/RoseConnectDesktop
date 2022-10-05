#include "LabelAutoSized.h"
#include <QDebug>
#include <common/gscommon.h>

namespace widget {

    /**
     * @brief LabelAutoSized::LabelAutoSized
     */
    LabelAutoSized::LabelAutoSized(QWidget *parent) : QLabel(parent){

    }

    void LabelAutoSized::setTextAutoResized(const QString text){
        this->org_text = text;
        this->setText(GSCommon::getTextCutFromLabelWidth(org_text, this->width(), this->font()));
    }

    void LabelAutoSized::paintEvent(QPaintEvent *event){
        QLabel::paintEvent(event);
        if(this->text() != this->org_text){
            this->setText(GSCommon::getTextCutFromLabelWidth(org_text, this->width(), this->font()));
        }
    }

}
