#pragma once
#include <QMap>
#include <QImage>
#include <QString>

class Images{
private:
    QMap<QString, QImage> images;
    bool isLoaded_;
public:
    Images();
    void load();
    QImage& get(const QString& imgName);
    bool isLoaded();
};
