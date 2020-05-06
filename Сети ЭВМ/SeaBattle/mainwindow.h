#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QString>
#include <QTime>
#include <QMessageBox>
#include "Field.h"
#include "FieldCoord.h"
#include "Images.h"
#include "Model.h"
#include "Controller.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();  

private slots:
    void redraw();
    void showGameResult();
    void on_actionStart_triggered();
    void on_actionClear_triggered();
    void on_actionRandom_triggered();    
    void on_actionCreateGame_triggered();
    void on_actionFindGame_triggered();

private:
    Ui::MainWindow *ui;

    void setStatus( const QString& status );
    void paintEvent( QPaintEvent* event );
    void mousePressEvent( QMouseEvent* ev );

    Images img;
    Status status;
    Model* model;
    Controller* controller;
};

#endif // MAINWINDOW_H
