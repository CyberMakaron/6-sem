#include "mytextbrowser.h"

MyTextBrowser::MyTextBrowser(QWidget *parent):QTextBrowser(parent){}

QVariant MyTextBrowser::loadResource(int type, const QUrl &url) {
    if (type == QTextDocument::ImageResource && url.scheme() == QLatin1String("https")){
        if (cache.contains(url))
                return QVariant(cache[url]);
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        QNetworkRequest request;
        QEventLoop loop;
        QTimer getTimer; // let use a 10 second period for timing out the GET opn
        request.setUrl(url);
        request.setRawHeader("User-Agent", "Mozilla Firefox");
        QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
        sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
        sslConfiguration.setProtocol(QSsl::AnyProtocol);
        request.setSslConfiguration(sslConfiguration);
        // connect the timeout() signal of getTimer object to quit() slot of event loop
        QTimer::connect(&getTimer,SIGNAL(timeout()),&loop, SLOT(quit()));
        QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()));
        QNetworkReply *reply = manager->get( request );
        getTimer.start(10000); // 10000 milliSeconds wait period for get() method to work properly
        loop.exec();

        QByteArray ba = reply->readAll();
        QPixmap image;
        image.loadFromData(ba);
        delete reply;
        delete manager;
        cache.insert(url, image);
        return QVariant(image);
    }
    return QTextBrowser::loadResource(type, url);
}
