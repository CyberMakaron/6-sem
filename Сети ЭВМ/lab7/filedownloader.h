#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject
{
    Q_OBJECT

public:
    explicit FileDownloader(QObject *parent = 0);
    virtual ~FileDownloader();    
    QByteArray downloadedData() const;
    void setUrl(QUrl url)   {this->url = url;}
    void setInThread(bool f)   {inThread = f;}

signals:
    // emits error string
    void error(QString);
    // Emits path to img on disk and id
    void downloaded();

private slots:
    void fileDownloaded();
    void onReadyRead();
public slots:
    void downloadFile();
private:
    QNetworkAccessManager *webCtrl;
    const QByteArray userAgent = "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.94 Safari/537.36";
    QByteArray m_DownloadedData;
    QUrl url;
    bool inThread = false;
};

#endif // DOWNLOADER_H
