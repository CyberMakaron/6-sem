#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    ui->arpTable->setColumnCount(3);
    ui->arpTable->setHorizontalHeaderLabels({"Адрес в Интернете", "Физический адрес", "Тип"});
    ui->arpTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->arpTable->setRowCount(1);
    getArpTable();
    ui->stackedWidget->setCurrentIndex(0);
    ui->arpTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::getArpTable(){
    int pos = 0;
    PMIB_IPNETTABLE IPARPTable = NULL;
    DWORD ActualSize = 0;
    GetIpNetTable(IPARPTable, &ActualSize, true);
    IPARPTable = (PMIB_IPNETTABLE)malloc(ActualSize);
    if (GetIpNetTable(IPARPTable, &ActualSize, true) != NO_ERROR) {
        if (IPARPTable)
            free(IPARPTable);
        return;
    }
    DWORD CurrentIndex;
    char Address[256];
    PMIB_IPADDRTABLE IPAddressTable = NULL;
    ActualSize = 0;
    GetIpAddrTable(IPAddressTable, &ActualSize, true);
    IPAddressTable = (PMIB_IPADDRTABLE)malloc(ActualSize);
    GetIpAddrTable(IPAddressTable, &ActualSize, true);
    ui->arpTable->setRowCount(ActualSize);
    CurrentIndex = -100;//индекс адаптера
    for (DWORD i = 0; i < IPARPTable->dwNumEntries; i++) {
        QString tmp;
        if (IPARPTable->table[i].dwIndex != CurrentIndex) {
            CurrentIndex = IPARPTable->table[i].dwIndex;
            IN_ADDR InAddress;
            for (DWORD j = 0; j < IPAddressTable->dwNumEntries; j++) {
                if (CurrentIndex != IPAddressTable->table[j].dwIndex)
                    continue;
                InAddress.S_un.S_addr = IPAddressTable->table[j].dwAddr;
                strcpy(Address, inet_ntoa(InAddress));
            }
            ui->arpTable->setItem(pos++, 0, new QTableWidgetItem(QString("Интерфейс: %1 --- 0x%2").arg(Address).arg(CurrentIndex, 0, 16)));
        }
        QString type = "";
        switch (IPARPTable->table[i].dwType) {
            case 1:
                type = "Другой";
                break;
            case 2:
                type = "Недопустимый";
                break;
            case 3:
                type = "Динамический";
                break;
            case 4:
                type = "Статический";
                break;
            default:
                type =  "";
        }
        ui->arpTable->setItem(pos, 2, new QTableWidgetItem(type));
        IN_ADDR InAddress;
        InAddress.S_un.S_addr = IPARPTable->table[i].dwAddr;
        ui->arpTable->setItem(pos, 0, new QTableWidgetItem(QString(inet_ntoa(InAddress))));
        QString macStr = "";
        for (int k = 0; k < 6; k++){
            if (IPARPTable->table[i].bPhysAddr[k] < 0x10){
                macStr += QString("0%1").arg(IPARPTable->table[i].bPhysAddr[k], 0, 16).toUpper();
            } else {
                macStr += QString("%1").arg(IPARPTable->table[i].bPhysAddr[k], 0, 16).toUpper();
            }
            if (k != 5) macStr += ":";
        }
        ui->arpTable->setItem(pos, 1, new QTableWidgetItem(macStr));
        pos++;
    }
    ui->arpTable->setRowCount(pos);
    free(IPARPTable);
}

void MainWindow::getMac(const char *IP) {
    DWORD ActualSize = 0;
    PMIB_IPNETTABLE IPAddressTable = NULL;
    GetIpNetTable(IPAddressTable, &ActualSize, true);
    IPAddressTable = (PMIB_IPNETTABLE)malloc(ActualSize);
    GetIpNetTable(IPAddressTable, &ActualSize, true);
    DWORD InetAddress = inet_addr(IP);
    if (InetAddress == INADDR_NONE) {
        ui->macAddress->setText("Неправильный адрес!");
        return;
    }
    bool SearchFlag = true;
    for (DWORD i = 0; i < IPAddressTable->dwNumEntries; i++) {
        if (InetAddress == IPAddressTable->table[i].dwAddr) {
            QString macStr = "";
            for (int k = 0; k < 6; k++){
                if (IPAddressTable->table[i].bPhysAddr[k] < 0x10){
                    macStr += QString("0%1").arg(IPAddressTable->table[i].bPhysAddr[k], 0, 16).toUpper();
                } else {
                    macStr += QString("%1").arg(IPAddressTable->table[i].bPhysAddr[k], 0, 16).toUpper();
                }
                if (k != 5) macStr += ":";
            }
            macStr += QString(" на интерфейсе с индексом 0x%1").arg(IPAddressTable->table[i].dwIndex, 0, 16);
            ui->macAddress->setText(macStr);
            SearchFlag = false;
        }
    }
    if (SearchFlag) ui->macAddress->setText("Соответствий не найдено!");
}

DWORD MainWindow::addIpNetEntry(const char * ip, const char * mac, unsigned iface){
    DWORD InetAddress = inet_addr(ip);
    if (InetAddress == INADDR_NONE) {
        QMessageBox::warning(this, "Ошибка!", "Введен неверный IP!");
        return 1;
    }
    MIB_IPNETROW ARPRow;
    ARPRow.dwIndex = iface;
    ARPRow.dwPhysAddrLen = 6;
    ARPRow.dwAddr = InetAddress;
    ARPRow.dwType = MIB_IPNET_TYPE_STATIC;
    switch (CreateIpNetEntry(&ARPRow)) {
    case ERROR_ACCESS_DENIED:
        QMessageBox::warning(this, "Ошибка", "Запись не добавлена, доступ запрещен!");
        break;
    case NO_ERROR:
        QMessageBox::information(this, "", "Запись добавлена!");
        break;
    default:
        QMessageBox::warning(this, "Ошибка", "Запись не добавлена!");
    }
    return 0;
}

DWORD MainWindow::dropIpNetEntry(const char * ip, unsigned iface){

    DWORD InetAddress = inet_addr(ip);
    if (InetAddress == INADDR_NONE){
        QMessageBox::warning(this, "Ошибка!", "Введен неверный IP!");
        return 1;
    }
    MIB_IPNETROW ARPRow;
    ARPRow.dwIndex = iface;
    ARPRow.dwAddr = InetAddress;
    switch (DeleteIpNetEntry(&ARPRow)) {
    case ERROR_ACCESS_DENIED:
        QMessageBox::warning(this, "Ошибка", "Запись не добавлена, доступ запрещен!");
        break;
    case NO_ERROR:
        QMessageBox::warning(this, "", "Запись добавлена!");
        break;
    default:
        QMessageBox::warning(this, "Ошибка", "Запись не добавлена!");
    }
    return 0;
}

void MainWindow::on_refreshArpTable_clicked(){
    ui->arpTable->clearContents();
    getArpTable();
}

void MainWindow::on_getMacFromIp_clicked(){
    QString IP = ui->ipAdress->text();
    if (IP == "") return;
    getMac(IP.toStdString().c_str());
}

void MainWindow::on_addEntry_clicked(){
    addIpNetEntry(
                ui->ipEdit->text().toStdString().c_str(),
                ui->macEdit->text().toStdString().c_str(),
                ui->ifaceEdit->text().toUInt(0, 16)
                );
}

void MainWindow::on_deleteEntry_clicked(){
    dropIpNetEntry(
                ui->ipDrop->text().toStdString().c_str(),
                ui->ifaceDrop->text().toUInt(0, 16)
                );
}

void MainWindow::on_pushButton_add_clicked(){
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_delete_clicked(){
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pushButton_getMAC_clicked(){
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_pushButton_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_2_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_3_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
