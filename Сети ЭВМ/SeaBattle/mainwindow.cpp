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
    lobby_status = LS_NONE;
    connect(controller, SIGNAL(wrongPass()), this, SLOT(onWrongPass()));
    connect(controller, SIGNAL(connectionSuccessful()), this, SLOT(onConnectionSuccessful()));
    connect(controller, SIGNAL(opponentConnected()), this, SLOT(onOpponentConnected()));
    connect(controller, SIGNAL(opponentReady()), this, SLOT(onOpponentReady()));
    connect(controller, SIGNAL(opponentDisconnected()), this, SLOT(onOpponentDisconnected()));
    connect(controller, SIGNAL(gameResult(GameResult)), this, SLOT(onGameResult(GameResult)));
    update();
}

MainWindow::~MainWindow(){
    if (lobby_status == LS_NONE) closeGame(false);
        else closeGame(true);
    delete controller;
    delete model;
    delete ui;
}

void MainWindow::setStatus(const QString& status){
    ui->statusBar->showMessage(status);
}

void MainWindow::closeGame(bool initiator){
    lobby_status = LS_NONE;
    controller->closeGame(initiator);
    repaint();
}

void MainWindow::paintEvent(QPaintEvent* event){
    QImage image(img.get("field"));
    QPainter painter(&image);
    painter.drawImage(MYFIELD_X, MYFIELD_Y, controller->myFieldImage(img));
    painter.drawImage(ENEMYFIELD_X, ENEMYFIELD_Y, controller->enemyFieldImage(img));
    ui->gameField->setGeometry(ui->gameField->x(), ui->gameField->y(), image.width(), image.height());
    ui->gameField->setPixmap(QPixmap::fromImage(image));
    resize(minimumSizeHint());

    switch(lobby_status){
    case LS_NONE:
        setStatus("Создайте новую игру или подключитесь к уже существующей.");
        ui->menu_game->setDisabled(true);
        ui->menu_field->setDisabled(true);
        ui->menu_lobby->setDisabled(false);
        ui->actionStart->setDisabled(false);
        break;
    case LS_CREATED:
        ui->menu_game->setDisabled(true);
        ui->menu_field->setDisabled(true);
        ui->menu_lobby->setDisabled(true);
        setStatus("Игра создана. Ожидайте подключения второго игрока.");
        break;
    case LS_OPPONENT_READY:
    case LS_CONNECTED:
        setStatus("Расстановка кораблей.");
        ui->menu_lobby->setDisabled(true);
        ui->menu_game->setDisabled(false);
        ui->menu_field->setDisabled(false);
        break;
    case LS_WAITIN_OPPONENT:
        setStatus("Ожидание... Второй игрок расставляет корабли.");
        ui->menu_game->setDisabled(true);
        ui->menu_field->setDisabled(true);
        ui->menu_lobby->setDisabled(true);
        break;
    case LS_GAME:
        if (controller->getStatus() == ST_MAKING_STEP)
            setStatus("Ваш ход.");
        if (controller->getStatus() == ST_WAITING_STEP)
            setStatus("Ход противника.");
        ui->menu_field->setDisabled(true);
        ui->menu_lobby->setDisabled(true);
        ui->menu_game->setDisabled(false);
        ui->actionStart->setDisabled(true);
        break;
    }
}

void MainWindow::mousePressEvent(QMouseEvent* ev){
    QPoint pos = ev->pos();
    pos.setY(pos.y() - (centralWidget()->y() + ui->gameField->y()));
    pos.setX(pos.x() - (centralWidget()->x() + ui->gameField->x()));
    if (lobby_status == LS_GAME || lobby_status == LS_CONNECTED)
        controller->onMousePressed(pos, ev->button() == Qt::LeftButton);
    update();
}

void MainWindow::on_actionStart_triggered(){  
    if (!controller->gameStart()){
        QMessageBox::information(this, "", "Корабли расставлены неправильно!\nПравила расстановки:\
 4 1-палубных, 3 2-палубных, 2 3-палубных и 1 4-палубны;\nМинимальное расстояние между кораблями - 1 клетка.");
        return;
    }
    lobby_status = (lobby_status == LS_OPPONENT_READY) ? LS_GAME : LS_WAITIN_OPPONENT;
    update();
}

void MainWindow::on_actionClear_triggered(){
    controller->clearMyField();
    update();
}

void MainWindow::onGameResult(GameResult res){
    QString messageString = res == GR_WON ? "Победа!" : "Поражение!";
    QMessageBox::information(this, "Результат игры", messageString);
    closeGame(false);
}

void MainWindow::on_actionRandom_triggered(){
    controller->randomMyField();
    update();
}

void MainWindow::on_actionCreateGame_triggered(){
    NetDialog dialog;
    dialog.setMode(true);
    if (dialog.exec() == QDialog::Accepted){
        QString name, pas;
        dialog.getParametrs(name, pas);
        controller->createNetGame(name, pas);
        lobby_status = LS_CREATED;
    }
}

void MainWindow::on_actionFindGame_triggered(){
    NetDialog dialog;
    dialog.setMode(false);
    QVector<QPair<QString, QString>> table;
    table = controller->searchLocalGames();
    dialog.setTable(table);
    if (dialog.exec() == QDialog::Accepted){
        QString name, pas;
        dialog.getParametrs(name, pas);
        controller->connectToGame(name, pas);
    }
}

void MainWindow::onOpponentConnected(){
    lobby_status = LS_CONNECTED;
    QMessageBox::information(this, "", "Второй игрок подключился. Можете расставлять корабли!");
    controller->placingShips();
    repaint();
}

void MainWindow::onWrongPass(){
    QMessageBox::information(this, "", "Неверный пароль!");
    on_actionFindGame_triggered();
}

void MainWindow::onConnectionSuccessful(){
    lobby_status = LS_CONNECTED;
    QMessageBox::information(this, "", "Вы подключились к игре! Можете расставлять корабли!");
    controller->placingShips();
    repaint();
}

void MainWindow::onOpponentReady(){
    if (lobby_status == LS_WAITIN_OPPONENT) lobby_status = LS_GAME;
    else lobby_status = LS_OPPONENT_READY;
}

void MainWindow::onOpponentDisconnected(){
    lobby_status = LS_NONE;
    QMessageBox::information(this, "", "Второй игрок отключился!");
}

void MainWindow::on_actionCloseGame_triggered(){
    closeGame(true);
}
