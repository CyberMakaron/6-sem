#include "filedownloader.h"
#include <QDebug>
#include <QEventLoop>

FileDownloader::FileDownloader(QObject *parent) :
    QObject(parent),
    webCtrl(new QNetworkAccessManager(this))
{
}

FileDownloader::~FileDownloader(){
    delete webCtrl;
}

void FileDownloader::downloadFile(){
    qDebug() << url;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", userAgent);
    QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setProtocol(QSsl::AnyProtocol);
    request.setSslConfiguration(sslConfiguration);

    QNetworkReply* reply = webCtrl->get(request);

    QObject::connect(reply, &QNetworkReply::finished, this, &FileDownloader::fileDownloaded);
    QObject::connect(reply, &QNetworkReply::readyRead, this, &FileDownloader::onReadyRead);
}

void FileDownloader::fileDownloaded(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    switch(reply->error()){
    case QNetworkReply::NoError:
        break;
    default:
        qDebug() << "Ошибка скачивания: " << reply->errorString().toLatin1();
        emit error(reply->errorString().toLatin1());
        break;
    }
    emit downloaded();
    delete reply;
    qDebug() << "кукуха поехала";
}

void FileDownloader::onReadyRead(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    m_DownloadedData = reply->readAll();
}

QByteArray FileDownloader::downloadedData() const{
    return m_DownloadedData;
}
