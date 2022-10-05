#include "mainwindow.h"
#include "common/rosesettings.h"

#include <QApplication>
#include <QtWebEngine/qtwebengineglobal.h>
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


int main(int argc, char *argv[])
{
    setQSettings();
    QApplication a(argc, argv);

    MainWindow w;
    w.setInitWindowSize();
    w.show();
    return a.exec();
}


