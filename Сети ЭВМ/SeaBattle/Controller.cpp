#include "Controller.h"
#include <QDebug>

Controller::Controller(Model* model_): model( model_ ){}
Controller::~Controller(){}

void Controller::onMousePressed(const QPoint& pos, bool setShip){
    //qDebug() << model->getMyShipsNum();
    if(model->getStatus() == ST_PLACING_SHIPS){
        QPoint point = getMyFieldCoord(pos);
        if(point.x() == -1 || point.y() == -1)
            return;
        model->setMyCell(point.x(), point.y(), setShip ? CL_SHIP : CL_CLEAR);
        return;
    }

    if(model->getStatus() == ST_MAKING_STEP){
        QPoint point = getEnemyFieldCoord(pos);
        if(point.x() == -1 || point.y() == -1)
            return;
        if(model->getEnemyCell(point.x(), point.y()) == CL_SHIP){
            model->setEnemyCell(point.x(), point.y(), CL_HALF);
            markEnemySunken(point.x(), point.y());
            return;
        }
        if(model->getEnemyCell(point.x(), point.y()) == CL_DOT ||
                model->getEnemyCell(point.x(), point.y()) == CL_SUNKEN ||
                model->getEnemyCell(point.x(), point.y()) == CL_HALF)
            return;
        if(model->getEnemyCell(point.x(), point.y()) == CL_CLEAR)
            model->setEnemyCell(point.x(), point.y(), CL_DOT);
        model->setStatus(ST_WAITING_STEP);
    }
}

void Controller::doEnemyStep(){
    int x, y;
    while (true){
        x = qrand() % 10, y = qrand() % 10;
        if(model->getMyCell(x, y) == CL_CLEAR){
            model->setMyCell(x, y, CL_DOT);
            break;
        }
        if(model->getMyCell(x, y) == CL_SHIP){
            model->setMyCell(x, y, CL_HALF);
            markMySunken(x, y);
            break;
        }
    }
}

void Controller::markShip(int x, int y, bool atEnemyField){
    if (atEnemyField)
        for(int i = -1; i <= 1; i++){
            markEnemyPoint( x + 1, y + i);
            markEnemyPoint( x - 1, y + i);
            markEnemyPoint( x + i, y + 1);
            markEnemyPoint( x + i, y - 1);
        }
    else
        for(int i = -1; i <= 1; i++){
            markMyPoint( x + 1, y + i);
            markMyPoint( x - 1, y + i);
            markMyPoint( x + i, y + 1);
            markMyPoint( x + i, y - 1);
        }
    if(atEnemyField)
        model->setEnemyCell(x, y, CL_SUNKEN);
    else
        model->setMyCell(x, y, CL_SUNKEN);
}

void Controller::markMyPoint(int x, int y){
    if(model->getMyCell(x, y) == CL_CLEAR)
        model->setMyCell(x, y, CL_DOT);
}

void Controller::markEnemyPoint(int x, int y){
    if( model->getEnemyCell(x, y) == CL_CLEAR )
        model->setEnemyCell(x, y, CL_DOT);
}

void Controller::markMySunken(int x, int y){
    int r, a, b, i;
    if (model->getMyCell(x, y - 1) == CL_HALF || model->getMyCell(x, y + 1) == CL_HALF||
        model->getMyCell(x, y - 1) == CL_SHIP || model->getMyCell(x, y + 1) == CL_SHIP)
        r = 1;
        else r = 0;
    if (r == 0){
        a = x; b = x;
        while(model->getMyCell(a, y) == CL_HALF) a--;
        while(model->getMyCell(b, y) == CL_HALF) b++;
        if (model->getMyCell(a, y) == CL_SHIP || model->getMyCell(b, y) == CL_SHIP)
            return;
        for (i = a + 1; i < b; i++)
            markShip(i, y, false);
        model->setMyShipsNum(model->getMyShipsNum() - 1);
    }
    else{
        a = y; b = y;
        while(model->getMyCell(x, a) == CL_HALF) a--;
        while(model->getMyCell(x, b) == CL_HALF) b++;
        if (model->getMyCell(x, a) == CL_SHIP || model->getMyCell(x, b) == CL_SHIP)
            return;
        for (i = a + 1; i < b; i++)
            markShip(x, i, false);
        model->setMyShipsNum(model->getMyShipsNum() - 1);
    }
}

void Controller::markEnemySunken(int x, int y){
    int r, a, b, i;
    if (model->getEnemyCell(x, y - 1) == CL_HALF || model->getEnemyCell(x, y + 1) == CL_HALF ||
        model->getEnemyCell(x, y - 1) == CL_SHIP || model->getEnemyCell(x, y + 1) == CL_SHIP)
        r = 1;
        else r = 0;
    if (r == 0){
        a = x; b = x;
        while(model->getEnemyCell(a, y) == CL_HALF) a--;
        while(model->getEnemyCell(b, y) == CL_HALF) b++;
        if (model->getEnemyCell(a, y) == CL_SHIP || model->getEnemyCell(b, y) == CL_SHIP)
            return;
        for (i = a + 1; i < b; i++)
            markShip(i, y, true);
        model->setEnemyShipsNum(model->getEnemyShipsNum() - 1);
    }
    else{
        a = y; b = y;
        while(model->getEnemyCell(x, a) == CL_HALF) a--;
        while(model->getEnemyCell(x, b) == CL_HALF) b++;
        if (model->getEnemyCell(x, a) == CL_SHIP || model->getEnemyCell(x, b) == CL_SHIP)
            return;
        for (i = a + 1; i < b; i++)
            markShip(x, i, true);
        model->setEnemyShipsNum(model->getEnemyShipsNum() - 1);
    }
}


void Controller::onGameStart(){
    qDebug() << model->checkMyField();
    if(!model->checkMyField())
        return;
    randomEnemyField();
    model->setMyShipsNum(10);
    model->setEnemyShipsNum(10);
    model->setStatus(ST_MAKING_STEP);
}

void Controller::clearFields(){
    if( model->getStatus() != ST_PLACING_SHIPS )
        return;

    model->clearEnemyField();
    model->clearMyField();
}

void Controller::randomMyField(){
    if(model->getStatus() != ST_PLACING_SHIPS)
        return;

    model->clearMyField();

    for(int i = 1, k = 4; i <= 4; i++, k--)
        for(int j = 0; j < i; j++)
            placeMyShipAtRandom(k);
}

void Controller::placeMyShipAtRandom(int size){
    int p;
    int q;
    bool r;
    bool isOk = true;

    while(isOk)
    {
        p = qrand() % (10 - size + 1);
        q = qrand() % (10 - size + 1);
        r = qrand() % 2;

        for(int k = r * p + !r * q - 1; k < (r * p + !r * q + size + 1); k++)
            if(     model->getMyCell(r * k + !r * p, r * q + !r * k) == CL_SHIP ||
                    model->getMyCell(r * k + !r * (p - 1), r * (q - 1) + !r * k) == CL_SHIP ||
                    model->getMyCell(r * k + !r * (p + 1), r * (q + 1) + !r * k) == CL_SHIP)
                isOk = false;
        isOk = ! isOk;
    }

    for(int k = r * p + !r * q; k < (r * p + !r * q + size); k++)
        model->setMyCell(r * k + !r * p, r * q + !r * k, CL_SHIP);
}

void Controller::randomEnemyField(){
    if(model->getStatus() != ST_PLACING_SHIPS)
        return;

    model->clearEnemyField();

    for(int i = 1, k = 4; i <= 4; i++, k--)
        for(int j = 0; j < i; j++)
            placeEnemyShipAtRandom(k);
}

void Controller::placeEnemyShipAtRandom(int size){
    int p;
    int q;
    bool r;
    bool isOk = true;

    while(isOk)
    {
        p = qrand() % (10 - size + 1);
        q = qrand() % (10 - size + 1);
        r = qrand() % 2;

        for(int k = r * p + !r * q - 1; k < (r * p + !r * q + size + 1); k++)
            if(     model->getEnemyCell(r * k + !r * p, r * q + !r * k) == CL_SHIP ||
                    model->getEnemyCell(r * k + !r * (p - 1), r * (q - 1) + !r * k) == CL_SHIP ||
                    model->getEnemyCell(r * k + !r * (p + 1), r * (q + 1) + !r * k) == CL_SHIP)
                isOk = false;
        isOk = ! isOk;
    }

    for(int k = r * p + !r * q; k < (r * p + !r * q + size); k++)
        model->setEnemyCell(r * k + !r * p, r * q + !r * k, CL_SHIP);
}

Status Controller::getStatus() const{
    return model->getStatus();
}

void Controller::setStatus(Status st){
    model->setStatus(st);
}

QImage Controller::myFieldImage(Images& img){
    return getFieldImage(img, false);
}

QImage Controller::enemyFieldImage(Images& img){
    return getFieldImage(img, true);
}

QImage Controller::getFieldImage(Images& img, bool atEnemyField){
    QImage image(FIELD_WIDTH, FIELD_HEIGHT, QImage::Format_ARGB32);
    Cell cell;
    image.fill(0);
    QPainter painter(&image);
    double cfx = 1.0 * FIELD_WIDTH / 10.0;
    double cfy = 1.0 * FIELD_HEIGHT / 10.0;
    for(int i = 0; i < 10; i++)
        for(int j = 0; j < 10; j++){
            if(atEnemyField)
                cell = model->getEnemyCell(i, j);
            else
                cell = model->getMyCell(i, j);

            int ship_size;
            ShipDisplayMode mode = model->getDisplayMode(ship_size, i, j, atEnemyField);
            if (mode != SDM_NONE && ((atEnemyField && cell == CL_SUNKEN) || !atEnemyField)){
                QImage ship_img(img.get("ship_" + QString::number(ship_size)));
                if (mode == SDM_VERTICAL){
                    QMatrix matr;
                    matr.rotate(90.f);
                    painter.drawImage(i * cfx, j * cfy, ship_img.transformed(matr));
                } else
                    painter.drawImage(i * cfx, j * cfy, ship_img);
            }

            switch(cell){
            case CL_DOT:
                    painter.drawImage(i * cfx, j * cfy, img.get("dot"));
                break;
            case CL_HALF:
            case CL_SUNKEN:
                painter.drawImage(i * cfx, j * cfy, img.get("half"));
                break;
            case CL_SHIP:
                if (!atEnemyField && model->getStatus() == ST_PLACING_SHIPS)
                    painter.drawImage(i * cfx, j * cfy, img.get("full"));
                break;
            }
        }
    return image;
}

GameResult Controller::checkGameResult(){
    if (model->getMyShipsNum() == 0)
        return GR_LOST;
    if (model->getEnemyShipsNum() == 0)
        return GR_WON;
    return GR_NONE;
}
