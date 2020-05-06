#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    img()
{
    qsrand(QTime::currentTime().msec());
    ui->setupUi(this);
    img.load();

    model = new Model;
    controller = new Controller(model);  
    this->redraw();
}

MainWindow::~MainWindow(){
    delete controller;
    delete model;
    delete ui;
}

void MainWindow::setStatus(const QString& status){
    ui->statusBar->showMessage(status);
}

void MainWindow::paintEvent(QPaintEvent* event){
    QImage image(img.get("field"));
    QPainter painter(&image);
    painter.drawImage(MYFIELD_X, MYFIELD_Y, controller->myFieldImage(img));
    painter.drawImage(ENEMYFIELD_X, ENEMYFIELD_Y, controller->enemyFieldImage(img));
    ui->gameField->setGeometry(ui->gameField->x(), ui->gameField->y(), image.width(), image.height());
    ui->gameField->setPixmap(QPixmap::fromImage(image));
    resize(minimumSizeHint());

    if(controller->checkGameResult() == GR_NONE){
        switch(controller->getStatus()){
        case ST_PLACING_SHIPS:
            setStatus("Расстановка кораблей");
            break;

        case ST_MAKING_STEP:
            setStatus("Ваш ход");
            break;

        case ST_WAITING_STEP:
            setStatus("Ход противника");
            break;
        }
    } else{
        QString messageString = controller->checkGameResult() == GR_WON ? "Победа!" : "Поражение!";
        messageString += " Для начала новой игры выберите соответствующий пункт меню.";
        setStatus(messageString);
    }
}

void MainWindow::mousePressEvent(QMouseEvent* ev){
    QPoint pos = ev->pos();
    pos.setY(pos.y() - (centralWidget()->y() + ui->gameField->y()));
    pos.setX(pos.x() - (centralWidget()->x() + ui->gameField->x()));
    controller->onMousePressed(pos, ev->button() == Qt::LeftButton);
    if (controller->getStatus() == ST_WAITING_STEP){
        controller->doEnemyStep();
        controller->setStatus(ST_MAKING_STEP);
    }
    this->update();
    showGameResult();
}

void MainWindow::on_actionStart_triggered(){
    controller->onGameStart();
    redraw();
}

void MainWindow::redraw(){
    if(controller->getStatus() == ST_PLACING_SHIPS)
        ui->actionStart->setDisabled(false);
    else
        ui->actionStart->setDisabled(true);
    this->update();
}

void MainWindow::on_actionClear_triggered(){
    controller->clearFields();
    this->update();
}

void MainWindow::showGameResult(){
    if(controller->checkGameResult() == GR_NONE)
        return;
    QString messageString = controller->checkGameResult() == GR_WON ? "Победа!" : "Поражение!";
    //this->update();
    QMessageBox::information(this, "Результат игры", messageString);
}

void MainWindow::on_actionRandom_triggered(){
    controller->randomMyField();
    this->update();
}

void MainWindow::on_actionCreateGame_triggered(){

}

void MainWindow::on_actionFindGame_triggered(){

}
