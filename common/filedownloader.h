#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

/**
 * @brief The FileDownloader class 파일 다운로더 @n
 *      http 파일 다운로드 주로 이미지파일 다운로드에 사용@n
 *      다운로드 완료시 sigal @sa FileDownloader::signal_downloading(qint64,qint64) 발생 @n
 *      @sa FileDownloader::downloadedData() 함수를 이용해 다운로드 결과 확인(QByteArray) @n
 *      참고 https://wiki.qt.io/Download_Data_from_URL/ko
 */
class FileDownloader : public QObject
{
    Q_OBJECT
public:
    FileDownloader(QObject *parent = nullptr);
    explicit FileDownloader(QUrl imageUrl, QObject *parent = nullptr);
    virtual ~FileDownloader();
    QByteArray downloadedData() const;
    //void setImageURL(QUrl p_imageUrl);

signals:
    void downloaded();
    void startDownLoad(QUrl p_imageUrl);
    void signal_downloading(qint64, qint64);

public slots:
    void setImageURL(QUrl p_imageUrl);

private slots:
    void fileDownloaded(QNetworkReply *pReply);

private:
    QNetworkAccessManager m_WebCtrl;
    QNetworkReply *reply;
    QByteArray m_DownloadedData;        // maximum 2GB
};

#endif // FILEDOWNLOADER_H
