#include "filedownloader.h"
#include <QNetworkReply>
#include <QSslConfiguration>

/**
 * @brief The FileDownloader class 파일 다운로더 @n
 *      http 파일 다운로드 주로 이미지파일 다운로드에 사용@n
 *      다운로드 완료시 sigal @sa FileDownloader::signal_downloading(qint64,qint64) 발생 @n
 *      @sa FileDownloader::downloadedData() 함수를 이용해 다운로드 결과 확인(QByteArray) @n
 *      참고 https://wiki.qt.io/Download_Data_from_URL/ko
 * @param imageUrl
 * @param parent
 */
FileDownloader::FileDownloader(QUrl imageUrl, QObject *parent) : QObject(parent)
{
    connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
    SLOT (fileDownloaded(QNetworkReply*)));

    this->setImageURL(imageUrl);
}

/**
 * @brief FileDownloader::FileDownloader 생성자 별도로 만듬.
 * (for 이미지 URL 을 생성자말고 따로 함수 호출하여 세팅하기 위함)
 * 안그러면 이미지 다운받을때마다 계속 생성하고 불필요한 connect 계속 하기때문
 * @param parent
 */
FileDownloader::FileDownloader( QObject *parent) : QObject(parent)
{
    connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)), SLOT (fileDownloaded(QNetworkReply*)));
}

FileDownloader::~FileDownloader()
{
    m_DownloadedData.clear();
}


/**
 * @brief FileDownloader::setImageURL 이미지 경로 별도로 세팅하는 함수 만듬.
 * @param p_imageUrl
 */
void FileDownloader::setImageURL(QUrl p_imageUrl)
{
//    QString tmp = p_imageUrl.url();
//    tmp = tmp.replace("https://", "http://");
//    QUrl url_tmp(tmp);
//    QNetworkRequest request(url_tmp);

    QNetworkRequest request(p_imageUrl);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    reply = m_WebCtrl.get(request);

    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(signal_downloading(qint64,qint64)));
}


void FileDownloader::fileDownloaded(QNetworkReply *pReply)
{
     m_DownloadedData = pReply->readAll();
     //emit a signal
     pReply->deleteLater();
     emit downloaded();
}

QByteArray FileDownloader::downloadedData() const
{
    return m_DownloadedData;
}
