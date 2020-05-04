#pragma once
#include <QString>
#include <QObject>
#include "Field.h"

enum Status{ST_PLACING_SHIPS = 0, ST_WAITING_STEP, ST_MAKING_STEP};
enum ShipDisplayMode{SDM_NONE = 0, SDM_HORIZONTAL = 1, SDM_VERTICAL = 2};

class Model: public QObject
{
    Q_OBJECT
public:
    Model();
    ~Model();
    Status getStatus() const;
    void setStatus(Status);
    Cell getMyCell(int x, int y) const;
    void setMyCell(int x, int y, Cell cell);
    Cell getEnemyCell(int x, int y) const;
    void setEnemyCell(int x, int y, Cell cell);
    bool checkMyField() const;
    void clearEnemyField();
    void clearMyField();
    int shipNum(int size) const;
    bool isShip(int size, int x, int y) const;
    ShipDisplayMode getDisplayMode(int &size, int x, int y, bool atEnemyField) const;
    int getMyShipsNum();
    int getEnemyShipsNum();
    void setMyShipsNum(int n);
    void setEnemyShipsNum(int n);

private:
    Field* myField, *enemyField;
    Status status;
};
