/*
 * Copyright (c) 2014-2021 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QJsonValue>
#include <QJsonObject>
//#include <QMessageBox>//c230315_3
#include <QApplication>
#include <QJsonDocument>
#include <QDesktopServices>
#include "../mainwindow.h"
#include "Updater.h"
#include "Downloader.h"

#include "common/rosesettings.h"
#include "widget/toastmsg.h"
#include <QDialog>
#include "common/global.h"
#include <QTimer>
#include <QThread>
#include <QTextEdit>
#include <QSettings>
#include <QFileDialog>



#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";

#if defined(Q_OS_WINDOWS)
static const QString DEFS_URL = "http://app.roseaudio.kr/pc/rose_updates.json";
//static const QString DEFS_URL = "http://192.168.1.3:280/rose_updates.json";
#endif
#if defined(Q_OS_MAC)
static const QString DEFS_URL = "http://app.roseaudio.kr/pc/rose_updates.json";
//static const QString DEFS_URL = "http://192.168.1.3:280/rose_updates.json";
#endif


Updater::Updater()
{
    m_url = "";
    m_openUrl = "";//setNotifyOnUpdate
    m_changelog_kor = "";
    m_changelog_eng = "";
    m_downloadUrl = "";
    m_latestVersion = "";
    m_customAppcast = false;
    m_notifyOnUpdate = true;
    m_notifyOnFinish = true;
    m_updateAvailable = false;
    m_downloaderEnabled = true;
    m_moduleName = qApp->applicationName();
    m_moduleVersion = qApp->applicationVersion();
    m_mandatoryUpdate = false;
    qDebug() << "Updater::openUrl()----4444444444444444444444444444444444444" ;
    m_downloader = new Downloader();
    m_manager = new QNetworkAccessManager();

#if defined Q_OS_WIN
    m_platform = "windows";
#elif defined Q_OS_MAC
    m_platform = "osx";
#elif defined Q_OS_LINUX
    m_platform = "linux";
#elif defined Q_OS_ANDROID
    m_platform = "android";
#elif defined Q_OS_IOS
    m_platform = "ios";
#endif
    m_updater_tmp = QSimpleUpdater::getInstance();

    linker = Linker::getInstance();
    emit linker->signal_checkQueue(18, "");
    setUserAgentString(QString("%1/%2 (Qt; QSimpleUpdater)").arg(qApp->applicationName(), qApp->applicationVersion()));
    //qDebug() << "qApp->applicationVersion()=" << qApp->applicationVersion();//cheon210721-update
    connect(m_downloader, SIGNAL(downloadFinished(QString, QString)), this, SLOT(slot_downloadFinished(QString, QString)));//c230410
    connect(m_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(onReply(QNetworkReply *)));
}


Updater::~Updater()
{
    delete m_downloader;
}


void  Updater::slot_downloadFinished(QString url, QString filename){//c230410
    //downloadFinished(m_url, m_downloadDir.filePath(m_fileName));
    print_debug();
    qDebug() << "m_url=" << m_url;
    qDebug() << "filename=" << filename;
    finishFlag = false;
}


/**
 * Returns the URL of the update definitions file
 */
QString Updater::url() const
{
    return m_url;
}


/**
 * Returns the URL that the update definitions file wants us to open in
 * a web browser.
 *
 * \warning You should call \c checkForUpdates() before using this functio
 */
QString Updater::openUrl() const
{

    return m_openUrl;
}


/**
 * Returns the changelog defined by the update definitions file.
 * \warning You should call \c checkForUpdates() before using this function
 */
QString Updater::changelog() const
{
    if(global.lang == 0){
        return m_changelog_eng;
    }else
        return m_changelog_kor;
}


/**
 * Returns the name of the module (if defined)
 */
QString Updater::moduleName() const
{
    return m_moduleName;
}


/**
 * Returns the platform key (be it system-set or user-set).
 * If you do not define a platform key, the system will assign the following
 * platform key:
 *    - On iOS: \c ios
 *    - On Mac OSX: \c osx
 *    - On Android: \c android
 *    - On GNU/Linux: \c linux
 *    - On Microsoft Windows: \c windows
 */
QString Updater::platformKey() const
{
    return m_platform;
}


/**
 * Returns the download URL defined by the update definitions file.
 * \warning You should call \c checkForUpdates() before using this function
 */
QString Updater::downloadUrl() const
{
    return m_downloadUrl;
}


/**
 * Returns the latest version defined by the update definitions file.
 * \warning You should call \c checkForUpdates() before using this function
 */
QString Updater::latestVersion() const
{
    return m_latestVersion;
}


/**
 * Returns the user-agent header used by the client when communicating
 * with the server through HTTP
 */
QString Updater::userAgentString() const
{
    return m_userAgentString;
}


/**
 * Returns the "local" version of the installed module
 */
QString Updater::moduleVersion() const
{
    return m_moduleVersion;
}


/**
 * Returns \c true if the updater should NOT interpret the downloaded appcast.
 * This is useful if you need to store more variables (or information) in the
 * JSON file or use another appcast format (e.g. XML)
 */
bool Updater::customAppcast() const
{
    return m_customAppcast;
}


/**
 * Returns \c true if the updater should notify the user when an update is
 * available.
 */
bool Updater::notifyOnUpdate() const
{
    return m_notifyOnUpdate;
}


/**
 * Returns \c true if the updater should notify the user when it finishes
 * checking for updates.
 *
 * \note If set to \c true, the \c Updater will notify the user even when there
 *       are no updates available (by congratulating him/her about being smart)
 */
bool Updater::notifyOnFinish() const
{
    print_debug();
    qDebug() << "notifyOnFinish--m_notifyOnFinish" << m_notifyOnFinish;
    return m_notifyOnFinish;
}


/**
 * Returns \c true if there the current update is mandatory.
 * \warning You should call \c checkForUpdates() before using this function
 */
bool Updater::mandatoryUpdate() const
{
    return m_mandatoryUpdate;
}


/**
 * Returns \c true if there is an update available.
 * \warning You should call \c checkForUpdates() before using this function
 */
bool Updater::updateAvailable() const
{
    return m_updateAvailable;
}


/**
 * Returns \c true if the integrated downloader is enabled.
 * \note If set to \c true, the \c Updater will open the downloader dialog if
 *       the user agrees to download the update.
 */
bool Updater::downloaderEnabled() const
{
    return m_downloaderEnabled;
}


/**
 * Returns \c true if the updater shall not intervene when the download has
 * finished (you can use the \c QSimpleUpdater signals to know when the
 * download is completed).
 */
bool Updater::useCustomInstallProcedures() const
{
    return m_downloader->useCustomInstallProcedures();
}


void Updater::checkForUpdates_tmp()
{
    //print_debug();
    /* Get settings from the UI */
    // QString version = m_ui->installedVersion->text();//cheon210721-update
    QString version = rosesettings.APPLICATION_VERSION;//cheon210721-update
    qDebug() << "Updater::checkForUpdates_tmp---version = " << version;//ceon210831-network
    bool customAppcast = false;
    bool downloaderEnabled = true ;
    bool notifyOnFinish = false;
    bool notifyOnUpdate = true;
    bool mandatoryUpdate = false;
    /*
    bool customAppcast = m_ui->customAppcast->isChecked();
    bool downloaderEnabled = m_ui->enableDownloader->isChecked();
    bool notifyOnFinish = m_ui->showAllNotifcations->isChecked();
    bool notifyOnUpdate = m_ui->showUpdateNotifications->isChecked();
    bool mandatoryUpdate = m_ui->mandatoryUpdate->isChecked();
*/
    /* Apply the settings */
    m_updater_tmp->setModuleVersion (DEFS_URL, version);
    m_updater_tmp->setNotifyOnFinish (DEFS_URL, notifyOnFinish);
    m_updater_tmp->setNotifyOnUpdate (DEFS_URL, notifyOnUpdate);
    m_updater_tmp->setUseCustomAppcast (DEFS_URL, customAppcast);
    m_updater_tmp->setDownloaderEnabled (DEFS_URL, downloaderEnabled);
    m_updater_tmp->setMandatoryUpdate (DEFS_URL, mandatoryUpdate);

    /* Check for updates */
    m_updater_tmp->checkForUpdates (DEFS_URL);
    //print_debug();
}


/**
 * Downloads and interpets the update definitions file referenced by the
 * \c url() function.
 */
void Updater::checkForUpdates()//c230306_1
{
    print_debug();
    QNetworkRequest request(url());
    qDebug() << " Updater::checkForUpdates--User-Agent: " << userAgentString().toUtf8();
    if (!userAgentString().isEmpty()){
        request.setRawHeader("User-Agent", userAgentString().toUtf8());
        m_manager->get(request);
        print_debug();
    }else{
        //m_manager->get(request);
        emit linker->signal_checkQueue(19, "");
        print_debug();
    }

    //qDebug() << " Updater::checkForUpdates--User-Agent: " << userAgentString().toUtf8();

}


/**
 * Changes the \c url in which the \c Updater can find the update definitions
 * file.
 */
void Updater::setUrl(const QString &url)
{
    print_debug();
    m_url = url;
}


/**
 * Changes the module \a name.
 * \note The module name is used on the user prompts. If the module name is
 *       empty, then the prompts will show the name of the application.
 */
void Updater::setModuleName(const QString &name)
{
    print_debug();
    m_moduleName = name;
}


/**
 * If \a notify is set to \c true, then the \c Updater will notify the user
 * when an update is available.
 */
void Updater::setNotifyOnUpdate(const bool notify)
{
    print_debug();
    qDebug() << "setNotifyOnUpdate--notify" << notify;
    m_notifyOnUpdate = notify;
}


/**
 * If \a notify is set to \c true, then the \c Updater will notify the user
 * when it has finished interpreting the update definitions file.
 */
void Updater::setNotifyOnFinish(const bool notify)
{
    print_debug();
    qDebug() << "setNotifyOnFinish--notify" << notify;
    m_notifyOnFinish = notify;
}


/**
 * Changes the user agent string used to identify the client application
 * from the server in a HTTP session.
 *
 * By default, the user agent will co
 */
void Updater::setUserAgentString(const QString &agent)
{
    //print_debug();
    m_userAgentString = agent;
    m_downloader->setUserAgentString(agent);
}


/**
 * Changes the module \a version
 * \note The module version is used to compare the local and remote versions.
 *       If the \a version parameter is empty, then the \c Updater will use the
 *       application version (referenced by \c qApp)
 */
void Updater::setModuleVersion(const QString &version)
{
    print_debug();
    qDebug() << " Updater::setModuleVersion--version: " << version;
    m_moduleVersion = version;
}


/**
 * If the \a enabled parameter is set to \c true, the \c Updater will open the
 * integrated downloader if the user agrees to install the update (if any)
 */
void Updater::setDownloaderEnabled(const bool enabled)
{
    //print_debug();
    qDebug() << "Updater::setDownloaderEnabled--m_downloaderEnabled = " << m_downloaderEnabled;
    m_downloaderEnabled = enabled;
}


/**
 * Changes the platform key.
 * If the platform key is empty, then the system will use the following keys:
 *    - On iOS: \c ios
 *    - On Mac OSX: \c osx
 *    - On Android: \c android
 *    - On GNU/Linux: \c linux
 *    - On Microsoft Windows: \c windows
 */
void Updater::setPlatformKey(const QString &platformKey)
{
    //print_debug();
    m_platform = platformKey;
}


/**
 * If the \a customAppcast parameter is set to \c true, then the \c Updater
 * will not try to read the network reply from the server, instead, it will
 * emit the \c appcastDownloaded() signal, which allows the application to
 * read and interpret the appcast file by itself
 */
void Updater::setUseCustomAppcast(const bool customAppcast)
{
    //print_debug();
    m_customAppcast = customAppcast;
}


/**
 * If the \a custom parameter is set to \c true, the \c Updater will not try
 * to open the downloaded file. Use the signals fired by the \c QSimpleUpdater
 * to install the update from the downloaded file by yourself.
 */
void Updater::setUseCustomInstallProcedures(const bool custom)
{
    //print_debug();
    m_downloader->setUseCustomInstallProcedures(custom);
}


/**
 * If the \a mandatory_update is set to \c true, the \c Updater has to download and install the
 * update. If the user cancels or exits, the application will close
 */
void Updater::setMandatoryUpdate(const bool mandatory_update)
{
    //print_debug();
    qDebug() << "Updater::Updater::setMandatoryUpdate--m_mandatoryUpdate = " << m_mandatoryUpdate;
    m_mandatoryUpdate = mandatory_update;
}


/**
 * Called when the download of the update definitions file is finished.
 */
void Updater::onReply(QNetworkReply *reply)
{
    print_debug();
    // ContentLoadingwaitingMsgHide();
    /* Check if we need to redirect */
    QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!redirect.isEmpty())
    {
        print_debug();
        qDebug() << "redirect.toString() = " << redirect.toString();
        setUrl(redirect.toString());
        checkForUpdates();
        return;
    }

    /* There was a network error */
    if (reply->error() != QNetworkReply::NoError)
    {
        print_debug();
        global.updateCheckFlag = true;
        setUpdateAvailable(false);
        emit checkingFinished(url());
        emit linker->signal_checkQueue(19, "");
        //emit linker->signal_device_connect_check();
        //ContentLoadingwaitingMsgShow(tr("   Rose_Connect Notice Message\n\nFailed to connect to server.\nOr, the update server is not ready yet.\n"), 1);

        return;
    }
    global.updateCheckFlag = false;
    qDebug() << "reply->error()=" << reply->error();
    /* The application wants to interpret the appcast by itself */
    if (customAppcast())
    {
        print_debug();
        emit appcastDownloaded(url(), reply->readAll());
        emit checkingFinished(url());
        return;
    }

    /* Try to create a JSON document from downloaded data */
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

    /* JSON is invalid */
    if (document.isNull())
    {
        print_debug();
        setUpdateAvailable(false);
        emit checkingFinished(url());
        //print_debug();
        return;
    }

    /* Get the platform information */
    QJsonObject updates = document.object().value("updates").toObject();
    QJsonObject platform = updates.value(platformKey()).toObject();

    /* Get update information */
    m_openUrl = platform.value("open-url").toString();
    m_changelog_eng = platform.value("changelog_eng").toString();
    m_changelog_kor = platform.value("changelog_kor").toString();
    m_downloadUrl = platform.value("download-url").toString();
    m_latestVersion = platform.value("latest-version").toString();
    if (platform.contains("mandatory-update"))
        m_mandatoryUpdate = platform.value("mandatory-update").toBool();
    print_debug();
    /* Compare latest and current version */
    qDebug() << "Updater::onReply---version_compare : " << compare(latestVersion(), moduleVersion());//cheon210721-update
    qDebug() << "Updater::onReply---latestVersion() : " << latestVersion();//cheon210721-update
    setUpdateAvailable(compare(latestVersion(), moduleVersion()));
    emit checkingFinished(url());
}


/**
 * Prompts the user based on the value of the \a available parameter and the
 * settings of this instance of the \c Updater class.
 */
void Updater::setUpdateAvailable(const bool available)
{
    //print_debug();
    m_updateAvailable = available;
    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
    int latestHeight = settings->value(rosesettings.SETTINGS_H, 800).toInt();
    int left = 0;
    int top = 0;


    QPushButton *btn_PlayPosition = new QPushButton(tr("New feature manual folder"));//c220718
    btn_PlayPosition->setObjectName("btn_PlayPosition");
    btn_PlayPosition->setCursor(Qt::PointingHandCursor);
    btn_PlayPosition->setFixedSize(280,30);            //45, 50
    //btn_PlayPosition->setStyleSheet("#btn_PlayPosition { font-size:16px; color:#ececec;  background-color:transparent;  border:2px solid #CCCCCC;border-radius:13px;  } ");
    btn_PlayPosition->setStyleSheet("QPushButton {font-size:16px; color:#ececec;  background-color:transparent;  border:2px solid #CCCCCC;border-radius:13px;} QPushButton:hover {background-color: #505050; color: white;} QPushButton:pressed{background-color: #F79862;} QPushButton:checked{background-color: #F79862;border:none;} QToolTip{ color: #404040; }");


    //QMessageBox box;//c230315_3
    //box.setWindowFlags(Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint);
    box.setWindowFlags(Qt::FramelessWindowHint );
    QPixmap tmp_pixmap(":images/def_mus_60.png");
    //tmp_pixmap = tmp_pixmap.scaled(40,40, Qt::AspectRatio, Qt::SmoothTransformation);
    box.setIconPixmap(tmp_pixmap);
//hl_top->addWidget(btn_PlayPosition);
    //box.setBaseSize(QSize(600, 220));
    //box.setStyleSheet("QPushButton {background-color: #111111;}");
    box.setStyleSheet("color:#111111;font-size:16px;background-color:#dddddd;");
    box.setTextFormat(Qt::RichText);
    //box.setIcon(QMessageBox::Information);

    left = (global.left_mainwindow + global.width_mainwindow) / 2;//c221007_1
    top = (global.top_mainwindow+global.height_mainwindow) / 2;//c221007_1
    box.setGeometry(left, top, 0, 0);//c221007_1
    /*//c221007_1
    left = (latestWidth / 2) - (box.sizeHint().width() / 2);
    top = (latestHeight/ 2) - (box.sizeHint().height() / 2);
    box.setGeometry(200, 200, 0, 0);
    */


    qDebug() << "Updater::setUpdateAvailable---m_updateAvailable = " << m_updateAvailable;
    if (updateAvailable() && (notifyOnUpdate() || notifyOnFinish()))
    {

        if (m_mandatoryUpdate)
        {
            box.setInformativeText(tr(" Rose_Connect... notice!\n\nCurrently version %1 --> Released version %2\n").arg(m_moduleVersion).arg(latestVersion())+tr("Would you like to download the update now? This is a mandatory update, exiting now will close the application"));

        }else {

            box.setInformativeText(tr(" Rose_Connect... notice!\n\nCurrently ver %1 --> Release ver %2\n").arg(m_moduleVersion).arg(latestVersion())
                                   +tr("Would you like to download the update now?\n(check the updated list)"));
            if(global.lang == 0){
                //box.setInformativeText(tr("Would you like to download the update now?\n")+m_changelog_eng);
                //box.setInformativeText(tr("Would you like to download the update now?\n"));
                box.setDetailedText(m_changelog_eng);
            }else if(global.lang == 1){
                //box.setInformativeText(tr("Would you like to download the update now?\n")+m_changelog_kor);

                box.setDetailedText(m_changelog_kor);
            }


        }


        box.setTextInteractionFlags(Qt::TextSelectableByMouse);

        box.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        box.setDefaultButton(QMessageBox::Yes);

        QSpacerItem* horizontalSpacer = new QSpacerItem(550, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)box.layout();
        //layout->addWidget(btn_PlayPosition, 5, 2);
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        foreach (QAbstractButton *button, box.buttons())
        {
            if (box.buttonRole(button) == QMessageBox::ActionRole)
            {
                button->click();
                break;
            }
        }
        QList<QPushButton*> pushbuuttons = box.findChildren<QPushButton*>();
        for(int i = 0; i < pushbuuttons.size(); i++){
            pushbuuttons[i]->setCursor(Qt::PointingHandCursor);
            pushbuuttons[i]->setStyleSheet("QPushButton{border:2px solid #0076a3; border-radius:5px; color:#fff; background-color:#0898d8; padding:2px 25px;}QPushButton:hover{color:#B18658;}");

        }
        pushbuuttons[1]->setStyleSheet("QPushButton{border:2px solid #0076a3; border-radius:5px; color:#fff; background-color:#777777; padding:2px 25px;}QPushButton:hover{color:#B18658;}");

        QList<QLabel*> textLabels = box.findChildren<QLabel*>();
        qDebug() << "textLabels.size()= " << textLabels.size();
        textLabels[2]->setStyleSheet("font-size:18px;");

        QList<QTextEdit*> textBoxes = box.findChildren<QTextEdit*>();
        int linecnt = textBoxes[0]->document()->blockCount();
        qDebug() << "textBoxes.size()= " << textBoxes.size();
        qDebug() << "linecnt= " << linecnt;
        if(textBoxes.size()){
            textBoxes[0]->setFixedSize(570, 300);
            textBoxes[0]->document()->setDocumentMargin(10);
            textBoxes[0]->setStyleSheet("color:#333333;;font-size:12px;background-color:#bbbbbb;");

        }
        //

        connect(btn_PlayPosition, SIGNAL(clicked()), this, SLOT(slot_DescriptionShow()));
        //pushbuuttons[0]->hide();
        if (box.exec() == QMessageBox::Yes)
        {
            print_debug();
            if (!openUrl().isEmpty()){
                print_debug();
                QDesktopServices::openUrl(QUrl(openUrl()));

            } else if (downloaderEnabled())
            {
                print_debug();
                //qApp->exit( MainWindow::EXIT_CODE_QUIT );
                m_downloader->setUrlId(url());
                m_downloader->setFileName(downloadUrl().split("/").last());
                m_downloader->setMandatoryUpdate(m_mandatoryUpdate);
                m_downloader->startDownload(QUrl(downloadUrl()));
            }

            else{
                qDebug() << "Updater::openUrl()----4444444444444444444444444444444444444" ;
                QDesktopServices::openUrl(QUrl(downloadUrl()));
            }
        }
        else
        {
            // print_debug();
            // MainWindow rose_window;
            // rose_window.setInitWindowSize();
            // rose_window.show();
            if (m_mandatoryUpdate)
            {
                //QApplication::quit();

            }
        }

    }

    else if (notifyOnFinish())
    {
        print_debug();
        box.setStyleSheet("font-size:12px;");
        box.setTextInteractionFlags(Qt::TextSelectableByMouse);
        box.setStandardButtons(QMessageBox::Close);
        box.setInformativeText(tr("No updates are available for the moment"));
        box.setText("<h3>"+ tr("Congratulations! You are running the latest version of %1").arg(moduleName())+ "</h3>");





        box.exec();


    }else{
        //print_debug();
        //MainWindow rose_window;
        //rose_window.setInitWindowSize();
        //rose_window.show();
        //ContentLoadingwaitingMsgHide();
        //ContentLoadingwaitingMsgShow(tr("   Rose_Connect Notice Message\n\nThere are no recent version updates.\n"), 1);

        if (m_mandatoryUpdate)
        {
           // QApplication::quit();//c220715

        }
    }

}


void Updater::slot_DescriptionShow(){
    print_debug();
    //QString shareFilePath = qApp->applicationDirPath()+"/manual/descriptionShare.mp4";
    QString shareFilePath = qApp->applicationDirPath()+"/manual";
    QDesktopServices::openUrl( QUrl::fromLocalFile(shareFilePath) );
    qDebug() << "shareFilePath=" << shareFilePath;
    /*
    QString dir = QFileDialog::getExistingDirectory(nullptr, tr("Open Directory"),
                                                 shareFilePath,
                                                  QFileDialog::DontResolveSymlinks);*/
    /*
    qDebug() << "shareFilePath=" << shareFilePath;
    //shareFilePath.replace("/", "\\");
    qDebug() << "shareFilePath=" << shareFilePath;
    QMediaPlayer *player = new QMediaPlayer;
        QVideoWidget *vw = new QVideoWidget;
        player->setVideoOutput(vw);
        player->setMedia(QUrl::fromLocalFile("file://"+shareFilePath));
        player->setVolume(50);
        vw->show();
        vw->raise();
        player->play();
        qDebug() << "mediaStatus: " << player->mediaStatus() << "error: " << player->error();
        */

}


/**
 * Compares the two version strings (\a x and \a y).
 *     - If \a x is greater than \y, this function returns \c true.
 *     - If \a y is greater than \x, this function returns \c false.
 *     - If both versions are the same, this function returns \c false.
 */
bool Updater::compare(const QString &x, const QString &y)
{
    //print_debug();
    QStringList versionsX = x.split(".");
    QStringList versionsY = y.split(".");

    int count = qMin(versionsX.count(), versionsY.count());

    for (int i = 0; i < count; ++i)
    {
        int a = QString(versionsX.at(i)).toInt();
        int b = QString(versionsY.at(i)).toInt();

        if (a > b)
            return true;

        else if (b > a)
            return false;
    }

    return versionsY.count() < versionsX.count();
}


void Updater::ContentLoadingwaitingMsgShow(QString msg, int flag)//cheontidal
{
    Q_UNUSED(msg);
    Q_UNUSED(flag);

    /*
    QLabel *lb_msg = new QLabel();
    lb_msg->setText(msg);
    lb_msg->setAlignment(Qt::AlignCenter);

    QHBoxLayout *hl_msgBox = new QHBoxLayout();
    hl_msgBox->setContentsMargins(0,0,0,0);
    hl_msgBox->setSpacing(0);
    hl_msgBox->setContentsMargins(35,20,35,15);
    hl_msgBox->addStretch(1);
    hl_msgBox->addWidget(lb_msg);
    hl_msgBox->addStretch(1);
    QWidget *widget_msgBox = new QWidget();
    widget_msgBox->setStyleSheet("background-color:#B18658;color:#FFFFFF;font-size:20px;border-radius:5px;");
    widget_msgBox->setLayout(hl_msgBox);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_msgBox, 0, Qt::AlignCenter);

    global.dialog_wait = new QDialog() ;
    global.dialog_wait->setModal(true);
    global.dialog_wait->setWindowFlags( Qt::Tool | Qt::FramelessWindowHint );
    global.dialog_wait->setAttribute(Qt::WA_TranslucentBackground);

    global.dialog_wait->setStyleSheet("background-color:#55FF0000;border-radius:20px;");

    global.dialog_wait->setFixedSize(800,150);
    global.dialog_wait->setLayout(vl_total);
    global.dialog_wait->show();
  if(flag == 1){
    global.dialog_wait->raise();
    global.dialog_wait->activateWindow();
    global.dialog_wait->move(global.dialog_wait->pos().x(), (global.height_mainwindow - global.dialog_wait->height() - 100));
     QTimer::singleShot(3000, global.dialog_wait, SLOT(hide()));
  }

*/

}


void Updater::ContentLoadingwaitingMsgHide()//cheontidal
{

    //    global.dialog_wait->hide(); //cheontidal
}
