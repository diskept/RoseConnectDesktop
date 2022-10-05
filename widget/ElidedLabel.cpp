#include "ElidedLabel.h"

#include <QTextLayout>
#include <QPainter>


// Except for initializing the member variables, the constructor sets the size policy to be horizontally expanding, since it's meant to fill the width of its container and grow vertically.
ElidedLabel::ElidedLabel(const QString &text, QWidget *parent)
    : QFrame(parent), elided(false), content(text) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}


// Changing the content require a repaint of the widget.
void ElidedLabel::setText(const QString &newText) {
    content = newText;
    update();
}


void ElidedLabel::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);

    QPainter painter(this);
    QFontMetrics fontMetrics = painter.fontMetrics();

    bool didElide = false;
    int lineSpacing = fontMetrics.lineSpacing();

    int y = this->contentsMargins().top();
    int left = this->contentsMargins().left();

    int width_draw = width() - this->contentsMargins().left() - this->contentsMargins().right();
    int height_draw = this->height() - this->contentsMargins().top() - this->contentsMargins().bottom();



    QTextLayout textLayout(content, painter.font());
    textLayout.beginLayout();
    forever {

        QTextLine line = textLayout.createLine();

        if (!line.isValid())
            break;

        line.setLineWidth(width_draw);
        int nextLineY = y + lineSpacing;

        if (height_draw >= nextLineY + lineSpacing) {
            line.draw(&painter, QPoint(left, y));
            y = nextLineY;
        } else {
            QString lastLine = content.mid(line.textStart());
            QString elidedLastLine = fontMetrics.elidedText(lastLine, Qt::ElideRight, width_draw);
            painter.drawText(QPoint(left, y + fontMetrics.ascent()), elidedLastLine);
            line = textLayout.createLine();
            didElide = line.isValid();
            break;
        }
    }

    textLayout.endLayout();

    if (didElide != elided) {
        elided = didElide;
        emit elisionChanged(didElide);
    }
}
