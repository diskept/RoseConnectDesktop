#ifndef DESKTOPMEDIAPLAYERTHREAD_H
#define DESKTOPMEDIAPLAYERTHREAD_H

#include <QThread>
#include <QMediaPlayer>
#include <QPixmap>
#include <QEventLoop>
#include <common/linker.h>
#include <QFileInfo>
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"
/**
 * @brief 미디어 플레이어 관리 Thread @n
 *      외부 라이브러리 없이 QT로 미디어 파일 MIME 정보 GET
 *
 * @note 성능 문제로 추후 OPEN SOURCE 로 변경 필요
 */
class DesktopMediaPlayerThread : public QThread//c220609

{
    Q_OBJECT

public:
    explicit DesktopMediaPlayerThread(const QString &p_filePath, QObject *parent = 0);
    ~DesktopMediaPlayerThread();
    void stop();

private:
    void run() override;
    void DesktopModetimePlayValue();

private://
    QString filePath;
    Linker *linker;
    bool working = true;

    void select_table(QString str_table);

signals:
    void signal_stopThread();

public slots:
    //void slot_desktopModeStart();//c220704
     void slot_loadedMeidaFile();
     void slot_playMusic_desktop(QJsonArray jsonArrMusic, bool flagPlayShuffle);//c220704

};

#endif // DESKTOPMEDIAPLAYERTHREAD_H
