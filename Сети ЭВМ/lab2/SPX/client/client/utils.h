// Подключение необходимых файлов и библиотек
//#include "stdafx.h"

int WSALibInit(WSADATA *wsad);
SOCKET CreateIPXSocket( unsigned short socket_num, SOCKADDR_IPX *address );
SOCKET CreateSPXSocket( unsigned short socket_num, SOCKADDR_IPX *address );
BOOL SetBroadcast(SOCKET s, BOOL broadcast);
int SendMessage( SOCKET sock, sockaddr_ipx destaddr, char *message, u_long msg_size );
int RecvMessage( SOCKET sock, unsigned short RecvSock, sockaddr_ipx srcaddr, char *message, u_long msg_size );
void PrintIpxAddress(char *lpsNetnum, char *lpsNodenum);
void ReadIpxAddress(char *lpsNetnum, char *lpsNodenum);