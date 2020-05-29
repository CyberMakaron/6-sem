#include "Model.h"

Model::Model(){
    myField = new Field;
    enemyField = new Field;
    status = ST_PLACING_SHIPS;
}

Model::~Model(){
    delete myField;
    delete enemyField;
}

Cell Model::getMyCell(int x, int y) const{
    return myField->getCell(x, y);
}

void Model::setMyCell(int x, int y, Cell cell){
    myField->setCell(x, y, cell);
}

Cell Model::getEnemyCell(int x, int y) const{
    return enemyField->getCell(x, y);
}

void Model::setEnemyCell(int x, int y, Cell cell){
    enemyField->setCell(x, y, cell);
}

Status Model::getStatus() const{
    return status;
}

void Model::setStatus(Status st){
    status = st;
}

bool Model::checkMyField() const{
    return (shipNum(1) == 4 && shipNum(2) == 3 && shipNum(3) == 2 && shipNum(4) == 1);
}

int Model::shipNum(int size) const{
    int shipNumber = 0;
    for(int i = 0; i < 10; i++)
        for(int j = 0; j < 10; j++)
            if(isShip(size, i, j))
                shipNumber++;

    return shipNumber;
}

bool Model::isShip(int size, int x, int y) const{
    if(x > 0 && myField->getCell(x - 1, y) != CL_CLEAR)
        return false;
    if(y > 0 && myField->getCell(x, y - 1) != CL_CLEAR)
        return false;
    if(myField->getCell(x, y) == CL_CLEAR)
        return false;
    int tmp = x;
    int num = 0;
    while(myField->getCell(tmp, y) != CL_CLEAR && tmp < 10){
        tmp++;
        num++;
    }
    if(num == size){
        if(myField->getCell(x, y + 1) != CL_CLEAR)
            return false;
        return true;
    }
    tmp = y;
    num = 0;
    while(myField->getCell(x, tmp) != CL_CLEAR && tmp < 10){
        tmp++;
        num++;
    }
    if(num == size){
        if(myField->getCell(x + 1, y) != CL_CLEAR)
            return false;
        return true;
    }
    return false;
}

ShipDisplayMode Model::getDisplayMode(int &size, int x, int y, bool atEnemyField) const{
    Field *field = atEnemyField ? enemyField : myField;
    if(x > 0 && field->getCell(x - 1, y) != CL_CLEAR && field->getCell(x - 1, y) != CL_DOT)
        return SDM_NONE;
    if(y > 0 && field->getCell(x, y - 1) != CL_CLEAR && field->getCell(x, y - 1) != CL_DOT)
        return SDM_NONE;
    if(field->getCell(x, y) == CL_CLEAR || field->getCell(x, y) == CL_DOT)
        return SDM_NONE;
    int tmp = x;
    int num = 0;
    while(field->getCell(tmp, y) != CL_CLEAR && field->getCell(tmp, y) != CL_DOT && tmp < 10){
        tmp++;
        num++;
    }
    size = num;
    tmp = y;
    num = 0;
    while(field->getCell(x, tmp) != CL_CLEAR && field->getCell(x, tmp) != CL_DOT && tmp < 10){
        tmp++;
        num++;
    }
    if(num > size){
        size = num;
        return SDM_VERTICAL;
    } else
        return SDM_HORIZONTAL;
}

void Model::clearEnemyField(){
    enemyField->clear();
}

void Model::clearMyField(){
    myField->clear();
}

int Model::getMyShipsNum(){
    return myField->getNum();
}

int Model::getEnemyShipsNum(){
    return enemyField->getNum();
}

void Model::setMyShipsNum(int n){
    myField->setNum(n);
}

void Model::setEnemyShipsNum(int n){
    enemyField->setNum(n);
}

void Model::getMyFieldArray(QString &field){
    myField->getFieldArray(field);
}
