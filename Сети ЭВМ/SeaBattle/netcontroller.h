#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QPair>
#include <QMap>
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>

#define WSA_ERROR 1
#define SOCK_ERROR 2
#define BIND_ERROR 3
#define CONNECT_ERROR 4

#define GAME_SEARCH "SEABATTLESEARCH"
#define GAME_FOUND "SEABATTLEGAMEFOUND"
#define CHECK_PASS "SEABATTLECHECKPASS"
#define PASS_OK "SEABATTLEPASSOK"
#define WRONG_PASS "SEABATTLEWRONGPASS"
#define GAME_START "SEABATTLEGAMESTART"
#define SHOT "SEABATTLESHOT"
#define ENDGAME "SEABATTLEEND"

#define BUF_SIZE 500

enum NetStatus{NST_NONE = 0, NST_SEARCH_GAME, NST_CREATE_GAME, ST_ON_GAME};

class NetController: public QObject{
    Q_OBJECT

    unsigned short def_port;
    QString game_name, game_password;
    QMap<QString, SOCKADDR_IN> games;
    NetStatus status;
    SOCKET opponent_soc, soc_TCP, soc_UDP;
    SOCKADDR_IN opponent_addr;

public:
    NetController();
    ~NetController();
    SOCKET getSocketUDP();
    SOCKET getSocketTCP();
    void setOpponentAddr(SOCKADDR_IN addr);
    void setOpponentSocket(SOCKET s);
    SOCKET getOpponentSocket();
    int prepareMessage(QString message, QString &reply);
    void sendToFunc(SOCKET s, const char *buf, int buf_sz, SOCKADDR_IN addr);
    void sendFunc(SOCKET s, const char *buf, int buf_sz);
    void createGame(QString name, QString password);
    QVector<QPair<QString, QString>> searchLocalGames();
    void connectToGame(QString name, QString password);
    void connectToOpponent();
    void sendMyField(QString &field);
    void sendShot(QString coord);
    void addGame(QString message, SOCKADDR_IN addr);
    void closeGame(bool initiator);

private:
    void initWSA();
    void closeWSA();
    void bindFunc(SOCKET s, SOCKADDR_IN *addr);
    SOCKET socketFunc(int af, int type, int protocol);

signals:
    void netError(QString err, uint code);
    void wrongPass();
    void connectionSuccessful();
    void opponentConnected();
    void messageFromOpponent(QString type, QString body);
};

#endif // CLIENTSERVER_H
