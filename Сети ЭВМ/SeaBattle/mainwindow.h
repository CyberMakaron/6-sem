#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QTime>
#include <QMessageBox>
#include "Field.h"
#include "FieldCoord.h"
#include "Images.h"
#include "Model.h"
#include "Controller.h"
#include "netdialog.h"

enum LobbyStatus{LS_NONE = 0, LS_CREATED, LS_CONNECTED, LS_WAITIN_OPPONENT, LS_OPPONENT_READY, LS_GAME};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();  

private slots:
    void on_actionStart_triggered();
    void on_actionClear_triggered();
    void on_actionRandom_triggered();    
    void on_actionCreateGame_triggered();
    void on_actionFindGame_triggered();
    void onOpponentConnected();
    void onWrongPass();
    void onConnectionSuccessful();
    void onOpponentReady();
    void onOpponentDisconnected();
    void onGameResult(GameResult res);
    void on_actionCloseGame_triggered();

private:
    Ui::MainWindow *ui;
    Images img;
    Model* model;
    Controller* controller;
    LobbyStatus lobby_status;

    void setStatus(const QString& status);
    void closeGame(bool initiator);
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* ev);
};

#endif // MAINWINDOW_H
