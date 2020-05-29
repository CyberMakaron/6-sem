#include "Field.h"

Field::Field() : num(10){
    clear();
}

Field::~Field(){}

int Field::getNum(){
    return num;
}

void Field::setNum(int n){
    num = n;
}

void Field::getFieldArray(QString &field){
    for(int i = 0; i < 100; i++)
        field += (this->field[i] == CL_CLEAR) ? "0" : "1";
}

Cell Field::getCell(int x, int y){
    int n = y * 10 + x;
    if(x < 0 || y < 0 || x >= 10 || y >= 10)
        return CL_CLEAR;
    if(n >= 0 && n < field.size())
        return field[n];
    return CL_CLEAR;
}

void Field::setCell(int x, int y, Cell cell){
    int n = y * 10 + x;
    if(x >= 0 && y >= 0 && x < 10 && y < 10 && n >= 0 && n < field.size()){
        field[n] = cell;
        return;
    }
}

void Field::clear(){
    field.fill(CL_CLEAR, 100);
}
