#ifndef DIALOG_NEWPLAYLIST_H
#define DIALOG_NEWPLAYLIST_H

#include <QDialog>
#include <QLineEdit>
#include <QKeyEvent>

namespace Dialog{

    /**
     * @brief 신규 플레이리스트 생성 Dlg @n
     *      신규 플레이리스트명만 입력 받는다.
     */
    class Dialog_NewPlaylist : public QDialog
    {
        Q_OBJECT
    public:

        explicit Dialog_NewPlaylist(QWidget *parent = nullptr);
        QString getName(){return edit_name->text();};


    private slots:
        void slot_clickedOk();

    private :

        const int DLG_WIDTH = 600;
        const int DLG_HEIGHT = 250;


        QLineEdit *edit_name;

        void setInit();
        void setUIControl();
        bool checkUserInput();

        void keyPressEvent(QKeyEvent *event) override{
            if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
                return;
            }
            QDialog::keyPressEvent(event);
        };
    };
};
#endif // DIALOG_NEWPLAYLIST_H
