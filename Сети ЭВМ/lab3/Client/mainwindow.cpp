#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void sendMessage(QString message, SOCKET s, SOCKADDR_IN srv_addr){
    char buf[BUF_SIZE];
    strcpy(buf, message.toStdString().c_str());
    qDebug() << "Байт отправлено: " << sendto(s, buf, message.length(), 0, (sockaddr*)&srv_addr, sizeof(srv_addr));
}

DWORD WINAPI listenServer(CONST LPVOID lpParam){
    MainWindow *w = (MainWindow*)lpParam;
    int addr_size = sizeof(SOCKADDR_IN);
    int iResult;
    char buf[BUF_SIZE];
    SOCKADDR_IN svr;
    while(1){
        iResult = recvfrom(w->s, buf, BUF_SIZE, 0, (sockaddr*)&svr, &addr_size);//(SOCKADDR*)(&w->srv_adr)
        if ( iResult > 0 )
            qDebug() << "Bytes received: " << iResult;
        else if ( iResult == 0 )
            qDebug() << "Connection closed";
        else
            qDebug() << "recv failed with error: " << WSAGetLastError();
        w->showMessage(QString(buf));
    }
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
    s = socket(AF_INET, SOCK_DGRAM,  IPPROTO_IP);//IPPROTO_UDP);
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
    //clt_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Привязываем к сокету
    if (bind(s, (sockaddr*)& clt_adr, addr_size) == SOCKET_ERROR){
        qDebug() << "Bind error: " << WSAGetLastError();
        return 10;
    }
    // Получаем адрес присвоенный bind
    getsockname(s, (sockaddr*)& clt_adr, &addr_size);

    //Вводим данные сервера
    srv_adr.sin_family = AF_INET;
    //memset(&srv_adr.sin_addr, 0, 4); // этот компьютер
    srv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    srv_adr.sin_port = htons(socketID_srv);
    showInetInfo(QString((inet_ntoa(srv_adr.sin_addr))), QString::number(htons(srv_adr.sin_port), 16), QString((inet_ntoa(clt_adr.sin_addr))), QString::number(htons(clt_adr.sin_port), 16));
    CreateThread(NULL, 0, &listenServer, this, 0, NULL);
    return 0;
}

void MainWindow::showMessage(QString input_message){
    QString message;
    message += "\tFrom server :\n" + input_message + "\n\n\n";
    ui->textBrowser->append(message);
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
    sendMessage(ui->lineEdit_messageToSrv->text(), s, srv_adr);
    ui->lineEdit_messageToSrv->clear();
}
