#ifndef DIALOG_NEWPLAYLIST_ONROSE_H
#define DIALOG_NEWPLAYLIST_ONROSE_H

#include <QDialog>
#include <QLineEdit>
#include <QKeyEvent>


namespace Dialog{

    class Dialog_NewPlaylist_OnRose : public QDialog
    {
        Q_OBJECT
    public:
        explicit Dialog_NewPlaylist_OnRose(QWidget *parent = nullptr);
        ~Dialog_NewPlaylist_OnRose();

        QString getTitle();
        QString getDestription();

    private slots:
        void slot_clicked_OK();

    private:
        void setInit();
        void setUIControl();
        bool checkUserInput();

        void keyPressEvent(QKeyEvent *event) override{
            if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
                return;
            }
            QDialog::keyPressEvent(event);
        };

    private:
        QLineEdit *edit_title;
        QLineEdit *edit_description;

        QString title = "";
        QString description = "";

        const int DLG_WIDTH = 600;
        const int DLG_HEIGHT = 310;
    };
};

#endif // DIALOG_NEWPLAYLIST_ONROSE_H
