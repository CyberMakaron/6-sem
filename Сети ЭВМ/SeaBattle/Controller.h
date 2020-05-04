#pragma once
#include <QObject>
#include <QPoint>
#include "Model.h"
#include "FieldCoord.h"
#include <QPainter>
#include "Images.h"

enum GameResult {
    GR_NONE = 0,
    GR_WON = 1,
    GR_LOST = -1
};

class Controller{
public:
    Controller(Model* model_);
    ~Controller();
    void onMousePressed(const QPoint& position, bool setShip = true);
    void onGameStart();
    void clearFields();
    void randomMyField();
    void randomEnemyField();
    Status getStatus() const;
    void setStatus(Status st);
    void doEnemyStep();
    QImage myFieldImage(Images& img);
    QImage enemyFieldImage(Images& img);
    QImage getFieldImage(Images& img, bool atEnemyField);
    GameResult checkGameResult();
    void markMySunken(int x, int y);
    void markEnemySunken(int x, int y);

private:
    void placeMyShipAtRandom(int size);
    void placeEnemyShipAtRandom(int size);
    void markShip(int x, int y, bool atEnemyField = true);
    void markMyPoint(int x, int y);
    void markEnemyPoint(int x, int y);

    Model* model;
};
