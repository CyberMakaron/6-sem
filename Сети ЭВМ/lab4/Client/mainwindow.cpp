#include "mainwindow.h"
#include "ui_mainwindow.h"

struct first_packet {
    unsigned file_sz;
    unsigned max_buf_sz;
    unsigned full_packet_num;
    unsigned last_packet_size;
};

struct to_thread{
    MainWindow* w;
    SOCKET s;
    struct sockaddr FAR* saddr;
    QString fname;
    to_thread(MainWindow* w, SOCKET s, struct sockaddr FAR* saddr, QString fname):
        w(w), s(s), saddr(saddr), fname(fname){}
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

void sendMessage(QString message, SOCKET s, SOCKADDR_IN srv_addr){
    char buf[BUF_SIZE];
    strcpy(buf, message.toStdString().c_str());
    qDebug() << "Байт отправлено: " << sendto(s, buf, message.length() + 1, 0, (sockaddr*)&srv_addr, sizeof(srv_addr));
}

DWORD WINAPI receiveFile(CONST LPVOID lpParam) {
    to_thread to_th = *((to_thread*)lpParam);
    qDebug() << to_th.fname;
    unsigned f_sz, buf_sz;
    int full_packet_num, last_packet_sz, packet_num;
    first_packet fp;
    int k = 0;
    // приём пакета с информацией о рамере файла и размером пакетов
    to_th.w->showMessage("Ожидание приёма файла");
    int sz = sizeof(SOCKADDR_IN);
    recvfrom(to_th.s, (char*) &fp, sizeof(fp), 0, to_th.saddr, &sz);
    qDebug() << fp.file_sz << " " << fp.max_buf_sz;
    f_sz = fp.file_sz;
    buf_sz = fp.max_buf_sz;
    full_packet_num = fp.full_packet_num;
    last_packet_sz = fp.last_packet_size;
    packet_num = full_packet_num + (last_packet_sz != 0 ? 1 : 0);
    if (f_sz == 0) {
        to_th.w->showMessage("Файл не найден!");
        ExitThread(0);
    }
    FILE* f = fopen(to_th.fname.toStdString().c_str(), "wb");
    to_th.w->showMessage("Размер файла: " + QString::number(f_sz) + " bytes");
    //подготовка буфера приёма
    char** buf;
    buf = new char*[packet_num];
    for (int i = 0; i < packet_num; i++)
        buf[i] = new char[buf_sz];
    //приём файла
    for (int i = 0; i < full_packet_num; i++)
        k += recvfrom(to_th.s, buf[i], buf_sz, 0, to_th.saddr, &sz);
    if (last_packet_sz != 0)
        k += recvfrom(to_th.s, buf[full_packet_num], buf_sz, 0, to_th.saddr, &sz);
    // запись в файл
    to_th.w->showMessage("Запись в файл");
    for (int i = 0; i < full_packet_num; i++)
        for (unsigned k = 0; k < buf_sz; k++)
            fputc(buf[i][k], f);
    for (int k = 0; k < last_packet_sz; k++)
            fputc(buf[full_packet_num][k], f);
    to_th.w->showMessage("Приём окончен!");
    to_th.w->showMessage("Принято: " + QString::number(k) + " байт");
    for (int i = 0; i < packet_num; i++)
        delete buf[i];
    delete buf;
    fclose(f);
    delete (to_thread*)lpParam;
    ExitThread(0);
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
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
    sendMessage(str, s, srv_adr);
    ui->lineEdit_messageToSrv->clear();
    to_thread* to_th= new to_thread(this, s, (sockaddr*)&srv_adr, str);
    CreateThread(NULL, 0, &receiveFile, to_th, 0, NULL);
}
