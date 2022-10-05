#include "networkhttp.h"
#include "common/global.h"
#include "bugs/ProcBugsAPI.h"
#include "home/ReplyTimeout.h"

const int TIMEOUTValue =  25000;//c221004_1
/**
 * @brief NetworkHttp::NetworkHttp : Http + TimeOut + Thread
 * @details QNetworkAccessManager를 이용한 Http 통신
 * QTimer를 이용해 타임아웃 설정
 * QThread를 이용해 메인쓰레드와 분기(moveToThread func 이용)
 * @param p_parent
 */
NetworkHttp::NetworkHttp()
    : QObject()
{
    // 초기 세팅
    setInit();
    setConnect();

}

NetworkHttp::NetworkHttp(QObject *object)
    : QObject(object)
{

}

NetworkHttp::~NetworkHttp(){

    this->deleteLater();
}

void NetworkHttp::setInit(){


    //ReplyTimeout::set(reply, 3000);
   // linker = Linker::getInstance();
}

/**
 * @brief NetworkHttp::setConnect : QThread 위한 connect 호출
 */
void NetworkHttp::setConnect(){
   // nothing not use
}


/**
 * @brief NetworkHttp::request
 * @param p_url URL
 * @param p_param QUrlQuery
 * @param flagPost if true, POST
 */
void NetworkHttp::request(const int &p_id,const QString &p_url, const QUrlQuery &p_param, const bool &flagPost, const bool &flagAuth){
    //thread.start();
    id = p_id;

    slotRequestHttp(p_url, p_param, flagPost, flagAuth);
    ReplyTimeout::set(reply, TIMEOUTValue);

}


/**
 * @brief NetworkHttp::request
 * @param p_url URL
 * @param p_param QUrlQuery
 * @param flagPost if true, POST
 */
void NetworkHttp::request(const int &p_id, const QString &p_url, const QJsonObject &p_param, const bool &flagPost, const bool &flagAuth){
    //thread.start();
    id = p_id;
    slotRequestHttp(p_url, p_param, flagPost, flagAuth);
    ReplyTimeout::set(reply, TIMEOUTValue);
}

/**
 * @brief NetworkHttp::request
 * @param p_url URL
 * @param p_param QString
 * @param flagPost if true, POST
 */
void NetworkHttp::request(const int &p_id, const QString &p_url, const QString &p_param, const bool &flagPost, const bool &flagAuth){
    id = p_id;
    slotRequestHttp(p_url, p_param, flagPost, flagAuth);
    ReplyTimeout::set(reply, TIMEOUTValue);
}


/**
 * @brief NetworkHttp::requestPut
 * @param p_url URL
 * @param p_param QUrlQuery
 */
void NetworkHttp::requestPut(const int &p_id, const QString &p_url, const QJsonObject &p_param,  const bool &flagAuth){
    //thread.start();
    id = p_id;
    slotRequestHttpPut(p_url, p_param, flagAuth);
    ReplyTimeout::set(reply, TIMEOUTValue);
}


/**
 * @brief NetworkHttp::requestPut
 * @param p_url URL
 * @param p_param QUrlQuery
 */
void NetworkHttp::requestDelete(const int &p_id, const QString &p_url, const QJsonObject &p_param,  const bool &flagAuth){
    //thread.start();
    id = p_id;
    slotRequestHttpDelete(p_url, p_param, flagAuth);
}


/**
 * @brief TIDAL API 요청 관련
 * @param p_url URL
 * @param p_headerOpt TIDAL 파트에서 사용을 위해서 추가된 함수임. Tidal 전용으로 사용하는 함수
 * @param p_param QUrlQuery
 * @param flagPost if true, POST
 */
void NetworkHttp::request_forTidal(const int &p_id,const QString &p_url, const HeaderOption &p_headerOpt, const QUrlQuery &p_param, const RequestMethod method, const bool flagShowDebug, const QJsonObject &jsonObj_optUser){
    //thread.start();
    id = p_id;
    this->flagShowDebug = flagShowDebug;
    this->jsonObj_optUser = jsonObj_optUser;
    //qDebug() << "NetworkHttp::request_forTidal()-------------------------------------";//cheon-qdebug01
    //qDebug() << p_url;
    //qDebug() << p_headerOpt;
    //qDebug() << flagShowDebug;
    //qDebug() << jsonObj_optUser;
    //QJsonDocument doc(jsonObj_optUser);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << strJson;
    slotRequestHttp_forTidal(p_url, p_headerOpt, p_param, method);
}


/**
 * @brief TIDAL API 요청 관련
 * @param p_id
 * @param p_url
 * @param p_headerOpt
 * @param p_param_post - post용 params
 * @param p_param_get = get용 params
 * @param flagShowDebug
 */
void NetworkHttp::request_forTidal(const int &p_id, const QString &p_url, const HeaderOption &p_headerOpt, const QUrlQuery &p_param_post, const QUrlQuery& p_param_get, const bool flagShowDebug){
    id = p_id;
    this->flagShowDebug = flagShowDebug;

    slotRequestHttp_forTidal(p_url, p_headerOpt, p_param_post, p_param_get);
}


/**
 * @brief NetworkHttp::request_forBugs
 * @param p_id
 * @param p_url
 * @param p_headerOpt
 * @param p_param
 * @param method
 * @param flagShowDebug
 */
void NetworkHttp::request_forBugs(const int &p_id,const QString &p_url, const HeaderOption_forBugs &p_headerOpt, const QUrlQuery &p_param, const RequestMethod method, const bool flagShowDebug, const QJsonObject &jsonObj_optUser){
    //thread.start();
    id = p_id;
    this->flagShowDebug = flagShowDebug;
    this->jsonObj_optUser = jsonObj_optUser;
    slotRequestHttp_forBugs(p_url, p_headerOpt, p_param, method);
}


/**
 * @brief Qobuz API 요청 관련         // TODO :: Added Jeon 12/14/2020
 * @param p_url URL
 * @param p_headerOpt Qobuz 파트에서 사용을 위해서 추가된 함수임. Qobuz 전용으로 사용하는 함수
 * @param p_param QUrlQuery
 * @param flagPost if true, POST
 */
void NetworkHttp::request_forQobuz(const int &p_id,const QString &p_url, const HeaderOption_forQobuz &p_headerOpt, const QUrlQuery &p_param, const RequestMethod method, const bool flagShowDebug, const QJsonObject &jsonObj_optUser){
    //thread.start();
    id = p_id;
    this->flagShowDebug = flagShowDebug;
    this->jsonObj_optUser = jsonObj_optUser;
    //qDebug() << "NetworkHttp::request_forQobuz()-------------------------------------";//cheon-qdebug01
    //qDebug() << p_url;
    //qDebug() << p_headerOpt;
    //qDebug() << flagShowDebug;
    //qDebug() << jsonObj_optUser;
    //QJsonDocument doc(jsonObj_optUser);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << strJson;
    slotRequestHttp_forQobuz(p_url, p_headerOpt, p_param, method);
}


/**
 * @brief Qobuz API 요청 관련         // TODO :: Added Jeon 12/14/2020
 * @param p_id
 * @param p_url
 * @param p_headerOpt
 * @param p_param_post - post용 params
 * @param p_param_get = get용 params
 * @param flagShowDebug
 */
void NetworkHttp::request_forQobuz(const int &p_id, const QString &p_url, const HeaderOption_forQobuz &p_headerOpt, const QUrlQuery &p_param_post, const QUrlQuery& p_param_get, const bool flagShowDebug){

    id = p_id;
    this->flagShowDebug = flagShowDebug;

    slotRequestHttp_forQobuz(p_url, p_headerOpt, p_param_post, p_param_get);
}


/**
 * @brief TIDAL API 요청 관련
 * @param p_url URL
 * @param p_headerOpt TIDAL 파트에서 사용을 위해서 추가된 함수임. Tidal 전용으로 사용하는 함수
 * @param p_param QUrlQuery
 * @param flagPost if true, POST
 */
void NetworkHttp::request_forApple(const int &p_id,const QString &p_url, const HeaderOption_forApple &p_headerOpt, const QUrlQuery &p_param, const RequestMethod method, const bool flagShowDebug, const QJsonObject &jsonObj_optUser){

    id = p_id;
    this->flagShowDebug = flagShowDebug;
    this->jsonObj_optUser = jsonObj_optUser;

    slotRequestHttp_forApple(p_url, p_headerOpt, p_param, method);
}


/**
 * @brief TIDAL API 요청 관련
 * @param p_id
 * @param p_url
 * @param p_headerOpt
 * @param p_param_post - post용 params
 * @param p_param_get = get용 params
 * @param flagShowDebug
 */
void NetworkHttp::request_forApple(const int &p_id, const QString &p_url, const HeaderOption_forApple &p_headerOpt, const QUrlQuery &p_param_post, const QUrlQuery& p_param_get, const bool flagShowDebug){

    id = p_id;
    this->flagShowDebug = flagShowDebug;

    slotRequestHttp_forApple(p_url, p_headerOpt, p_param_post, p_param_get);
}


/**
 * @brief NetworkHttp::slotRequestHttp : Http 요청함수
 * @param p_url
 * @param param
 * @param flagPost
 */
void NetworkHttp::slotRequestHttp(const QString &p_url, const QUrlQuery &param, const bool &flagPost, const bool &flagAuth){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8;");

    if(flagAuth){
        QString header = global.user.getToken_type() + " " + global.user.getAccess_token();
        //qDebug() << "slotRequestHttp.1 :" << global.user.getToken_type() << " " << global.user.getAccess_token() << "\n";
        request.setRawHeader("Authorization", header.toUtf8());
    }


    if(flagPost){
        request.setUrl(serviceURL);
        QByteArray jsonData = param.query(QUrl::FullyEncoded).toUtf8();
        reply = m_qnam->post(request, jsonData);
    }else{
        serviceURL.setQuery(param.query());
        request.setUrl(serviceURL);
        reply = m_qnam->get(request);
    }
    //qDebug() << "reply_timeout=" << reply->time;

    connect(reply, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)),this, SLOT(slotSslErrors(const QList<QSslError>&)));

}



/**
 * @brief NetworkHttp::slotRequestHttp : Http 요청함수
 * @param p_url
 * @param param
 * @param flagPost
 */
void NetworkHttp::slotRequestHttp(const QString &p_url, const QJsonObject &param, const bool &flagPost, const bool &flagAuth){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8;");

    if(flagAuth){
        QString header = global.user.getToken_type() + " " + global.user.getAccess_token();
        //qDebug() << "slotRequestHttp.2 :" << header << "\n";
        request.setRawHeader("Authorization", header.toUtf8());
    }

    if(flagPost){
        QJsonDocument doc(param);        
        QByteArray jsonData = doc.toJson();
        //QByteArray jsonData = doc.toJson(QJsonDocument::JsonFormat::Compact);         // testing by sunnyfish
        request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(jsonData.size()));
        request.setUrl(serviceURL);

        reply = m_qnam->post(request, jsonData);

        //print_common_func();
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << strJson << "\n";

    }else{
        //serviceURL.setQuery(param.query());
        request.setUrl(serviceURL);
        reply = m_qnam->get(request);
    }


//ReplyTimeout::set(reply, 5000);

    connect(reply, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)),this, SLOT(slotSslErrors(const QList<QSslError>&)));
}



void NetworkHttp::slotRequestHttp(const QString &p_url, const QString &param, const bool &flagPost, const bool &flagAuth){
    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8;");
    //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8;");

    if(flagAuth){
        QString header = global.user.getToken_type() + " " + global.user.getAccess_token();
        request.setRawHeader("Authorization", header.toUtf8());
    }

    if(flagPost){
        QByteArray jsonData = param.toUtf8();

        request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(param.size()));
        request.setUrl(serviceURL);

        reply = m_qnam->post(request, jsonData);

        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << strJson << jsonData.size();

    }else{
        request.setUrl(serviceURL);
        reply = m_qnam->get(request);
    }
    //ReplyTimeout::set(reply, 5000);
    //ReplyTimeout::set(reply, 100);
    connect(reply, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)),this, SLOT(slotSslErrors(const QList<QSslError>&)));
}



/**
 * @brief NetworkHttp::slotRequestHttp : Http 요청함수
 * @param p_url
 * @param param
 * @param flagPost
 */
void NetworkHttp::slotRequestHttpPut(const QString &p_url, const QJsonObject &param, const bool &flagAuth){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8;");

    if(flagAuth){
        QString header = global.user.getToken_type() + " " + global.user.getAccess_token();
        request.setRawHeader("Authorization", header.toUtf8());
    }

    QJsonDocument doc(param);
    QByteArray jsonData = doc.toJson();

    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(jsonData.size()));
    request.setUrl(serviceURL);

    reply = m_qnam->put(request, jsonData);

    connect(reply, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)),this, SLOT(slotSslErrors(const QList<QSslError>&)));
}



/**
 * @brief NetworkHttp::slotRequestHttp : Http 요청함수
 * @param p_url
 * @param param
 * @param flagPost
 */
void NetworkHttp::slotRequestHttpDelete(const QString &p_url, const QJsonObject &param, const bool &flagAuth){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8;");

    if(flagAuth){
        QString header = global.user.getToken_type() + " " + global.user.getAccess_token();
        request.setRawHeader("Authorization", header.toUtf8());
    }

    if(!param.empty()){
        QJsonDocument doc(param);
        QByteArray jsonData = doc.toJson();

        request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(jsonData.size()));

        request.setUrl(serviceURL);

        reply = m_qnam->deleteResource(request);
    }
    else{
        request.setUrl(serviceURL);

        reply = m_qnam->deleteResource(request);
    }

    connect(reply, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)),this, SLOT(slotSslErrors(const QList<QSslError>&)));
}


/**
 * @brief NetworkHttp::slotRequestHttp
 * @param p_url
 * @param p_headerOpt TIDAL 파트에서 사용을 위해서 추가된 함수임. Tidal 전용으로 사용하는 함수
 * @param param
 * @param flagPost
 * @param flagAuth
 */
void NetworkHttp::slotRequestHttp_forTidal(const QString &p_url, const HeaderOption &p_headerOpt, const QUrlQuery &param, const RequestMethod method){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8;");

    //---------------------------------------------------------------------------
    // headerOpt 처리
    //---------------------------------------------------------------------------
    if(p_headerOpt == HeaderOption::Tidal_X_Token) {
        // TIDAL
        QString header = global.tidalToken;
        request.setRawHeader("X-Tidal-Token", header.toUtf8());
    }
    else if(p_headerOpt == HeaderOption::Tidal_X_Session) {
        QString header = global.user_forTidal.getSessionId();
        request.setRawHeader("X-Tidal-SessionId", header.toUtf8());
    }
    else if(p_headerOpt == HeaderOption::Tidal_X_Session_with_IF_None_Match){
        QString header = global.user_forTidal.getSessionId();
        request.setRawHeader("X-Tidal-SessionId", header.toUtf8());
        request.setRawHeader("If-None-Match", QString("*").toUtf8());
    }
    else if(p_headerOpt == HeaderOption::Tidal_AccessToken){
        QString tmp_auth = "Bearer " + global.user_forTidal.get_access_token();
        request.setRawHeader("Authorization", tmp_auth.toUtf8());
    }
    else if(p_headerOpt == HeaderOption::Tidal_AccessToken_with_IF_None_Match){
        QString tmp_auth = "Bearer " + global.user_forTidal.get_access_token();
        request.setRawHeader("Authorization", tmp_auth.toUtf8());
        request.setRawHeader("If-None-Match", QString("*").toUtf8());
    }
    else if(p_headerOpt == HeaderOption::Tidal_RefreshToken){

    }
    else if(p_headerOpt == HeaderOption::Tidal_ReqAccessToken){
        // No header
    }

    //---------------------------------------------------------------------------
    // Body 처리
    //---------------------------------------------------------------------------
    if(method == NetworkHttp::RequestMethod::Get){
        serviceURL.setQuery(param.query());
        request.setUrl(serviceURL);
        reply = m_qnam->get(request);
    }
    else if(method == NetworkHttp::RequestMethod::Post){
        request.setUrl(serviceURL);
        QByteArray postData = param.query(QUrl::FullyEncoded).toUtf8();
        reply = m_qnam->post(request, postData);
    }
    else if(method == NetworkHttp::RequestMethod::Delete){
        request.setUrl(serviceURL);
        reply = m_qnam->deleteResource(request);
    }

    connect(reply, &QNetworkReply::readyRead, this, &NetworkHttp::slotReadyRead_forTidal);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slot_occurError_forTidal(QNetworkReply::NetworkError)));
    connect(reply, &QNetworkReply::sslErrors, this, &NetworkHttp::slot_occur_sslError_forTidal);
}


/**
 * @brief NetworkHttp::slotRequestHttp_forTidal
 * @param p_url
 * @param p_headerOpt
 * @param param_post
 * @param param_get
 */
void NetworkHttp::slotRequestHttp_forTidal(const QString &p_url, const HeaderOption &p_headerOpt, const QUrlQuery &param_post, const QUrlQuery &param_get){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8;");

    //---------------------------------------------------------------------------
    // headerOpt 처리
    //---------------------------------------------------------------------------
    if(p_headerOpt == HeaderOption::Tidal_X_Token) {
        // TIDAL
        QString header = global.tidalToken;
        request.setRawHeader("X-Tidal-Token", header.toUtf8());
    }
    else if(p_headerOpt == HeaderOption::Tidal_X_Session) {
        QString header = global.user_forTidal.getSessionId();
        request.setRawHeader("X-Tidal-SessionId", header.toUtf8());
    }
    else if(p_headerOpt == HeaderOption::Tidal_X_Session_with_IF_None_Match){
        QString header = global.user_forTidal.getSessionId();
        request.setRawHeader("X-Tidal-SessionId", header.toUtf8());
        request.setRawHeader("If-None-Match", QString("*").toUtf8());
    }
    else if(p_headerOpt == HeaderOption::Tidal_AccessToken){
        QString tmp_auth = "Bearer " + global.user_forTidal.get_access_token();
        request.setRawHeader("Authorization", tmp_auth.toUtf8());
    }
    else if(p_headerOpt == HeaderOption::Tidal_AccessToken_with_IF_None_Match){
        QString tmp_auth = "Bearer " + global.user_forTidal.get_access_token();
        request.setRawHeader("Authorization", tmp_auth.toUtf8());
        request.setRawHeader("If-None-Match", QString("*").toUtf8());
    }
    else if(p_headerOpt == HeaderOption::Tidal_RefreshToken){

    }
    else if(p_headerOpt == HeaderOption::Tidal_ReqAccessToken){
        // No header
    }

    //---------------------------------------------------------------------------
    // Body 처리
    //---------------------------------------------------------------------------
    serviceURL.setQuery(param_get.query());
    request.setUrl(serviceURL);

    QByteArray postData = param_post.query(QUrl::FullyEncoded).toUtf8();
    reply = m_qnam->post(request, postData);

    connect(reply, &QNetworkReply::readyRead, this, &NetworkHttp::slotReadyRead_forTidal);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slot_occurError_forTidal(QNetworkReply::NetworkError)));
    connect(reply, &QNetworkReply::sslErrors, this, &NetworkHttp::slot_occur_sslError_forTidal);
}


/**
 * @brief NetworkHttp::slotRequestHttp_forBugs
 * @param p_url
 * @param p_headerOpt
 * @param param
 * @param method
 */
void NetworkHttp::slotRequestHttp_forBugs(const QString &p_url, const HeaderOption_forBugs &p_headerOpt, const QUrlQuery &param, const RequestMethod method){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8;");

    //---------------------------------------------------------------------------
    // HeaderOption_forBugs 처리
    //---------------------------------------------------------------------------
    if(p_headerOpt == HeaderOption_forBugs::Bugs_AccessToken) {
        QString tmp_auth = "Bearer " + global.user_forBugs.get_access_token();
        request.setRawHeader("Authorization", tmp_auth.toUtf8());
        request.setRawHeader("User-Agent", bugs::ProcBugsAPI::get_headerForRequest_userAgent().toUtf8());
    }
    else if(p_headerOpt == HeaderOption_forBugs::Bugs_RefreshToken){

    }
    else if(p_headerOpt == HeaderOption_forBugs::Request_AccessToken){
        // No header
    }

    //---------------------------------------------------------------------------
    // Body 처리
    //---------------------------------------------------------------------------
    if(method == NetworkHttp::RequestMethod::Get){
        serviceURL.setQuery(param.query());
        request.setUrl(serviceURL);
        reply = m_qnam->get(request);
    }
    else if(method == NetworkHttp::RequestMethod::Post){
        request.setUrl(serviceURL);
        QByteArray postData = param.query(QUrl::FullyEncoded).toUtf8();
        reply = m_qnam->post(request, postData);
    }
    else if(method == NetworkHttp::RequestMethod::Delete){
        serviceURL.setQuery(param.query());
        request.setUrl(serviceURL);
        reply = m_qnam->deleteResource(request);
    }

    connect(reply, &QNetworkReply::readyRead, this, &NetworkHttp::slotReadyRead_forBugs);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slot_occurError_forBugs(QNetworkReply::NetworkError)));
    connect(reply, &QNetworkReply::sslErrors, this, &NetworkHttp::slot_occur_sslError_forBugs);
}


/**
 * @brief NetworkHttp::slotRequestHttp        // TODO :: Added Jeon 12/14/2020
 * @param p_url
 * @param p_headerOpt Qobuz 파트에서 사용을 위해서 추가된 함수임. Qobuz 전용으로 사용하는 함수
 * @param param
 * @param flagPost
 * @param flagAuth
 */
void NetworkHttp::slotRequestHttp_forQobuz(const QString &p_url, const HeaderOption_forQobuz &p_headerOpt, const QUrlQuery &param, const RequestMethod method){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;    
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8;");

    //---------------------------------------------------------------------------
    // headerOpt 처리
    //---------------------------------------------------------------------------
    if(p_headerOpt == HeaderOption_forQobuz::X_App_Id) {
        // Qobuz
        QString header = global.X_App_Id;
        request.setRawHeader("X-App-Id", header.toUtf8());                  //cheon_debug1220
    }
    else if(p_headerOpt == HeaderOption_forQobuz::Qobuz_X_Session) {

        QString header = global.X_App_Id;
        request.setRawHeader("X-App-Id", header.toUtf8());                  //cheon_debug1220

        QString header1 = global.user_forQobuz.getUserAuthToken();
        request.setRawHeader("X-User-Auth-Token", header1.toUtf8());        //cheon_debug1220
    }
    else if(p_headerOpt == HeaderOption_forQobuz::Qobuz_X_Session_with_IF_None_Match){

        QString header = global.user_forQobuz.getUserAuthToken();           //cheon_debug1220
        request.setRawHeader("X-User_Auth_Token", header.toUtf8());
        request.setRawHeader("If-None-Match", QString("*").toUtf8());
    }

    //---------------------------------------------------------------------------
    // Body 처리
    //---------------------------------------------------------------------------
    if(method == NetworkHttp::RequestMethod::Get){
        serviceURL.setQuery(param.query());
        request.setUrl(serviceURL);
        reply = m_qnam->get(request);
    }
    else if(method == NetworkHttp::RequestMethod::Post){
        request.setUrl(serviceURL);
        QByteArray postData = param.query(QUrl::FullyEncoded).toUtf8();
        reply = m_qnam->post(request, postData);
    }
    else if(method == NetworkHttp::RequestMethod::Delete){
        request.setUrl(serviceURL);
        reply = m_qnam->deleteResource(request);
    }

    connect(reply, &QNetworkReply::readyRead, this, &NetworkHttp::slotReadyRead_forQobuz);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slot_occurError_forQobuz(QNetworkReply::NetworkError)));
    connect(reply, &QNetworkReply::sslErrors, this, &NetworkHttp::slot_occur_sslError_forQobuz);
}


/**
 * @brief NetworkHttp::slotRequestHttp_forQobuz       // TODO :: Added Jeon 12/14/2020
 * @param p_url
 * @param p_headerOpt
 * @param param_post
 * @param param_get
 */
void NetworkHttp::slotRequestHttp_forQobuz(const QString &p_url, const HeaderOption_forQobuz &p_headerOpt, const QUrlQuery &param_post, const QUrlQuery &param_get){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;    
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8;");

    //---------------------------------------------------------------------------
    // headerOpt 처리
    //---------------------------------------------------------------------------
    if(p_headerOpt == HeaderOption_forQobuz::X_App_Id) {
        // Qobuz
        QString header = global.X_App_Id;
        request.setRawHeader("X-App-Id", header.toUtf8());
    }
    else if(p_headerOpt == HeaderOption_forQobuz::Qobuz_X_Session) {

        QString header = global.X_App_Id;
        request.setRawHeader("X-App-Id", header.toUtf8());                  //cheon_debug1220

        QString header1 = global.user_forQobuz.getUserAuthToken();
        request.setRawHeader("X-User-Auth-Token", header1.toUtf8());        //cheon_debug1220
    }
    else if(p_headerOpt == HeaderOption_forQobuz::Qobuz_X_Session_with_IF_None_Match){

        QString header = global.user_forQobuz.getUserAuthToken();           //cheon_debug1220
        request.setRawHeader("X-User_Auth_Token", header.toUtf8());
        request.setRawHeader("If-None-Match", QString("*").toUtf8());
    }

    //---------------------------------------------------------------------------
    // Body 처리
    //---------------------------------------------------------------------------
    serviceURL.setQuery(param_get.query());
    request.setUrl(serviceURL);

    QByteArray postData = param_post.query(QUrl::FullyEncoded).toUtf8();
    reply = m_qnam->post(request, postData);

    connect(reply, &QNetworkReply::readyRead, this, &NetworkHttp::slotReadyRead_forQobuz);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slot_occurError_forQobuz(QNetworkReply::NetworkError)));
    connect(reply, &QNetworkReply::sslErrors, this, &NetworkHttp::slot_occur_sslError_forQobuz);
}


/**
 * @brief NetworkHttp::slotRequestHttp
 * @param p_url
 * @param p_headerOpt TIDAL 파트에서 사용을 위해서 추가된 함수임. Tidal 전용으로 사용하는 함수
 * @param param
 * @param flagPost
 * @param flagAuth
 */
void NetworkHttp::slotRequestHttp_forApple(const QString &p_url, const HeaderOption_forApple &p_headerOpt, const QUrlQuery &param, const RequestMethod method){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8;");

    //---------------------------------------------------------------------------
    // headerOpt 처리
    //---------------------------------------------------------------------------
    if(p_headerOpt == HeaderOption_forApple::Request_UserToken) {
        QString tmp_auth = "Bearer " + global.user.getShazam_token();
        request.setRawHeader("Authorization", tmp_auth.toUtf8());
    }
    else if(p_headerOpt == HeaderOption_forApple::Apple_UserToken) {
        QString tmp_auth = "Bearer " + global.user.getShazam_token();   //global.user_forApple.get_user_token();
        request.setRawHeader("Authorization", tmp_auth.toUtf8());
    }

    //---------------------------------------------------------------------------
    // Body 처리
    //---------------------------------------------------------------------------
    if(method == NetworkHttp::RequestMethod::Get){
        serviceURL.setQuery(param.query());
        request.setUrl(serviceURL);
        reply = m_qnam->get(request);
    }
    else if(method == NetworkHttp::RequestMethod::Post){
        request.setUrl(serviceURL);
        QByteArray postData = param.query(QUrl::FullyEncoded).toUtf8();
        reply = m_qnam->post(request, postData);
    }
    else if(method == NetworkHttp::RequestMethod::Delete){
        request.setUrl(serviceURL);
        reply = m_qnam->deleteResource(request);
    }

    connect(reply, &QNetworkReply::readyRead, this, &NetworkHttp::slotReadyRead_forApple);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slot_occurError_forApple(QNetworkReply::NetworkError)));
    connect(reply, &QNetworkReply::sslErrors, this, &NetworkHttp::slot_occur_sslError_forApple);
}


/**
 * @brief NetworkHttp::slotRequestHttp_forTidal
 * @param p_url
 * @param p_headerOpt
 * @param param_post
 * @param param_get
 */
void NetworkHttp::slotRequestHttp_forApple(const QString &p_url, const HeaderOption_forApple &p_headerOpt, const QUrlQuery &param_post, const QUrlQuery &param_get){

    QNetworkAccessManager * m_qnam = new QNetworkAccessManager(this);
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QUrl serviceURL(p_url);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Rose Desktop");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8;");

    //---------------------------------------------------------------------------
    // headerOpt 처리
    //---------------------------------------------------------------------------
    if(p_headerOpt == HeaderOption_forApple::Request_UserToken) {
        QString tmp_auth = "Bearer " + global.user.getShazam_token();
        request.setRawHeader("Authorization", tmp_auth.toUtf8());
    }
    else if(p_headerOpt == HeaderOption_forApple::Apple_UserToken) {
        QString tmp_auth = "Bearer " + global.user.getShazam_token();   //global.user_forApple.get_user_token();
        request.setRawHeader("Authorization", tmp_auth.toUtf8());
    }

    //---------------------------------------------------------------------------
    // Body 처리
    //---------------------------------------------------------------------------
    serviceURL.setQuery(param_get.query());
    request.setUrl(serviceURL);

    QByteArray postData = param_post.query(QUrl::FullyEncoded).toUtf8();
    reply = m_qnam->post(request, postData);

    connect(reply, &QNetworkReply::readyRead, this, &NetworkHttp::slotReadyRead_forApple);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(slot_occurError_forApple(QNetworkReply::NetworkError)));
    connect(reply, &QNetworkReply::sslErrors, this, &NetworkHttp::slot_occur_sslError_forApple);
}


/**
 * @brief NetworkHttp::slotReadyRead : SLOT - Http 데이터 읽기
 */
void NetworkHttp::slotReadyRead(){

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    if(status == 200 || status == 400 || status == 500){
        flagOk = true;
        flagReadReady = true;
    }
}


/**
 * @brief NetworkHttp::slotReadyRead : SLOT - Http 데이터 읽기
 */
void NetworkHttp::slotReadyRead_forTidal(){

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();

    if(status == 200 || status == 201){
        flagOk = true;
        flagReadReady = true;
    }
    else if(status == 11003 || status == 6001 || status == 11001 || status == 11002 || status == 11101){
        flagOk = true;
        flagReadReady = true;
    }
    else if(status == 400 || status == 401 || status == 404 || status == 500){
        flagOk = true;
        flagReadReady = true;
    }
}


/**
 * @brief NetworkHttp::slotReadyRead : SLOT - Http 데이터 읽기
 */
void NetworkHttp::slotReadyRead_forBugs(){

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();

    if(status == 200 || status == 201){
        flagOk = true;
        flagReadReady = true;
    }
    else if(status == 400 || status == 401 || status == 404 || status == 500){
        flagOk = true;
        flagReadReady = true;
    }
}


/**
 * @brief NetworkHttp::slotReadyRead : SLOT - Http 데이터 읽기     // TODO :: Added Jeon 12/14/2020
 */
void NetworkHttp::slotReadyRead_forQobuz(){

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();

    if(status == 200 || status == 201){
        flagOk = true;
        flagReadReady = true;
    }
    else if(status == 400 || status == 401 || status == 404 || status == 500){
        flagOk = true;
        flagReadReady = true;
    }
}


/**
 * @brief NetworkHttp::slotReadyRead : SLOT - Http 데이터 읽기
 */
void NetworkHttp::slotReadyRead_forApple(){

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();

    if(status == 200 || status == 201){
        flagOk = true;
        flagReadReady = true;
    }
    else if(status == 400 || status == 401 || status == 404 || status == 500){
        flagOk = true;
        flagReadReady = true;
    }
}


/**
 * @brief NetworkHttp::slotError : SLOT - Http 에러
 * @param err
 */
void NetworkHttp::slotError(QNetworkReply::NetworkError){
//print_debug();
//qDebug() << "*****************************************************************************";
  // emit linker->signal_device_connect_check_error();

    flagOk = false;
}


/**
 * @brief http 에러 발생
 * @param error  - NoError, ConnectionRefusedError, RemoteHostClosedError, HostNotFoundError, TimeoutError, …, UnknownServerError
 */
void NetworkHttp::slot_occurError_forTidal(QNetworkReply::NetworkError error){

    Q_UNUSED(error);
    flagOk = false;
}


/**
 * @brief http 에러 발생
 * @param error  - NoError, ConnectionRefusedError, RemoteHostClosedError, HostNotFoundError, TimeoutError, …, UnknownServerError
 */
void NetworkHttp::slot_occurError_forBugs(QNetworkReply::NetworkError error){

    Q_UNUSED(error);
    flagOk = false;
}


/**  
 * @brief http 에러 발생
 * @param error  - NoError, ConnectionRefusedError, RemoteHostClosedError, HostNotFoundError, TimeoutError, …, UnknownServerError
 */
void NetworkHttp::slot_occurError_forQobuz(QNetworkReply::NetworkError error){

    Q_UNUSED(error);
    flagOk = false;
}


/**
 * @brief http 에러 발생
 * @param error  - NoError, ConnectionRefusedError, RemoteHostClosedError, HostNotFoundError, TimeoutError, …, UnknownServerError
 */
void NetworkHttp::slot_occurError_forApple(QNetworkReply::NetworkError error){

    Q_UNUSED(error);
    flagOk = false;
}


/**
 * @brief NetworkHttp::slotSslErrors : SLOT - SSL 에러
 * @param err
 */
void NetworkHttp::slotSslErrors(const QList<QSslError> &p_listError){

    Q_UNUSED(p_listError);
    flagOk = false;
}


/**
 * @brief NetworkHttp::slot_occur_sslError
 * @param list_error
 */
void NetworkHttp::slot_occur_sslError_forTidal(const QList<QSslError>& list_error){

    Q_UNUSED(list_error);
    flagOk = false;
}


/**
 * @brief NetworkHttp::slot_occur_sslError
 * @param list_error
 */
void NetworkHttp::slot_occur_sslError_forBugs(const QList<QSslError>& list_error){

    Q_UNUSED(list_error);
    flagOk = false;
}


/**
 * @brief NetworkHttp::slot_occur_sslError    // TODO :: Added Jeon 12/14/2020
 * @param list_error
 */
void NetworkHttp::slot_occur_sslError_forQobuz(const QList<QSslError>& list_error){

    Q_UNUSED(list_error);
    flagOk = false;
}


/**
 * @brief NetworkHttp::slot_occur_sslError
 * @param list_error
 */
void NetworkHttp::slot_occur_sslError_forApple(const QList<QSslError>& list_error){

    Q_UNUSED(list_error);
    flagOk = false;
}


/**
 * @brief NetworkHttp::replyFinished : SLOT - Http 요청 종료
 * Http 응답결과를 SIGNAL response(QJsonObject)로 전달한다.
 * @param reply
 */
void NetworkHttp::replyFinished(QNetworkReply* reply){

    if(flagReadReady){
        responseData = reply->readAll();
    }

    QString responseValue(responseData);

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(responseValue.toUtf8());

    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "p_jsonObj = " << strJson;

    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            obj.insert("flagOk", flagOk);
        }else if(doc.isArray()){
            obj.insert("array", doc.array());
            obj.insert("flagOk", flagOk);
        }
        else{
            obj.insert("flagOk", false);
        }
    }else{
        obj.insert("flagOk", false);
    }

    // QNetworkReply close
    reply->close();

    reply->deleteLater();
    sender()->deleteLater();

    emit response(id, obj);
}
