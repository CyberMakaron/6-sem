#pragma once
#include <QObject>
#include <QPoint>
#include "Model.h"
#include "FieldCoord.h"
#include <QPainter>
#include "Images.h"
#include "netcontroller.h"

enum GameResult {GR_WON = 0, GR_LOST = -1};

class Controller: public QObject{
    Q_OBJECT
public:
    Controller(Model* model_);
    ~Controller();
    void onMousePressed(const QPoint& position, bool setShip = true);
    bool gameStart();
    void clearMyField();
    void randomMyField();
    Status getStatus() const;
    QImage myFieldImage(Images& img);
    QImage enemyFieldImage(Images& img);      
    void createNetGame(QString name, QString password);
    QVector<QPair<QString, QString>> searchLocalGames();
    void connectToGame(QString name, QString password);
    void placingShips();    
    void closeGame(bool initiator);

private:
    QImage getFieldImage(Images& img, bool atEnemyField);
    void markMySunken(int x, int y);
    void markEnemySunken(int x, int y);
    void shotFromOpponent(QString coord);
    void placeMyShipAtRandom(int size);
    void setEnemyField(QString field);
    void markShip(int x, int y, bool atEnemyField = true);
    void markMyPoint(int x, int y);
    void markEnemyPoint(int x, int y);

    Model* model;
    NetController net;
    bool I_creator;

private slots:
    void onNetError(QString err, uint code);
    void onWrongPass();
    void onConnectionSuccessful();
    void onMessageFromOpponent(QString type, QString body);
    void onOpponentConnected();

signals:
    void wrongPass();
    void connectionSuccessful();
    void opponentConnected();
    void opponentReady();
    void opponentDisconnected();
    void gameResult(GameResult res);
};
