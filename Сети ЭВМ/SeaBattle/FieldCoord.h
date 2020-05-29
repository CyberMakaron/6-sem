#pragma once
#include <QPoint>

const int MYFIELD_X = 52;
const int MYFIELD_Y = 54;
const int ENEMYFIELD_X = 515;
const int ENEMYFIELD_Y = 54;
const int FIELD_WIDTH = 322;
const int FIELD_HEIGHT = 322;

QPoint getMyFieldCoord(const QPoint& pos);
QPoint getEnemyFieldCoord(const QPoint& pos);
