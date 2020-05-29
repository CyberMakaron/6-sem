#include "Images.h"

Images::Images(): isLoaded_(false){}

void Images::load(){
    if( isLoaded_ )
        return;
    images.insert("field", QImage(":/img/images/background.png"));
    images.insert("dot", QImage(":/img/images/dot.png"));
    images.insert("full", QImage(":/img/images/full.png"));
    images.insert("half", QImage(":/img/images/half.png"));
    images.insert("ship_1", QImage(":/img/images/ship_1.png"));
    images.insert("ship_2", QImage(":/img/images/ship_2.png"));
    images.insert("ship_3", QImage(":/img/images/ship_3.png"));
    images.insert("ship_4", QImage(":/img/images/ship_4.png"));
    isLoaded_ = true;
}

QImage& Images::get(const QString& imgName){
    QMap<QString, QImage>::iterator i = images.find(imgName);
    return i.value();
}

bool Images::isLoaded(){
    return isLoaded_;
}
