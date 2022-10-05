#ifndef SQLITEVIEWTOTABLE_H
#define SQLITEVIEWTOTABLE_H

#include <QObject>
#include <QThread>
#include <common/sqlitehelper.h>


/**
 * @brief 로즈 DB의 View를 Table로 저장 @n
 * Thread에서 작업이 진행되며 모든 작업 완료후 시그널 @sa SqliteViewToTable::signal_finished() 발생
 */
class SqliteViewToTable : public QObject
{
    Q_OBJECT
public:
    explicit SqliteViewToTable(QObject *parent = nullptr);
    ~SqliteViewToTable();



signals:
    void signal_viewToTable();
    void signal_finished();

private slots:
    void viewToTable();

private:

    QThread thread;
    SqliteHelper *helper;


    void setInit();
};

#endif // SQLITEVIEWTOTABLE_H
