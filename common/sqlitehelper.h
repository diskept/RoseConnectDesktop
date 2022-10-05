#ifndef SQLITEHELPER_H
#define SQLITEHELPER_H

#include <QObject>

#include <QtSql/QSql>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>


const QString DB_NAME_ROSE = "rose";
const QString DB_NAME_ROSE_OLD = "rose_old";
const QString DB_NAME_ROSE_DESKTOP = "rose_desktop";

/**
 * @brief The SqliteHelper class QtSql 헬퍼 클래스 @n
 *      공통 DB 접속(드라이버명,디비패스,디비명) @sa SqliteHelper::addConnection() @n
 *      로즈 기기 DB 접속 @sa SqliteHelper::addConnectionRose() , 로즈 PC APP DB 접속 @sa SqliteHelper:addConnectionLocal()
 */
class SqliteHelper : public QObject
{
    Q_OBJECT
public:



    explicit SqliteHelper(QObject *parent = nullptr);
    ~SqliteHelper();

    QSqlError addConnectionLocal();
    QSqlError addConnectionRose();
    QSqlError addConnectionRoseOld();
    QSqlError addConnection(const QString &driver, const QString &dbName, const QString &name);

    bool exec(const QString& q);
    bool exec(const QString& q, QVariantList& data);
    void close();
    QString getActiveDb();
    QString getDBName_Rose(){return DB_NAME_ROSE;};
    QString getDBName_Rose_Old(){return DB_NAME_ROSE_OLD;};
    QString getDBName_Rose_Desktop(){return DB_NAME_ROSE_DESKTOP;};

private:
    QString activeDb;

};


#endif // SQLITEHELPER_H
