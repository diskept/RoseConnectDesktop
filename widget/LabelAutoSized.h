#ifndef LABELAUTOSIZED_H
#define LABELAUTOSIZED_H

#include <QLabel>

namespace widget {

    /**
     * @brief Auto Sizing을 지원하는 Label.
     * @details 현재는 미사용. 대신 @sa ElidedLabel 사용중
     */
    class LabelAutoSized : public QLabel
    {
    public:
        explicit LabelAutoSized(QWidget *parent = nullptr);
        void paintEvent(QPaintEvent *) override;

        void setTextAutoResized(const QString text);

    private:
        QString org_text;
    };

};

#endif // LABELAUTOSIZED_H
