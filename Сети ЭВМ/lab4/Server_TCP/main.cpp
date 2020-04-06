#include "mainwindow.h"
#include <QDebug>
#include <QApplication>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wsipx.h>
#include <process.h>
#include <thread>
#include <chrono>
#define BUF_SIZE 500

struct to_listening_thread{
    MainWindow &w;
    SOCKET &s;
    to_listening_thread(MainWindow &w, SOCKET &s): w(w), s(s){}
};

/*struct to_work_thread{
    MainWindow &w;
    SOCKET &s;
    SOCKADDR_IN &addr;
    char fname[BUF_SIZE];
    to_work_thread(MainWindow &w, SOCKET &s, SOCKADDR_IN &addr, char* buf): w(w), s(s), addr(addr){
        int i;
        for (i = 0; buf[i] != '\0'; i++)
            fname[i] = buf[i];
        fname[i] = '\0';
    }
};*/

struct first_packet {
    unsigned file_sz;
};

int initWSA() {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        qDebug() << "WSAStartup failed with error: " << err << endl;
        qDebug() << WSAGetLastError() << endl;
        return 1;
    }
    return 0;
}
int closeWSA() {
    int err;
    err = WSACleanup();
    if (err != 0) {
        qDebug() << "WSACleanup failed with error: " << err << endl;
        qDebug() << WSAGetLastError() << endl;
        return 1;
    }
    return 0;
}
long int get_file_size(FILE* f) {
    long int size;
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
};

first_packet getFileData(MainWindow* w, char* fname, char** buf){
    first_packet fp;
    fp.file_sz = 0;
    FILE *f = fopen(fname, "rb");
    if (f == NULL){
        w->showMessage("Файл " + QString(fname) + " не найден!");
        return fp;
    }
    long int f_sz = get_file_size(f);
    fp.file_sz = f_sz;
    *buf = new char[f_sz];
    w->showMessage("Чтение файла " + QString(fname));
        fread(*buf, f_sz, 1, f);
    fclose(f);
    return fp;
}

unsigned sendData(first_packet fp, char* data, SOCKET s){
    //0-й пакет с информацией о размере файла, размерах сегментов и количестве сегментов    
    unsigned k;
    send(s, (char*)&fp, sizeof(fp), 0);
    Sleep(10);
    // отправка файла
    k = send(s, data, fp.file_sz, 0);
    return k;
}

DWORD WINAPI workWithClient(CONST LPVOID lpParam){
    to_listening_thread to_w_th(*((to_listening_thread*)lpParam));
    MainWindow* w = &to_w_th.w;
    SOCKET s = to_w_th.s;
    int iResult;
    char message[BUF_SIZE];
    iResult = recv(s, message, BUF_SIZE, 0);
    if ( iResult > 0 )
        qDebug() << "Bytes received: " << iResult;
    else if ( iResult == 0 )
        qDebug() << "Connection closed";
    else
        qDebug() << "recv failed with error: " << WSAGetLastError();
    w->showMessage("Клиент запросил файл: " + QString(message));
    char* buf;
    first_packet fp;
    fp = getFileData(w, message, &buf);
    if(fp.file_sz != 0){
        w->showMessage("Отправка файла " + QString(message));
        unsigned k = sendData(fp, buf, s);
        delete buf;
        w->showMessage("Отправка файла " + QString(message) + " завершена!");
        w->showMessage("Размер файла: " + QString::number(fp.file_sz) + " байт");
        w->showMessage("Отправлено: " + QString::number(k) + " байт");
    } else
        //0-й пакет с информацией о размере файла
        send(s, (char*)& fp, sizeof(fp), 0);
    ExitThread(0);
}

DWORD WINAPI listenClients(CONST LPVOID lpParam){
    to_listening_thread to_th(*((to_listening_thread*)lpParam));
    MainWindow* w = &to_th.w;
    SOCKET s = to_th.s;
    SOCKADDR_IN clt_adr;
    int addr_size = sizeof(SOCKADDR_IN);
    while(1){
        SOCKET client_socket = accept(s, (sockaddr*)&clt_adr, &addr_size);
        if (client_socket == INVALID_SOCKET) {
            qDebug() << "accept failed: " << WSAGetLastError();
            continue;
        }
        w->showMessage("Соединение с клиентом " + QString::number(htons(clt_adr.sin_port)) + " успешно установлено!");
        to_listening_thread to_w_th(to_th.w, client_socket);
        CreateThread(NULL, 0, &workWithClient, &to_w_th, 0, NULL);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    if (initWSA())
        return 1;
    SOCKET s;
    unsigned short socketID_svr = 0x6000;
    s = socket(AF_INET, SOCK_STREAM,  IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        qDebug() << "Socket creation failed with error: " << WSAGetLastError() << endl;
        if (closeWSA())
            return 12;
        return 2;
    }
    // Создаем адрес сервера srv_adr
    SOCKADDR_IN srv_adr;
    int addr_size = sizeof(SOCKADDR_IN);
    srv_adr.sin_family = AF_INET;
    srv_adr.sin_port = htons(socketID_svr);
    // Привязываем к сокету
    if (bind(s, (sockaddr*)& srv_adr, addr_size) == SOCKET_ERROR){
        qDebug() << "Bind error: " << WSAGetLastError();
        return 10;
    }
    QString("Сокет " + QString::number(htons(srv_adr.sin_port), 16) + " успешно открыт!");
    // Получаем адрес присвоенный bind
    int sz = sizeof(SOCKADDR_IN);
    getsockname(s, (sockaddr*)& srv_adr, &sz);
    w.showMessage(QString("Сокет " + QString::number(htons(srv_adr.sin_port), 16) + " успешно открыт!"));

    w.showInetInfo(QString((inet_ntoa(srv_adr.sin_addr))), QString::number(htons(srv_adr.sin_port), 16));

    w.showMessage("Ожидание клиентов...");
        if ( listen( s, SOMAXCONN ) == SOCKET_ERROR ) {
            qDebug() << "Listen failed with error: " << WSAGetLastError();
            closesocket(s);
            WSACleanup();
            return 1;
        }
    to_listening_thread to_th(w, s);
    CreateThread(NULL, 0, &listenClients, &to_th, 0, NULL);
    if(!a.exec()){
        int err;
        err = closesocket(s);
            if (err == SOCKET_ERROR) {
                qDebug() << "Socket closure failed with error: " << WSAGetLastError();
                if (closeWSA())
                    return 12;
                return 2;
            }
            if (closeWSA())
                return 1;
            qDebug() << "Serever is shut down!";
            ExitProcess(0);
    }
    return 3;
}
