#include "Images.h"

Images::Images(): isLoaded_(false){}

void Images::load(){
    if( isLoaded_ )
        return;

    images.insert("field", QImage(":/img/images/background.png"));
    images.insert("dot", QImage(":/img/images/dot.png"));
    images.insert("full", QImage(":/img/images/full.png"));
    images.insert("half", QImage(":/img/images/half.png"));
    images.insert("redhalf", QImage(":/img/images/redhalf.png"));
    images.insert("redfull", QImage(":/img/images/redfull.png"));
    isLoaded_ = true;
}

QImage& Images::get(const QString& imgName)
{
    QMap<QString, QImage>::iterator i = images.find(imgName);

    if(i == images.end())
        throw 1;

    return i.value();
}

bool Images::isLoaded()
{
    return isLoaded_;
}
