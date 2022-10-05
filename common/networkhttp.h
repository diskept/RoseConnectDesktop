#ifndef NETWORKHTTP_H
#define NETWORKHTTP_H

#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrlQuery>
#include <QTimer>
#include <QEventLoop>
#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QThread>
#include <QXmlStreamReader>
//#include "common/linker.h"//c220601


#define print_common_func() qDebug() << "[QOBUZ][HTTP][INFO]" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";


/**
 * @brief The NetworkHttp class 네트워크 Http 통신 클래스 @n
 * Param 종류로 @sa QJsonObject, @sa QUrlQuery 지원 @n
 * 각 request 함수 param의 bool 값으로 Post, Get 구분, Put 요청은 @sa NetworkHttp::requestPut() 함수 사용@n
 * 각 request 함수 param의 bool 값으로 access_token send
 * @note
 */
class NetworkHttp : public QObject
{
    Q_OBJECT
public:
    NetworkHttp();
    NetworkHttp(QObject *parent);
    ~NetworkHttp();

    /**
     * @brief HTTP 요청 시, Header 추가적으로 옵션 처리해야하는 경우를 위한 Enum값
     */
    enum HeaderOption {
        Tidal_X_Token
        , Tidal_X_Session
        , Tidal_X_Session_with_IF_None_Match
        , Tidal_AccessToken
        , Tidal_AccessToken_with_IF_None_Match
        , Tidal_RefreshToken
        , Tidal_ReqAccessToken
        , Tidal_ReqAccessToken_with_String
    };
    Q_ENUM(HeaderOption);

    enum HeaderOption_forBugs{
        Request_AccessToken
        , Bugs_AccessToken
        , Bugs_RefreshToken
    };
    Q_ENUM(HeaderOption_forBugs);

    enum HeaderOption_forQobuz {
        X_App_Id
        , Qobuz_X_Session
        , Qobuz_X_Session_with_IF_None_Match
    };
    Q_ENUM(HeaderOption_forQobuz);

    enum HeaderOption_forApple {
        Request_UserToken
        , Apple_UserToken
    };
    Q_ENUM(HeaderOption_forApple);

    /**
     * @brief request_forTidal 관련 함수에서 사용함
     */
    enum RequestMethod {
        Get
        , Post
        , Delete
    };
    Q_ENUM(RequestMethod);

    void request(const int &id, const QString &url, const QUrlQuery &param, const bool &flagPost, const bool &flagAuth = false);
    void request(const int &id, const QString &url, const QJsonObject &param, const bool &flagPost, const bool &flagAuth = false);
    void request(const int &id, const QString&, const QString &param, const bool &flagPost, const bool &flagAuth = false);
    void requestPut(const int &id, const QString &url, const QJsonObject &param, const bool &flagAuth = false);
    void requestDelete(const int &id, const QString &url, const QJsonObject &param,  const bool &flagAuth = false);
    void request_forTidal(const int &id, const QString&, const NetworkHttp::HeaderOption&, const QUrlQuery&, const RequestMethod, const bool flagShowDebug=false, const QJsonObject &jsonObj_optUser=QJsonObject());
    void request_forTidal(const int &id, const QString&, const NetworkHttp::HeaderOption&, const QUrlQuery&, const QUrlQuery&, const bool flagShowDebug=false);
    void request_forBugs(const int &id, const QString&, const NetworkHttp::HeaderOption_forBugs&, const QUrlQuery&, const RequestMethod, const bool flagShowDebug=false, const QJsonObject &jsonObj_optUser=QJsonObject());
    void request_forQobuz(const int &id, const QString&, const NetworkHttp::HeaderOption_forQobuz&, const QUrlQuery&, const RequestMethod, const bool flagShowDebug=false, const QJsonObject &jsonObj_optUser=QJsonObject());
    void request_forQobuz(const int &id, const QString&, const NetworkHttp::HeaderOption_forQobuz&, const QUrlQuery&, const QUrlQuery&, const bool flagShowDebug=false);
    void request_forApple(const int &id, const QString&, const NetworkHttp::HeaderOption_forApple&, const QUrlQuery&, const RequestMethod, const bool flagShowDebug=false, const QJsonObject &jsonObj_optUser=QJsonObject());
    void request_forApple(const int &id, const QString&, const NetworkHttp::HeaderOption_forApple&, const QUrlQuery&, const QUrlQuery&, const bool flagShowDebug=false);

    QJsonObject get_userOptJson(){ return this->jsonObj_optUser; }

signals:
    void response(const int &p_id, const QJsonObject &json);
    //void response(const int &p_id, const QString &pageInfo);

private slots:
    void slotReadyRead();
    void slotReadyRead_forTidal();
    void slotReadyRead_forBugs();
    void slotReadyRead_forQobuz();
    void slotReadyRead_forApple();

    void slotError(QNetworkReply::NetworkError);
    void slot_occurError_forTidal(QNetworkReply::NetworkError);
    void slot_occurError_forBugs(QNetworkReply::NetworkError);
    void slot_occurError_forQobuz(QNetworkReply::NetworkError);
    void slot_occurError_forApple(QNetworkReply::NetworkError);
    void slotSslErrors(const QList<QSslError>&);

    void slot_occur_sslError_forTidal(const QList<QSslError>&);
    void slot_occur_sslError_forBugs(const QList<QSslError>&);
    void slot_occur_sslError_forQobuz(const QList<QSslError>&);
    void slot_occur_sslError_forApple(const QList<QSslError>&);
    void replyFinished(QNetworkReply *reply);

    void slotRequestHttp(const QString &url, const QUrlQuery &param, const bool &flagPost, const bool &flagAuth = false);
    void slotRequestHttp(const QString &url, const QJsonObject &param, const bool &flagPost, const bool &flagAuth = false);
    void slotRequestHttp(const QString&, const QString &param, const bool &flagPost, const bool &flagAuth = false);
    void slotRequestHttpPut(const QString &url, const QJsonObject &param, const bool &flagAuth = false);
    void slotRequestHttpDelete(const QString &url, const QJsonObject &param, const bool &flagAuth = false);
    void slotRequestHttp_forTidal(const QString&, const NetworkHttp::HeaderOption&, const QUrlQuery&, const RequestMethod);
    void slotRequestHttp_forTidal(const QString&, const NetworkHttp::HeaderOption&, const QUrlQuery&, const QUrlQuery&);
    void slotRequestHttp_forBugs(const QString&, const NetworkHttp::HeaderOption_forBugs&, const QUrlQuery&, const RequestMethod);
    void slotRequestHttp_forQobuz(const QString&, const NetworkHttp::HeaderOption_forQobuz&, const QUrlQuery&, const RequestMethod);
    void slotRequestHttp_forQobuz(const QString&, const NetworkHttp::HeaderOption_forQobuz&, const QUrlQuery&, const QUrlQuery&);
    void slotRequestHttp_forApple(const QString&, const NetworkHttp::HeaderOption_forApple&, const QUrlQuery&, const RequestMethod);
    void slotRequestHttp_forApple(const QString&, const NetworkHttp::HeaderOption_forApple&, const QUrlQuery&, const QUrlQuery&);

private :
    void setInit();
    void setConnect();

   // Linker *linker;//c220601

    QNetworkReply *reply;
    QByteArray responseData;
    int id=1;
    bool flagOk = false;
    bool flagReadReady = false;

    QString url_request;


    bool flagShowDebug;
    QJsonObject jsonObj_optUser;

};

#endif // NETWORKHTTP_H
