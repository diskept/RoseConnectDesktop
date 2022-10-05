/*
 * Copyright (c) 2014-2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 */

#ifndef _WINDOW_H
#define _WINDOW_H

#include <QMainWindow>
#include <QApplication>
#include "../mainwindow.h"
namespace Ui {
class Window;
}

class QSimpleUpdater;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window (QWidget* parent = 0);
    ~Window();

public slots:
    void mainwindow_start_afterclose();
    void resetFields();
    void checkForUpdates();
    void updateChangelog (const QString& url);
    void displayAppcast (const QString& url, const QByteArray& reply);

private:
    MainWindow w;
    Ui::Window* m_ui;
    QSimpleUpdater* m_updater;
};

#endif
