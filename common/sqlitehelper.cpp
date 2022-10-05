#include "sqlitehelper.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

/**
 * @brief The SqliteHelper class QtSql 헬퍼 클래스 @n
 *      공통 DB 접속(드라이버명,디비패스,디비명) @sa SqliteHelper::addConnection() @n
 *      로즈 기기 DB 접속 @sa SqliteHelper::addConnectionRose() , 로즈 PC APP DB 접속 @sa SqliteHelper:addConnectionLocal()
 * @param QObject
 */
SqliteHelper::SqliteHelper(QObject *parent)
    :QObject (parent)
{
    if (QSqlDatabase::drivers().isEmpty()){
        // nothing
    }
}

SqliteHelper::~SqliteHelper(){
}

QSqlError SqliteHelper::addConnectionLocal(){

    const QString driver = QString("QSQLITE");
    const QString dbPath = QString("%1%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg("/rose_desktop.db");
    const QString dbName = QString(DB_NAME_ROSE_DESKTOP);

    return addConnection(driver, dbPath, dbName);
}

QSqlError SqliteHelper::addConnectionRose(){
    const QString driver = QString("QSQLITE");
    //const QString dbPath = QString("%1%2").arg(qApp->applicationDirPath()).arg("/config/rose.db");
    const QString dbPath = QString("%1%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg("/rose.db");
    const QString dbName = QString(DB_NAME_ROSE);

    return addConnection(driver, dbPath, dbName);
}

QSqlError SqliteHelper::addConnectionRoseOld(){
    const QString driver = QString("QSQLITE");
    //const QString dbPath = QString("%1%2").arg(qApp->applicationDirPath()).arg("/config/tmp_rose.db");
    const QString dbPath = QString("%1%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg("/tmp_rose.db");
    const QString dbName = QString(DB_NAME_ROSE_OLD);

    return addConnection(driver, dbPath, dbName);
}

QSqlError SqliteHelper::addConnection(const QString &driver, const QString &dbName, const QString &name){

    QSqlError err;
    QSqlDatabase db;
    if(QSqlDatabase::contains(name)){
        db = QSqlDatabase::database(name);
    }else{
        db = QSqlDatabase::addDatabase(driver, QString("%1").arg(name));
    }
    db.setDatabaseName(dbName);

    if(!db.open()){
        err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QString("%1").arg(name));
    }

    activeDb = name;

    return err;
}

bool SqliteHelper::exec(const QString &q){
    QSqlQuery query(QSqlDatabase::database(activeDb));
    query.prepare(q);
    return query.exec();
}

bool SqliteHelper::exec(const QString &q , QVariantList& data){
    QSqlQuery query(QSqlDatabase::database(activeDb));
    query.prepare(q);
    bool flagOk = query.exec();

    while(query.next()){
        QSqlRecord record = query.record();
        int count = record.count();
        QVariantMap map;
        for(int i=0; i < count; i++){
            map[record.field(i).name()] = record.value(i);
        }
        data.push_back(map);
    }

    return flagOk;
}

void SqliteHelper::close(){

    QSqlDatabase db = QSqlDatabase::database(activeDb);
    db.close();
}

QString SqliteHelper::getActiveDb(){
    return activeDb;
}
