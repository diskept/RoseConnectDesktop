#include "mainwindow.h"
#include "common/rosesettings.h"

#include <QApplication>
#include <QTranslator>
#include <QtWebEngine/qtwebengineglobal.h>
#include "versionupdate/Window.h"//cheon210812-iso
//#include "versionupdate/Updater.h"//cheon210812-iso
//#include "versionupdate/QSimpleUpdater.h"//cheon210812-iso

#include <QtNetwork>
#include <QNetworkAccessManager>

#include <iostream>
#include <QTextStream>
#include <QStandardPaths>
#include <common/sqlitehelper.h>
#include "common/global.h"


//#define MY_LANG
/**
 * @brief setQSettings : QSettings 초기 세팅
 */
void setQSettings(){

    // set core value
    QCoreApplication::setOrganizationName(rosesettings.ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(rosesettings.ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(rosesettings.APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(rosesettings.APPLICATION_VERSION);

    // Youtube Crawling

    QCoreApplication::setOrganizationName("YoutubeGetData");
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QtWebEngine::initialize();
}


const QString QUERY_SEL = "SELECT * FROM Language";


void LogToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString logFilePath = qApp->applicationDirPath()+"/debug/logfile01.txt";
    //qDebug() << "logFilePath=" << logFilePath;
    QFile file(logFilePath);
    if(!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);

    QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                        //context.file, context.line, context.function
    switch (type) {
    case QtDebugMsg:
        out << "[Debug]" << curtime << msg << "\n";
        break;
    case QtInfoMsg:
        out << "[Info]" << curtime << msg << "\n";
        break;
    case QtWarningMsg:
        out << "[Warning]" << curtime << msg << "\n";
        break;
    case QtCriticalMsg:
        out << "[Critical]" << curtime << msg << context.function << context.line << "\n";
        break;
    case QtFatalMsg:
        out << "[Fatal]" << curtime << msg << "\n";
        break;
    }
    file.close();
}

int main(int argc, char *argv[])
{
    global.currentExitCode = 0;//cheon210812-iso


    QString ver = "4.2.293";//c220831_2  이하 6 line
    if(ver.split(".").last().toInt() > 9){
        //qInstallMessageHandler(LogToFile);
    }
    setQSettings();
    rosesettings.RoseSettings_setVersion(ver);//c220718
    rosesettings.RoseSettings_setBuildDate("22/10/14");//c220718

    /*QNetworkConfigurationManager manager;

    const bool canStartIAP = (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired);

    if(canStartIAP) {
        qDebug() << "NetWork enable";
    }
    else{
        qDebug() << "NetWork disable";
    }*/

        print_debug();

        do {

            global.lang = 0;
            global.currentExitCode = 0;

            QApplication roseApp(argc, argv);


            //-------------------------//c220619-search
            SqliteHelper *sqliteHelper1 = new SqliteHelper();
            QSqlError err1 = sqliteHelper1->addConnectionLocal();

            if(err1.type() == QSqlError::NoError){

                QString strQuery_create = "CREATE TABLE IF NOT EXISTS searchPreList (";
                strQuery_create += "idx integer PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, search_text VARCHAR(50))";
                sqliteHelper1->exec(strQuery_create);
            }
            sqliteHelper1->close();
            delete sqliteHelper1;

            //-----------------//c220619-search


            //-------------------------//c220619-dtPlayList_table
            SqliteHelper *sqliteHelper_dtPlayList = new SqliteHelper();
            QSqlError err_dtPlayList = sqliteHelper_dtPlayList->addConnectionLocal();

            if(err1.type() == QSqlError::NoError){

                QString strQuery_create = "CREATE TABLE IF NOT EXISTS dtPlayList_table (";
                strQuery_create += "idx integer PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, row integer, filePath VARCHAR(50))";
                sqliteHelper_dtPlayList->exec(strQuery_create);
            }
            sqliteHelper_dtPlayList->close();
            delete sqliteHelper_dtPlayList;

            //-----------------//c220619-search

            SqliteHelper *sqliteHelper = new SqliteHelper();
            //QSqlError err = sqlite->addConnectionRose();      // Rose에서 받는 DB 파일 수정 변경 불가
            QSqlError err = sqliteHelper->addConnectionLocal();

            //----------------------------dtPlayList_table-delete//c220704
            SqliteHelper *sqliteHelperDel = new SqliteHelper();
            QSqlError err2 = sqliteHelperDel->addConnectionLocal();
            QVariantList datad = QVariantList();
            if(err2.type() == QSqlError::NoError){
                datad = QVariantList();
                QString strQuery_delete = "DELETE ";
                strQuery_delete += " FROM ";
                strQuery_delete += " dtPlayList_table";
                sqliteHelperDel->exec(strQuery_delete, datad);
                // print_debug();
                qDebug() << "datad->size()=" << datad.size();
            }
            sqliteHelperDel->close();
            delete sqliteHelperDel;
            //------------------------------------------------------------

            // SELECT 처리
            int idx = 0;
            int sel = 0;
            QVariantList *list = new QVariantList();
        sqliteHelper->exec(QUERY_SEL, *list);
        if(err.type() == QSqlError::NoError){
            foreach(QVariant val, *list){
                QMap<QString, QVariant> map = val.toMap();
                idx = map["idx"].toInt();
                sel = map["lang"].toInt();
                break;
            }
        }
        global.lang = sel;
        sqliteHelper->close();
        delete sqliteHelper;


        //-------------translator routinue------------------------------
        QTranslator trans;

        QString langFilePath = qApp->applicationDirPath()+"/translations/myapp_ko.qm";
        std::string ascii_str = langFilePath.toUtf8().constData();


        //if(QLocale::system().language() == QLocale::Korean){
        qDebug()<< "langFilePath : " << langFilePath;
        if(global.lang == 1){

            qDebug() << "korea lang-----\n";

            if(trans.load(langFilePath)){
                roseApp.installTranslator(&trans);
                qDebug() << "*-------------success--main_func()-------lang/myapp_ko.qm-------------\n";
            }else{
                //qDebug() << "---------------fail-----main_func()-------lang/myapp_ko.qm-------------\n";
            }
        }else {
            global.lang = 0;
            qDebug() << "English lang-----\n";
        }

        MainWindow rose_window;
        rose_window.setWindowFlags(Qt::WindowCloseButtonHint |Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint|Qt::CustomizeWindowHint);
        //rose_window.setWindowFlags(Qt::WindowCloseButtonHint |Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint); //Qt::CustomizeWindowHint);
        rose_window.setInitWindowSize();

        rose_window.show();

        HttpServer httpServer;//cheon210617-http
        httpServer.start();//cheon210617-http



        global.currentExitCode = roseApp.exec();//cheon210812-iso
        //delete roseApp;

    } while(global.currentExitCode == MainWindow::EXIT_CODE_REBOOT);//cheon210812-iso

    return global.currentExitCode;//cheon210812-iso
}


