#ifndef PUSHBTN_WITHID_H
#define PUSHBTN_WITHID_H

#include <QPushButton>
#include <QWidget>


#define print_tidal_func() qDebug() << "[TIDAL][FILE][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


namespace tidal {

    class PushBtn_withID : public QPushButton
    {
        Q_OBJECT

    public:
        explicit PushBtn_withID(int p_btnID = -1, QWidget *parent = 0) : QPushButton(parent)
        {
            this->btnID = p_btnID;
            connect(this, SIGNAL(clicked()), this, SLOT(handleClick()));
        }

    signals:
        void btnClicked(int);

    private slots:
        void handleClick()
        {
            this->released();
            emit btnClicked(btnID);
        }

    private:
        int btnID;
    };


};

#endif // PUSHBTN_WITHID_H
