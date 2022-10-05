#ifndef MYPHOTOEDIT_H
#define MYPHOTOEDIT_H

#include <QWidget>
#include <qlabel.h>

/**
 * @brief The MyPhotoEdit class 설정>마이페이지의 편집화면에서 사용하는 사진1개에 대한 View 클래스 (삭제버튼 포함)
 */
class MyPhotoEdit : public QWidget
{
    Q_OBJECT
public:
    explicit MyPhotoEdit(QWidget *parent = nullptr);
    void setDataMyPhotoEdit(QString p_url);

signals:
    void signal_deletePhoto(QString p_imgPath);

private slots:
    void slot_loadImage();
    void slot_deletePhoto();

private :
    QString imgPath;
    QLabel *lb_img;

    void setUIControl();

};

#endif // MYPHOTOEDIT_H
