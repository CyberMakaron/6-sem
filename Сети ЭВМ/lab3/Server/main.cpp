#include "mainwindow.h"
#include <QDebug>
#include <QApplication>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wsipx.h>
#include <process.h>
#define BUF_SIZE 500

struct to_thread{
    MainWindow &w;
    SOCKET &s;
    to_thread(MainWindow &w, SOCKET &s): w(w), s(s){}
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

void sendMessage(QString message, SOCKET s, SOCKADDR_IN clt_addr){
    QString out_message = "Your message: \"" + message + "\"";
    char buf[BUF_SIZE];
    strcpy(buf, out_message.toStdString().c_str());
    sendto(s, buf, out_message.length(), 0, (sockaddr*)&clt_addr, sizeof(clt_addr));
}

DWORD WINAPI listenClients(CONST LPVOID lpParam){
    to_thread tmp(*((to_thread*)lpParam));
    MainWindow* w = &tmp.w;
    SOCKET* s = &tmp.s;
    SOCKADDR_IN clt_adr;
    int addr_size = sizeof(SOCKADDR_IN);
    int iResult;
    char buf[BUF_SIZE];
    while(1){
        iResult = recvfrom(*s, buf, BUF_SIZE, 0, (SOCKADDR*)&clt_adr, &addr_size);
        if ( iResult > 0 )
            qDebug() << "Bytes received: " << iResult;
        else if ( iResult == 0 )
            qDebug() << "Connection closed";
        else
            qDebug() << "recv failed with error: " << WSAGetLastError();
        w->showMessage(QString(buf), htons(clt_adr.sin_port));
        sendMessage(QString(buf), *s, clt_adr);
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
    s = socket(AF_INET, SOCK_DGRAM,  IPPROTO_IP);//IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        qDebug() << "Socket creation failed with error: " << WSAGetLastError() << endl;
        if (closeWSA())
            return 12;
        return 2;
    }
    // Создаем адрес сервера srv_adr
    SOCKADDR_IN srv_adr, clt_adr;
    int addr_size = sizeof(SOCKADDR_IN);
    srv_adr.sin_family = AF_INET;
    srv_adr.sin_port = htons(socketID_svr);
    //srv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Привязываем к сокету
    if (bind(s, (sockaddr*)& srv_adr, addr_size) == SOCKET_ERROR){
        qDebug() << "Bind error: " << WSAGetLastError();
        return 10;
    }
    // Получаем адрес присвоенный bind
    int sz = sizeof(SOCKADDR_IN);
    getsockname(s, (sockaddr*)& srv_adr, &sz);

    w.showInetInfo(QString((inet_ntoa(srv_adr.sin_addr))), QString::number(htons(srv_adr.sin_port), 16));

    to_thread tmp(w, s);
    CreateThread(NULL, 0, &listenClients, &tmp, 0, NULL);

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
