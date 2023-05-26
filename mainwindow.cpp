#include "mainwindow.h"

#include "common/filedownloader.h"
#include "common/global.h"
#include "common/rosesettings.h"
#include "common/sqlitehelper.h"

#include "widget/framerecentalbum.h"

#include <QSettings>
#include <QApplication>
#include <QStackedWidget>
#include <QFontDatabase>
#include <QStorageInfo>
#include <QDebug>
#include <QMouseEvent>
#include <QStandardPaths>
#include <QScreen>


const int MINSIZE_W = 1024;
const int MINSIZE_H = 800;


/**
 * @brief MainWindow::MainWindow : 메인윈도우
 * @param parent QWidget
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    this->setInit();
    this->setUIControl();
    //QGuiApplication::focusWindow();//c220724
}


/**
 * @brief MainWindow::~MainWindow : 소멸자
 */
MainWindow::~MainWindow()//c230302_1
{
    print_debug();
    this->linker->deleteLater();
    delete settings;
    qApp->exit();
    print_debug();
}


/**
 * @brief MainWindow::setInit : 초기 세팅
 */
void MainWindow::setInit(){

    this->linker = Linker::getInstance();

    this->setMinimumSize(MINSIZE_W, MINSIZE_H);
    //this->setWindowTitle("ROSE");
    this->setWindowTitle(rosesettings.APPLICATION_NAME + " " + rosesettings.APPLICATION_VERSION);

    // 폰트적용
    this->setFont();

    settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

    // 디렉토리 확인 및 생성
    checkDataDir();
}


/**
 * @brief MainWindow::setFont : Font 세팅
 * @note 나눔스퀘어
 */
void MainWindow::setFont(){//c220430

    // NotoSansCJKkr-Medium.ttf 폰트 등록
    int fontId = QFontDatabase::addApplicationFont(":/font/NotoSansCJKkr-Regular.ttf");
    //int fontId = QFontDatabase::addApplicationFont(":/font/NotoSansCJKkr-Medium.ttf");
    //int fontId = QFontDatabase::addApplicationFont(":/font/NotoSans-Medium.ttf");

    if (fontId != -1) { // 폰트 등록에 성공한 경우
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.empty()) { // 등록한 폰트가 하나 이상인 경우
            // QFont 객체 생성 후 등록한 폰트를 설정
            QFont font(fontFamilies.at(0));

            // QApplication의 기본 폰트로 설정
            QApplication::setFont(font);
        }
    }

    //qDebug() << QApplication::font();

    /*QString fontFilePath = qApp->applicationDirPath()+"/font/NanumSquareR.ttf";

    if(GSCommon::checkFileExists(fontFilePath)){
        //int id = QFontDatabase::addApplicationFont(qApp->applicationDirPath()+"/font/nanumsquare_ac_r.ttf");
        int id = QFontDatabase::addApplicationFont(qApp->applicationDirPath()+"/font/NanumSquareR.ttf");
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        //QFont font(family, 16, QFont::Bold, false);
        QFont font("Arial", 16, QFont::Light, false);
        QApplication::setFont(font);
    }
    else{
        QFont font("Arial", 16, QFont::Light, false);
        QApplication::setFont(font);
        // 배포파일 누락 :: 임시
        //QFont font("나눔스퀘어");
        //QApplication::setFont(font);
    }*/
}


/**
 * @brief MainWindow::checkDataDir : 디렉토리 확인 및 생성
 */
void MainWindow::checkDataDir(){

    QString tmp_dirPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(tmp_dirPath);
    if(!dir.exists()){
        dir.mkpath(tmp_dirPath);
    }
}


/**
 * @brief MainWindow::setInitWindowSize : 메인윈도우 사이즈 조절
 * @details 최초 실행시에는 WindowMaximized , 그 이후에는 사용자가 지정한 값을 이용해 메인윈도우 사이즈를 정한다.
 * 현재는 geometry를 사용하지 않고 width, height만 사용한다.
 */
void MainWindow::setInitWindowSize(){//c220408

    int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
    int latestHeight = settings->value(rosesettings.SETTINGS_H, MINSIZE_H).toInt();

    global.width_mainwindow = latestWidth;
    global.height_mainwindow = latestHeight;

    if(latestWidth > 0){
        this->resize(latestWidth, latestHeight);
    }
    else{
        this->setWindowState(Qt::WindowMaximized);
    }

    // TODO :: init position test... diskept 11/12/2020
//    int latestLeft = settings->value(rosesettings.SETTINGS_L, 0).toInt();
//    int latestTop = settings->value(rosesettings.SETTINGS_T, 0).toInt();

//    this->setGeometry(latestLeft, latestTop, latestWidth, latestHeight);
}


/**
 * @brief MainWindow::setUIControl : UI 세팅
 */
void MainWindow::setUIControl(){

    //settings->setValue(rosesettings.SETTINGS_NEED_TUTORIAL, true);   // TEST
    bool tmp_flagTutorial = settings->value(rosesettings.SETTINGS_NEED_TUTORIAL, QVariant(true)).toBool();
    if(tmp_flagTutorial){

        introWelcome = new IntroWelcome(this);
        introWifi = new IntroWifi(this);
        introRegister = new IntroRegister(this);
        stack = new QStackedWidget(this);
        stack->addWidget(introWelcome);
        stack->addWidget(introWifi);
        stack->addWidget(introRegister);
        this->setCentralWidget(stack);

        connect(introWelcome, SIGNAL(clickedNext()), this, SLOT(changedIntroPage()));
        connect(introWifi, SIGNAL(clickedNext()), this, SLOT(changedIntroPage()));
        connect(introRegister, SIGNAL(clickedNext()), this, SLOT(changedIntroPage()));
    }
    else{

        // deviceIP 세팅
        setDeviceInfo();

        int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
        int latestHeight = settings->value(rosesettings.SETTINGS_H, MINSIZE_H).toInt();

        global.width_mainwindow = latestWidth;
        global.height_mainwindow = latestHeight;

        // added by diskept
        int latestLeft = settings->value(rosesettings.SETTINGS_L).toInt();
        int latestTop = settings->value(rosesettings.SETTINGS_T).toInt();
//settings->ser
        global.left_mainwindow = latestLeft;
        global.top_mainwindow = latestTop;
     //   this->m
        this->move(global.left_mainwindow, global.top_mainwindow);//c230326
        this->homeMain = new HomeMain;
        this->setCentralWidget(homeMain);

        //QString ip = global.device.getDeviceIP();
        //this->homeMain->connectDevice(ip);

        QString ip = global.device.getDeviceIP();
        if(ip.size() > 0)
        {
            this->homeMain->connectDevice(ip);
        }
        /*//c221006_1
        QString ip2 = global.device.getDeviceIP();
        if(ip2.size() > 0)
        {
            this->homeMain->connectDevice(ip2);
        }*/
    }
}


/**
 * @brief MainWindow::setDeviceIP : 연결된 장치의 deviceIP값 세팅
 */
void MainWindow::setDeviceInfo(){

    if(global.device.getDeviceIP().isEmpty() || global.device.getDeviceRoseToken().isEmpty()){
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionLocal();

        if(err.type() == QSqlError::NoError){
            QVariantList data;
            sqlite->exec("SELECT * FROM Device", data);
            global.device.setData(data);
        }

        sqlite->close();
        QSqlDatabase::removeDatabase(sqlite->getActiveDb());
        delete sqlite;
    }
}


/**
 * @brief MainWindow::resizeEvent 윈도우 리사이즈 이벤트 함수
 * @param event QResizeEvent*
 */
void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
print_debug();
    //if (isMaximized()) {
    //        this->show();
    //    }
    if(homeMain != nullptr){
       // emit linker->signal_window_resize();//c220401
        homeMain->reSize(this->size().width());
    }

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    //int height = screenGeometry.height();
    int width = screenGeometry.width();

    if(this->width() >= width){
        this->setMaximumSize(screen->availableSize());
    }

    global.width_mainwindow = this->width();
    global.height_mainwindow = this->height();

    // added by sunnyfish
    global.left_mainwindow = this->pos().x();
    global.top_mainwindow = this->pos().y();

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

/**
 * @brief MainWindow::moveEvent
 * @param event
 */
void MainWindow::moveEvent(QMoveEvent *event){

    QMainWindow::moveEvent(event);
    // added by sunnyfish
    global.left_mainwindow = this->pos().x();
    global.top_mainwindow = this->pos().y();
}


void MainWindow::closeEvent(QCloseEvent *event){

    Q_UNUSED(event);

    // QSettings save
    settings->setValue(rosesettings.SETTINGS_W, this->size().width());
    settings->setValue(rosesettings.SETTINGS_H, this->size().height());
    settings->setValue(rosesettings.SETTINGS_L, this->pos().x());
    settings->setValue(rosesettings.SETTINGS_T, this->pos().y());
    qApp->exit();//  quit();//c220521
}


void MainWindow::hideEvent(QHideEvent *event){

    QMainWindow::hideEvent(event);
    print_debug();
    global.powerDialogShowFlag = true;
    emit linker->signal_window_hide();//c220401
}


void MainWindow::showEvent(QShowEvent *event){

    QMainWindow::showEvent(event);
    print_debug();
    global.powerDialogShowFlag = false;
    //emit linker->signal_window_show();//c220401
}


/**
 * @brief MainWindow::changedIntroPage [SLOT] 인트로 페이지 전환
 */
void MainWindow::changedIntroPage(){

    if(this->stack->currentIndex() == this->stack->count()-1){

        // 튜토리얼 Show 설정값 변경
        settings->setValue(rosesettings.SETTINGS_NEED_TUTORIAL, false);

        // 기존에 사용하던 DB 삭제
        /*
        QDir dir(qApp->applicationDirPath()+"/config");
        if(!dir.exists()){
            dir.mkpath(qApp->applicationDirPath()+"/config");
        }
        */

        QFile file_old(QString("%1%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg("/rose.db"));
        if(file_old.exists()){
            file_old.remove();
        }

        // 선택한 장치 정보 DB 저장
        introRegister->saveDeviceInfoDB();

        // 장치 정보 Data 구성
        setDeviceInfo();

        // 홈화면 Show
        this->homeMain = new HomeMain(this);
        this->setCentralWidget(homeMain);

        // 기기 연결 요청

        QString tmp_ip = global.device.getDeviceIP();
        if(tmp_ip.size() > 0) {
            this->homeMain->connectDevice(tmp_ip);
        }

        QString tmp_ip2 = global.device.getDeviceIP();
        if(tmp_ip2.size() > 0) {
            this->homeMain->connectDevice(tmp_ip2);
        }

        // 튜토리얼 widget hide
        this->stack->hide();
        delete stack;
    }
    else{
        this->stack->setCurrentIndex(this->stack->currentIndex()+1);
        if(stack->currentIndex()==2){
            IntroRegister *introReg = qobject_cast<IntroRegister *>(stack->currentWidget());
            print_debug();
            introReg->searchAllDevice();
        }
    }
}


void MainWindow::changeEvent(QEvent * e) {//c230323

    if(e->type() == QEvent::ActivationChange){
        if(this->isActiveWindow()) {
            //print_debug();
            //global.window_activate_flag = true;
        }else{
           // print_debug();
            //global.window_activate_flag = false;
        }
        // .. this is now the active window
       //print_debug();

    }
    else{
       // print_debug();

    }
}

/*
bool MainWindow::event(QEvent *e)
{
    //if(e->type() == QEvent::ActivationChange && this->isActiveWindow()) {
    qDebug() << "e->type() =" << e->type() ;
    if (e->type() == QEvent::FocusIn) {
        print_debug();
        // window was activated
    }else{
        print_debug();
    }
    return QWidget::event(e);
}
*/
