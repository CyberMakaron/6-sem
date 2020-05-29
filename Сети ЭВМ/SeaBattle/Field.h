#pragma once
#include <QVector>
#include <QString>

enum Cell {CL_CLEAR = 0, CL_SHIP, CL_DOT, CL_HALF, CL_SUNKEN};

class Field{
private:
    QVector<Cell> field;
    int num;
public:
    Field();
    ~Field();
    int getNum();
    void setNum(int n);
    void getFieldArray(QString &field);
    Cell getCell(int x, int y);
    void setCell(int x, int y, Cell cell);
    void clear();
};
