#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <common/linker.h>
#include <QMainWindow>
#include <home/homemain.h>
#include <QSettings>

#include "intro/introwelcome.h"
#include "intro/introwifi.h"
#include "intro/introregister.h"

/// Defining


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setInitWindowSize();
    static int const EXIT_CODE_REBOOT = -123456789;


private:
    void setInit();
    void setUIControl();
    void setDeviceInfo();
    void setFont();
    void checkDataDir();

    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent *event) override;

    Linker *linker;
    QSettings *settings;

    IntroWelcome *introWelcome;
    IntroWifi *introWifi;
    IntroRegister *introRegister;
    HomeMain *homeMain = nullptr;
    QStackedWidget *stack = nullptr;

    void changeEvent(QEvent * e) override;//c220724
    //bool event(QEvent *e) override;//c220724
    void closeEvent(QCloseEvent* event) override;
    void hideEvent(QHideEvent *event) override;//c220401
    void showEvent(QShowEvent *event) override;//c220401
private slots :
    void changedIntroPage();

};
#endif // MAINWINDOW_H
