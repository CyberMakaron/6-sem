#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>

#define SERVICE_NAME "http"
#define BUF_SIZE 1024

using namespace std;

int initWSA() {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        cout << "WSAStartup failed with error: " << err << " (" << WSAGetLastError() << ")" << endl;
        return 1;
    }
    return 0;
}
int closeWSA() {
    int err;
    err = WSACleanup();
    if (err != 0) {
        cout << "WSACleanup failed with error: " << err << " (" << WSAGetLastError() << ")" << endl;
        return 1;
    }
    return 0;
}

DWORD WINAPI workWithClient(CONST LPVOID lpParam){
    SOCKET client_socket = *((SOCKET*)lpParam);
    delete (SOCKET*)lpParam;
    int iResult;
    char buf[BUF_SIZE], method[BUF_SIZE], URN[BUF_SIZE], host[BUF_SIZE];
    char fname[BUF_SIZE];
    int v1, v2, response_code;
    stringstream response, response_content;
    ifstream file;
    iResult = recv(client_socket, buf, BUF_SIZE, 0);
    if (iResult > 0) {
        buf[iResult] = '\0';
        sscanf(buf, "%s %s HTTP/%i.%i\nHost: %s", method, URN, &v1, &v2, host);
        cout << "Request from client: " << method << "; URN = " << URN << endl;
        cout << "HTTP/" << v1 << '.' << v2 << endl << "Host: " << host << endl;
    }
    else
        if (iResult == 0) cout << "Connection closing" << endl;
            else {
                cout << "recv() failed with error: " << WSAGetLastError();
                closesocket(client_socket);
                ExitThread(1);
            }
    if (strcmp(URN, "/") == 0)
        strcpy(fname, "homepage.html");
    else strcpy(fname, URN);
    if (strcmp(method, "GET") == 0 || strcmp(method, "HEAD") == 0){
        file.open(fname, ios::in);
        if(file.is_open()){
            while (file.getline(buf, BUF_SIZE)) response_content << buf;
            response_code = 200;
            response << "HTTP/1.1 200 OK\r\n"
                     << "Version: HTTP/1.1\r\n"
                     << "Content-Type: text/html; charset=utf-8\r\n"
                     << "Content-Length: " << response_content.str().length()
                     << "\r\n\r\n";
            if (strcmp(method, "GET") == 0)
                response << response_content.str();
        } else{
            response_code = 404;
            file.open("notfound.html", ios::in);
            while (file.getline(buf, BUF_SIZE)) response_content << buf;
            response << "HTTP/1.1 404 Not Found\r\n"
                     << "Version: HTTP/1.1\r\n"
                     << "Content-Type: text/html; charset=utf-8\r\n"
                     << "Content-Length: " << response_content.str().length()
                     << "\r\n\r\n"
                     << response_content.str();
        }
    }
    cout << "Request execution ended with code " << response_code << " (\'" << method << " " << URN << " )\'" << endl;
    iResult = send(client_socket, response.str().c_str(), response.str().length(), 0);
    if (iResult == SOCKET_ERROR) {
            cout << "send() failed with error: " << WSAGetLastError() << endl;
            closesocket(client_socket);
            ExitThread(2);
        }
    shutdown(client_socket, SD_BOTH);
    closesocket(client_socket);
    ExitThread(0);
}

DWORD WINAPI listenClients(CONST LPVOID lpParam){
    SOCKET listen_socket = *((SOCKET*)lpParam);
    SOCKADDR_IN clt_addr;
    int sz = sizeof(SOCKADDR_IN);
    while(1){
        SOCKET *client_socket = new SOCKET;
        *client_socket = accept(listen_socket, (sockaddr*)&clt_addr, &sz);
        if (*client_socket == INVALID_SOCKET) {
            cout << "accept() failed with error: " << WSAGetLastError() << endl;
            continue;
        }
        cout << "New client is connected!" << endl;
        cout << "Client addres: " << inet_ntoa(clt_addr.sin_addr) << endl;
        cout << "Client port: " << clt_addr.sin_port << endl;
        CreateThread(NULL, 0, &workWithClient, client_socket, 0, NULL);
    }

}

int main(){
    if (initWSA()) return 1;
    SOCKET listen_socket;
    int iResult;
    char buf[BUF_SIZE];
    addrinfo *srv_addr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;    // будет участвовать в bind
    //получаем адрес и портом
    iResult = getaddrinfo(NULL, SERVICE_NAME, &hints, &srv_addr);
    if (iResult != 0) {
        cout << "getaddrinfo() failed with error: " << iResult << " (" << WSAGetLastError() << ")" << endl;
        if (closeWSA())
            return 12;
        return 2;
    }
    listen_socket = socket(srv_addr->ai_family, srv_addr->ai_socktype, srv_addr->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        cout << "socket() failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(srv_addr);
        if (closeWSA())
            return 13;
        return 3;
    }
    // Привязываем сокет к адресу сервера
    iResult = bind(listen_socket, srv_addr->ai_addr, srv_addr->ai_addrlen);
    if (iResult == INVALID_SOCKET){
        cout << "bind() failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(srv_addr);
        if (closeWSA())
            return 14;
        return 4;
    }
    cout << "Socket created successfully!" << endl;
    cout << "Port: " << htons(((sockaddr_in*)(srv_addr->ai_addr))->sin_port) << endl;
    cout << "Address: " << inet_ntoa(((sockaddr_in*)(srv_addr->ai_addr))->sin_addr) << endl;
    /*getnameinfo(srv_addr->ai_addr, srv_addr->ai_addrlen, buf, BUF_SIZE, NULL, 0, NI_NUMERICSERV);
    cout << "Address: " << buf << endl;*/
    freeaddrinfo(srv_addr); // больше не понадобится
    cout << "Waiting for clients..." << endl;
    if (listen(listen_socket, SOMAXCONN ) == SOCKET_ERROR) {
        cout << "listen() failed with error: " << WSAGetLastError() << endl;
        closesocket(listen_socket);
        if (closeWSA())
            return 15;
        return 5;
    }
    CreateThread(NULL, 0, &listenClients, &listen_socket, 0, NULL);
    cout << "For exit press \'e\'" << endl;
    while(getchar() != 'e');

    iResult = closesocket(listen_socket);
    if (iResult == SOCKET_ERROR) {
        cout << "closesocket() failed with error: " << WSAGetLastError() << endl;
        if (closeWSA())
            return 16;
        return 6;
    }
    if (closeWSA())
        return 1;
    cout << "Serever is shut down!" << endl;
    ExitProcess(0);
}
