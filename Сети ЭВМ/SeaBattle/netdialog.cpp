#include "netdialog.h"
#include "ui_netdialog.h"

NetDialog::NetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetDialog){
    ui->setupUi(this);
}

NetDialog::~NetDialog(){
    delete ui;
}

void NetDialog::setMode(bool is_create){
    if (is_create) ui->stackedWidget->setCurrentIndex(0);
    else ui->stackedWidget->setCurrentIndex(1);
}

void NetDialog::getParametrs(QString &name, QString &password){
    name = this->name;
    password = this->password;
}

void NetDialog::clearAll(){
    ui->lineEdit_name_create->clear();
    ui->lineEdit_pass_create->clear();
    ui->games_table->clear();
}

void NetDialog::setTable(QVector<QPair<QString, QString>> table){
    ui->games_table->clearContents();
    ui->games_table->setRowCount(table.length());
    QTableWidgetItem *item;
    for (int i = 0; i < table.length(); i++){
        item = new QTableWidgetItem(table[i].first);
        ui->games_table->setItem(i, 0, item);
        item = new QTableWidgetItem(table[i].second);
        ui->games_table->setItem(i, 1, item);
    }
}

void NetDialog::on_pushButton_ok_clicked(){
    if(ui->stackedWidget->currentIndex() == 0){
        name = ui->lineEdit_name_create->text();
        password = ui->lineEdit_pass_create->text();
    }
    accept();
}

void NetDialog::on_pushButton_cancel_clicked(){
    reject();
}

void NetDialog::on_pushButton_connect_clicked(){
    if (ui->games_table->currentRow() == -1){
        QMessageBox::information(this, "", "Нужно выбрать игру!");
        return;
    }
    ui->lineEdit_password_connect->clear();
    ui->stackedWidget->setCurrentIndex(2);
}

void NetDialog::on_pushButton_check_pass_clicked(){
    password = ui->lineEdit_password_connect->text();
    name = ui->games_table->itemAt(0, ui->games_table->currentRow())->text();
    accept();
}

void NetDialog::on_pushButton_connect_back_clicked(){
    ui->stackedWidget->setCurrentIndex(1);
}

void NetDialog::paintEvent(QPaintEvent *event){
    switch(ui->stackedWidget->currentIndex()){
        case 0:
            setWindowTitle("Создание игры");
            break;
        case 1:
            setWindowTitle("Поиск игры");
            break;
        case 2:
            setWindowTitle("Подключенире к игре");
            break;
    }
}
