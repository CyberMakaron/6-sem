#include "netcontroller.h"
#include <QDebug>

DWORD WINAPI listenUDP(CONST LPVOID lpParam){
    NetController *net = (NetController*)lpParam;
    SOCKET s = net->getSocketUDP();
    SOCKADDR_IN addr;
    int addr_size = sizeof(SOCKADDR_IN);
    char buf[BUF_SIZE];
    int iResult;
    while(1){
        memset(buf, 0, BUF_SIZE);
        iResult = recvfrom(s, buf, BUF_SIZE, 0, (SOCKADDR*)&addr, &addr_size);
        if (iResult == 0 || iResult == -1)
            ExitThread(0);
        qDebug() << "received (UDP):" << buf;
        QString reply;
        uint code = net->prepareMessage(QString(buf), reply);
        if (code == 0)
            net->sendToFunc(s, reply.toStdString().c_str(), reply.length(), addr);
        if (code == 1)
            net->addGame(QString(buf), addr);
        if (code == 2){
            closesocket(net->getSocketUDP());
            net->connectToOpponent();
            ExitThread(0);
        }
        if (code == 3){
            emit net->wrongPass();            
            closesocket(net->getSocketUDP());
            ExitThread(0);
        }
    }
}

DWORD WINAPI listenTCP(CONST LPVOID lpParam){
    NetController *net = (NetController*)lpParam;
    SOCKET s = net->getOpponentSocket();
    char buf[BUF_SIZE];
    int iResult;
    while(1){
        memset(buf, 0, 100);
        iResult = recv(s, buf, BUF_SIZE, 0);
        if (iResult == -1 || iResult == 0){
            ExitThread(0);
        }
        qDebug() << "received (TCP):" << buf;
        QString reply;
        net->prepareMessage(QString(buf), reply);
    }
}

DWORD WINAPI waitingOpponentThread(CONST LPVOID lpParam){
    NetController *net = (NetController*)lpParam;
    listen(net->getSocketTCP(), SOMAXCONN);
    int addr_size = sizeof(SOCKADDR_IN);
    SOCKADDR_IN opponent_addr;
    SOCKET s = accept(net->getSocketTCP(), (sockaddr*)&(opponent_addr), &addr_size);
    qDebug() << "Второй игрок подключился! Адрес игрока: " << inet_ntoa(opponent_addr.sin_addr);
    net->setOpponentSocket(s);
    emit net->opponentConnected();
    net->setOpponentAddr(opponent_addr);
    CreateThread(NULL, 0, &listenTCP, net, 0, NULL);
}

NetController::NetController(){
    def_port = 30000;
    initWSA();
}

NetController::~NetController(){
    closeGame(false);
    closeWSA();
}

SOCKET NetController::getSocketUDP(){
    return soc_UDP;
}

SOCKET NetController::getSocketTCP(){
    return soc_TCP;
}

void NetController::setOpponentAddr(SOCKADDR_IN addr){
    opponent_addr = addr;
}

void NetController::setOpponentSocket(SOCKET s){
    opponent_soc = s;
}

SOCKET NetController::getOpponentSocket(){
    return opponent_soc;
}

void NetController::sendToFunc(SOCKET s, const char *buf, int buf_sz, SOCKADDR_IN addr){
    int iResult = sendto(s, buf, buf_sz, 0, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
    if (iResult == SOCKET_ERROR){
        QString err_str = QString("sendto(%1) failed with error: ").arg(buf) + QString::number(WSAGetLastError());
        emit netError(err_str, SOCK_ERROR);
    }
    qDebug() << "sent (UDP):" << buf;
}

void NetController::sendFunc(SOCKET s, const char *buf, int buf_sz){
    int iResult = send(s, buf, buf_sz, 0);
    if (iResult == SOCKET_ERROR){
        QString err_str = QString("send(%1) failed with error: ").arg(buf) + QString::number(WSAGetLastError());
        emit netError(err_str, SOCK_ERROR);
    }
    qDebug() << "sent (TCP):" << buf;
}

void NetController::bindFunc(SOCKET s, SOCKADDR_IN *addr){
    if (bind(s, (sockaddr*) addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
        QString err_str = "Bind error: " + QString::number(WSAGetLastError());
        emit netError(err_str, BIND_ERROR);
    }
}

SOCKET NetController::socketFunc(int af, int type, int protocol){
    SOCKET soc = socket(af, type, protocol);
    if (soc == INVALID_SOCKET) {
        QString err_str = "Socket creation failed with error: " + QString::number(WSAGetLastError());
        emit netError(err_str, SOCK_ERROR);
    }
    return soc;
}

void NetController::createGame(QString name, QString password){
    status = NST_CREATE_GAME;
    game_name = name;
    game_password = password;
    soc_UDP = socketFunc(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN srv_adr;
    int addr_size = sizeof(SOCKADDR_IN);
    memset(&srv_adr, 0, addr_size);
    srv_adr.sin_family = AF_INET;
    srv_adr.sin_port = htons(def_port);
    bindFunc(soc_UDP, &srv_adr);
    CreateThread(NULL, 0, &listenUDP, this, 0, NULL);
    soc_TCP = socketFunc(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bindFunc(soc_TCP, &srv_adr);
    CreateThread(NULL, 0, &waitingOpponentThread, this, 0, NULL);
}

void NetController::addGame(QString message, SOCKADDR_IN addr){
    QString name = message.right(message.length() - message.indexOf(" "));
    games[name] = addr;
}

int NetController::prepareMessage(QString message, QString &reply){
    //code 0 - без действий; -1 - ничего не делать
    QString type = message.split(" ").at(0);
    if(type == GAME_SEARCH){
        if (status == NST_SEARCH_GAME)
            return -1;
        reply = QString(GAME_FOUND) + " " + game_name;
        return 0;
    }
    if(type == GAME_FOUND)
        return 1;
    if(type == CHECK_PASS){
        QString pass = message.split(" ").at(1);
        if(pass == game_password)
            reply = QString(PASS_OK);
        else reply = QString(WRONG_PASS);
        return 0;
    }
    if(type == PASS_OK){
        return 2;
    }
    if(type == WRONG_PASS){
        return 3;
    }
    if(type == GAME_START){
        QString field = message.split(" ").at(1);
        emit messageFromOpponent(type, field);
        return 0;
    }
    if(type == SHOT){
        QString coord = message.split(" ").at(1) + " " + message.split(" ").at(2);
        emit messageFromOpponent(type, coord);
        return 0;
    }
    if(type == ENDGAME){
        emit messageFromOpponent(type, QString());
        return 0;
    }
}

QVector<QPair<QString, QString>> NetController::searchLocalGames(){
    status = NST_SEARCH_GAME;
    games.clear();
    unsigned short port = def_port;
    soc_UDP = socketFunc(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN addr;
    int addr_size = sizeof(SOCKADDR_IN);
    memset(&addr, 0, addr_size);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    bindFunc(soc_UDP, &addr);
    SOCKADDR_IN addr1;
    memset(&addr1, 0, addr_size);
    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(port);
    addr1.sin_addr.s_addr = inet_addr("255.255.255.255");
    // настройка широковещательного канала
    int set_broadcast = 1;
    setsockopt(soc_UDP, SOL_SOCKET, SO_BROADCAST, (char*)& set_broadcast, sizeof(set_broadcast));
    CreateThread(NULL, 0, &listenUDP, this, 0, NULL);
    char buf[] = GAME_SEARCH;
    sendToFunc(soc_UDP, buf, strlen(buf), addr1);
    set_broadcast = 0;
    setsockopt(soc_UDP, SOL_SOCKET, SO_BROADCAST, (char*)& set_broadcast, sizeof(set_broadcast));
    Sleep(1000);
    shutdown(soc_UDP, SD_BOTH);
    closesocket(soc_UDP);
    QList<QString> list = games.keys();
    QVector<QPair<QString, QString>> res;
    for (int i = 0; i < list.length(); i++)
        res.append(QPair<QString, QString>(list[i], QString(inet_ntoa(games[list[i]].sin_addr))));
    return res;
}

void NetController::connectToGame(QString name, QString password){
    unsigned short port = def_port;
    soc_UDP = socketFunc(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN addr;
    int addr_size = sizeof(SOCKADDR_IN);
    memset(&addr, 0, addr_size);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    bindFunc(soc_UDP, &addr);
    opponent_addr = games[name];
    CreateThread(NULL, 0, &listenUDP, this, 0, NULL);
    QString mes = QString(CHECK_PASS) + " " + password;
    sendToFunc(soc_UDP, mes.toStdString().c_str(), mes.length(), opponent_addr);
}

int connectTCP(SOCKET s, SOCKADDR_IN addr){
    return connect(s, (sockaddr*)&addr, sizeof(addr));
}

void NetController::connectToOpponent(){
    soc_TCP = socketFunc(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN addr;
    int addr_size = sizeof(SOCKADDR_IN);
    memset(&addr, 0, addr_size);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(def_port);
    bindFunc(soc_TCP, &addr);
    opponent_soc = soc_TCP;
    int err = connectTCP(soc_TCP, opponent_addr);
    CreateThread(NULL, 0, &listenTCP, this, 0, NULL);
    if (err != 0){
        QString err_str = "Connection TCP error: " + QString::number(WSAGetLastError());
        emit netError(err_str, CONNECT_ERROR);
    } else{
        qDebug() << "подключение к второму игроку выполнено успешно! Адрес игрока:" << inet_ntoa(opponent_addr.sin_addr);
        emit connectionSuccessful();
    }
}

void NetController::sendMyField(QString &field){
    QString buf = QString(GAME_START) + " " + field;
    sendFunc(opponent_soc, buf.toStdString().c_str(), buf.length());
}

void NetController::sendShot(QString coord){
    QString buf = QString(SHOT) + " " + coord;
    sendFunc(opponent_soc, buf.toStdString().c_str(), buf.length());
}

void NetController::closeGame(bool initiator){
    QString mes(ENDGAME);
    if (initiator) sendFunc(opponent_soc, mes.toStdString().c_str(), mes.length());
    shutdown(soc_TCP, SD_BOTH);
    closesocket(soc_TCP);
    shutdown(soc_UDP, SD_BOTH);
    closesocket(soc_UDP);
    shutdown(opponent_soc, SD_BOTH);
    closesocket(opponent_soc);
}

void NetController::initWSA(){
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        QString err_str = "WSAStartup failed with error: " + QString::number(err);
        err_str += " | " + QString::number(WSAGetLastError());
        emit netError(err_str, WSA_ERROR);
    }
}

void NetController::closeWSA(){
    int err;
    err = WSACleanup();
    if (err != 0) {
        QString err_str = "WSACleanup failed with error: " + QString::number(err);
        err_str += " | " + QString::number(WSAGetLastError());
        emit netError(err_str, WSA_ERROR);
    }
}
