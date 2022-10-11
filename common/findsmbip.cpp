#include "findsmbip.h"
#include <QHostInfo>
#include <QFileInfo>
#include <QProcess>
#include "common/global.h"//c220908_2


/**
 * @brief FindSMBIP::FindSMBIP : SMB 연결 가능 IP 조회 @n
 *      아이피 대역 1~255 중 SMB 연결 가능한 IP를 조회한다. @n
 *      SMB 포트 445만 확인하며, NETBIOS port 139 미확인 @n
 * @param parent
 */
FindSMBIP::FindSMBIP(QObject *parent)
    : QObject(parent)
{

}

/**
 * @brief FindSMBIP::~FindSMBIP : 소멸자
 */
FindSMBIP::~FindSMBIP(){
    closeAllSocket();
    list_socket.clear();
}


/**
 * @brief FindSMBIP::setIP : 아이피 세팅
 * @param p_uuid qint64
 * @param p_ip Qstring local ip
 */
void FindSMBIP::setIP(const qint64 &p_uuid, const QString &p_ip){
    uuid = p_uuid;
    ipPrefix = p_ip.left(p_ip.lastIndexOf(".")+1);
}


/**
 * @brief FindSMBIP::findIP: SMB아이피 찾기 실행
 */
void FindSMBIP::findNetworkHost(){

    resultCnt = 0;
    list_socket.clear();

    for(int i = 0 ; i <=255; ++i){
        list_socket.append(new QTcpSocket(this));
        QString tmp_address = ipPrefix + QString::number(i);

        connect(list_socket.at(i), SIGNAL(connected()), SLOT(connectedSocket()));
        connect(list_socket.at(i), SIGNAL(error(QAbstractSocket::SocketError)), SLOT(errorSocket(QAbstractSocket::SocketError)));
        list_socket.at(i)->connectToHost(tmp_address, 445);
    }
}

void FindSMBIP::lookedUp(const QHostInfo &host)
 {
     if (host.error() != QHostInfo::NoError) {
         qDebug() << "Lookup failed:" << host.errorString();
         return;
     }

     foreach (const QHostAddress &address, host.addresses()){
         qDebug() << "Found address:" << address.toString();
         QHostInfo HI = QHostInfo::fromName(address.toString());
         qDebug() << "hostName=" << host.hostName();
     }
     //QNetworkInterface::allAddresses();
 }

/**
 * @brief FindSMBIP::connectedSocket:[슬롯] 소켓 연결 성공
 * @details : emit signal_findedSMBIP / emit signal_end
 */
void FindSMBIP::connectedSocket(){//c221008_1
    for(int i = 0 ; i < list_socket.size(); i++){
        if(sender() == list_socket.at(i)){          

            list_socket.at(i)->close();
            list_socket.at(i)->deleteLater();
            list_socket[i] = nullptr;

            // 아이피 접속 가능 여부 확인
            bool flagOn = true;
            QFileInfo check_file("//"+ipPrefix+ QString::number(i)+"/");
            //QFileInfo check_file("smb//chen:Shwan1378@"+ipPrefix+ QString::number(i)+"/");

            if(!check_file.exists()){//c220909_1
#if defined(Q_OS_WIN)
                flagOn = false;
#endif
#if defined(Q_OS_MAC)
                flagOn = false;
#endif

            }

#if defined(Q_OS_WIN)
            // find IP 시그널
            emit signal_findedSMBIP(uuid, ipPrefix+ QString::number(i), QHostInfo::fromName(ipPrefix+QString::number(i)).hostName(), flagOn);
#endif
#if defined(Q_OS_MAC)
            QProcess* process = new QProcess();
            process->start(QString("smbutil status %1").arg(ipPrefix+ QString::number(i)));//c220913_1
            process->waitForFinished(-1);
            QString std_out = process->readAllStandardOutput();
            print_debug();
            std_out = std_out.split("Server: ").last();
            qDebug() << "std_out=" << std_out.chopped(2);
            if(std_out.isEmpty()){
                qDebug() << "std_out_empty: " << ipPrefix+ QString::number(i);

            }
            // find IP 시그널
            emit signal_findedSMBIP(uuid, ipPrefix+ QString::number(i), std_out.chopped(2), flagOn);
#endif


            resultCnt++;
            // 검색 종료 시그널
            if(resultCnt>255){
                emit signal_endedFind();
            }
            break;
        }
    }
}

/**
 * @brief FindSMBIP::errorSocket:[슬롯][ 소켓 에러
 * @param p_error
 */
void FindSMBIP::errorSocket(QAbstractSocket::SocketError p_error){
    Q_UNUSED(p_error);
    for(int i =0; i < list_socket.size(); i++){
        if(sender() == list_socket.at(i)){

            list_socket.at(i)->close();
            list_socket.at(i)->deleteLater();
            list_socket[i] = nullptr;

            resultCnt++;
            // 검색 종료 시그널
            if(resultCnt>255){
                emit signal_endedFind();
            }
        }
    }
}


/**
 * @brief FindSMBIP::closeAllSocket : 모든 소켓 close
 */
void FindSMBIP::closeAllSocket(){
    for(int i = 0 ; i < list_socket.size(); i++){
        if(list_socket.at(i)!= nullptr && list_socket.at(i)->isOpen()){
            list_socket.at(i)->close();
            list_socket.at(i)->deleteLater();
            list_socket[i] = nullptr;
        }
    }
}
