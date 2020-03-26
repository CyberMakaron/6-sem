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

void MainWindow::showMessage(QString input_message, unsigned port){
    QString message;
    message += "\tFrom client with port " + QString::number(port, 16) + ":\n" + input_message + "\n\n\n";
    ui->textBrowser->append(message);
}

void MainWindow::showInetInfo(QString address, QString port){
    ui->lineEdit_addr->setText(address);
    ui->lineEdit_port->setText(port);
}

