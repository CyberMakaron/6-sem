#ifndef MYTEXTBROWSER_H
#define MYTEXTBROWSER_H

#include <QTextBrowser>
#include <QWidget>
#include <QDebug>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>

class MyTextBrowser: public QTextBrowser{
    Q_OBJECT
    QMap<QUrl, QPixmap> cache;
public:
    MyTextBrowser(QWidget *parent);
    QVariant loadResource(int type, const QUrl &url);
};

#endif // MYTEXTBROWSER_H
