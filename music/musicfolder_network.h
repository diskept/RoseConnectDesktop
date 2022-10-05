#ifndef MUSICFOLDER_NETWORK_H
#define MUSICFOLDER_NETWORK_H

#include <QWidget>
#include <QThread>
#include <QVBoxLayout>
#include "common/findsmbip.h"
#include "common/findip.h"
#include "common/linker.h"//c220909_1

/**
 * @brief 음악 > 폴더 > 네트워크 > 네트워크 목록 @n
 *      접속가능한 Local Network 검색
 *
 * @note
 *      접속가능한 네트워크인경우 @sa MusicFolder_FileList 파일 목록 Show @n
 *      접속불가능한 네트워크인 경우 네트워크 로그인 Dlg Show
 */
class MusicFolder_Network : public QWidget
{
    Q_OBJECT
public:
    explicit MusicFolder_Network(QWidget *parent = nullptr);
    ~MusicFolder_Network();

    void requestFindIP(bool flagRefresh = false);

signals:
    void signal_clickedHost(const QString &p_hostNAme, const QString &p_ip);

private slots:
    void slot_clickedHost();
    void findedIP(const qint64 &p_uuid, const QString &p_ip, const QString &p_hostName, const bool &flagOn);

private:
    void setInit();
    void setUIcontrol();
    void appendHostUI(const QString &p_ip, const QString &p_hostName, const bool &flagOn);
    void dialog_comfirmNetworkfolder_forFinder_add(QStringList);//c220913_1

    Linker *linker;//c220909_1
    qint64 reqUUID;
    FindIP *findMyIP;
    QThread thread;
    QWidget *wg_contents;

    QVBoxLayout *boxHost;
};

#endif // MUSICFOLDER_NETWORK_H
