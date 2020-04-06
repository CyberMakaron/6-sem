#include "mainwindow.h"
#include "ui_mainwindow.h"

struct first_packet {
    unsigned file_sz;
};

struct to_thread{
    MainWindow* w;
    SOCKET s;
    QString fname;
    to_thread(MainWindow* w, SOCKET s, QString fname):
        w(w), s(s), fname(fname){}
};

int initWSA() {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        qDebug() << "WSAStartup failed with error: " << err << endl;
        qDebug() << WSAGetLastError();
        return 1;
    }
    return 0;
}
int closeWSA() {
    int err;
    err = WSACleanup();
    if (err != 0) {
        qDebug() << "WSACleanup failed with error: " << err;
        qDebug() << WSAGetLastError() << endl;
        return 1;
    }
    return 0;
}

void sendMessage(QString message, SOCKET s){
    char buf[BUF_SIZE];
    strcpy(buf, message.toStdString().c_str());
    qDebug() << "Байт отправлено: " << send(s, buf, message.length() + 1, 0);
}

DWORD WINAPI receiveFile(CONST LPVOID lpParam) {
    to_thread to_th = *((to_thread*)lpParam);
    unsigned f_sz;
    first_packet fp;
    int k = 0;
    // приём пакета с информацией о рамере файла и размером пакетов
    to_th.w->showMessage("Ожидание приёма файла...");
    recv(to_th.s, (char*) &fp, sizeof(fp), 0);
    f_sz = fp.file_sz;
    if (f_sz == 0) {
        to_th.w->showMessage("Файл не найден!");
        ExitThread(0);
    }
    FILE* f = fopen(to_th.fname.toStdString().c_str(), "wb");
    to_th.w->showMessage("Размер файла: " + QString::number(f_sz) + " bytes");
    //подготовка буфера приёма
    char* buf;
    buf = new char[f_sz];
    //приём файла
    k = recv(to_th.s, buf, f_sz, 0);
    // запись в файл
    to_th.w->showMessage("Запись в файл");
    for (unsigned i = 0; i < f_sz; i++)
        fputc(buf[i], f);
    to_th.w->showMessage("Приём файла окончен!");
    to_th.w->showMessage("Принято: " + QString::number(k) + " байт");
    delete buf;
    fclose(f);
    delete (to_thread*)lpParam;
    ExitThread(0);
}

int connectToServer(SOCKET s, SOCKADDR_IN srv_addr){
    return connect(s, (sockaddr*)&srv_addr, sizeof(srv_addr));
}

//-------------------------------------------------------------------------//

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitInet();
}

MainWindow::~MainWindow(){
    int err;
    err = closesocket(s);
    if (err == SOCKET_ERROR) {
        qDebug() << "Socket closure failed with error: " << WSAGetLastError();
        if (closeWSA())
            qDebug() << "Error 12";
        qDebug() << "Error 2";
    }
    if (closeWSA())
        qDebug() << "Error 1";
    qDebug() << "Client is shut down!";
    ExitProcess(0);
    delete ui;
}

int MainWindow::InitInet(){
    if (initWSA())
        return 1;
    unsigned short socketID_clt = 0x0, socketID_srv = 0x6000;
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        qDebug() << "Socket creation failed with error: " << WSAGetLastError() << endl;
        if (closeWSA())
            return 12;
        return 2;
    }
    // Создаем адрес клиента srv_adr
    int addr_size = sizeof(SOCKADDR_IN);
    clt_adr.sin_family = AF_INET;
    clt_adr.sin_port = htons(socketID_clt);
    // Привязываем к сокету
    if (bind(s, (sockaddr*)& clt_adr, addr_size) == SOCKET_ERROR){
        qDebug() << "Bind error: " << WSAGetLastError();
        return 10;
    }
    // Получаем адрес присвоенный bind
    getsockname(s, (sockaddr*)& clt_adr, &addr_size);
    showMessage(QString("Сокет " + QString::number(htons(clt_adr.sin_port), 16) + " успешно открыт!"));
    //Вводим данные сервера
    srv_adr.sin_family = AF_INET;
    srv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    srv_adr.sin_port = htons(socketID_srv);
    showInetInfo(QString((inet_ntoa(srv_adr.sin_addr))), QString::number(htons(srv_adr.sin_port), 16), QString((inet_ntoa(clt_adr.sin_addr))), QString::number(htons(clt_adr.sin_port), 16));
    int err = connectToServer(s, srv_adr);
    if (err){
        qDebug() << "Error while connecting! " << WSAGetLastError();
        return 2;
    } else showMessage("Подключение к серверу выполнено успешно!");
    return 0;
}

void MainWindow::showMessage(QString message){
    ui->textBrowser->append(message);
    update();
}

void MainWindow::showInetInfo(QString srv_adr, QString srv_port, QString clt_adr, QString clt_port){
    ui->lineEdit_addr->setText(srv_adr);
    ui->lineEdit_port->setText(srv_port);
    ui->lineEdit_cltAddr->setText(clt_adr);
    ui->lineEdit_cltPort->setText(clt_port);
}


void MainWindow::on_lineEdit_messageToSrv_returnPressed(){
    on_pushButton_sendMessage_clicked();
}

void MainWindow::on_pushButton_sendMessage_clicked(){
    QString str = ui->lineEdit_messageToSrv->text();
    sendMessage(str, s);
    ui->lineEdit_messageToSrv->clear();
    to_thread* to_th= new to_thread(this, s, str);
    CreateThread(NULL, 0, &receiveFile, to_th, 0, NULL);
}
