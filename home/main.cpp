#include "mainwindow.h"
#include "common/rosesettings.h"
#include "common/httpServer.h"

#include <QApplication>
#include <QTranslator>
#include <QtWebEngine/qtwebengineglobal.h>
#include "versionupdate/Window.h"//cheon210812-iso
//#include "versionupdate/Updater.h"//cheon210812-iso
//#include "versionupdate/QSimpleUpdater.h"//cheon210812-iso

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

#include <iostream>
#include <QTextStream>
#include <QStandardPaths>
#include <common/sqlitehelper.h>
#include "common/global.h"
const QString QUERY_SEL = "SELECT * FROM Language";


int main(int argc, char *argv[])
{
    global.currentExitCode = 0;//cheon210812-iso


    print_debug();
    do {
        global.lang = 0;
        global.currentExitCode = 0;
        setQSettings();
        QApplication roseApp(argc, argv);


        rosesettings.RoseSettings_setVersion("4.1.8.4");//c220411


        HttpServer httpServer;//cheon210617-http
        httpServer.start();//cheon210617-http

        SqliteHelper *sqliteHelper = new SqliteHelper();
        //QSqlError err = sqlite->addConnectionRose();      // Rose에서 받는 DB 파일 수정 변경 불가
        QSqlError err = sqliteHelper->addConnectionLocal();


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


        MainWindow rose_window;
        rose_window.setInitWindowSize();
        rose_window.show();


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


        global.currentExitCode = roseApp.exec();//cheon210812-iso

    } while(global.currentExitCode == MainWindow::EXIT_CODE_REBOOT);//cheon210812-iso
    return global.currentExitCode;//cheon210812-iso
}


