#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::showMessage(QString message){
    ui->textBrowser->append(message);
    update();
}

void MainWindow::showInetInfo(QString address, QString port){
    ui->lineEdit_addr->setText(address);
    ui->lineEdit_port->setText(port);
}

