#include "stdafx.h"
#include "utils.h"


int WSALibInit(WSADATA *wsad)
{
	memset(wsad, 0, sizeof(WSADATA));
	WORD wVersionRequested = MAKEWORD(2,0);
	return !WSAStartup(wVersionRequested, wsad);
}

BOOL SetBroadcast( SOCKET s, BOOL broadcast )
{
	BOOL optval = broadcast;
	// Записываем в состояние сокета, в дополнительные опции
	// SO_BROADCAST = broadcast
	int rc = setsockopt(s, SOL_SOCKET, SO_BROADCAST, (const char FAR*)&optval, sizeof(BOOL));
	return rc != SOCKET_ERROR;
}

// посылка сообщения
int SendMessage( SOCKET sock, sockaddr_ipx destaddr, char *message, u_long msg_size )
{
	// Здесь непонятно, зачем копировать структуру
	//sockaddr_ipx resIPX = { 0 };
	//// заполняем данными
	//resIPX.sa_family = AF_IPX;
	//resIPX.sa_socket = htons( destsock );
	//memcpy( resIPX.sa_netnum, destaddr.sa_netnum, 4 );
	//resIPX.sa_nodenum[5] = 0x00;
	// посылаем в сеть ...
	return sendto( 
			sock,
			message,
			msg_size,
			0,
			( sockaddr* )&destaddr,
			sizeof( sockaddr_ipx ) );
}

// прием сообщения
int RecvMessage( SOCKET sock, unsigned short RecvSock, sockaddr_ipx srcaddr, char *message, u_long msg_size )
{
	//// результат (предварительно ошибка)
	//int res = 0x00;
	//// объявляем структуру
	//sockaddr_ipx resIPX = { 0 };
	//// объявляем размер структуры
	//int size = 0x00;
	//// заполняем данными
	//resIPX.sa_family = AF_IPX;
	//resIPX.sa_socket = htons( RecvSock );
	//memcpy( resIPX.sa_netnum, srcaddr.sa_netnum, 4 );
	//resIPX.sa_nodenum[5] = 0x03;
	int size;
	// ioctlsocket узнает, сколько в буфере сокета доступных пакетов
	ioctlsocket( sock, FIONREAD, ( u_long* )&size );
	// Если пакетов нет, возвращаем SOCKET_ERROR
	if ( size <= 0 ) return SOCKET_ERROR;

	size = sizeof( sockaddr_ipx );
	return recvfrom( 
			sock,		// Сокер
			message,	// Буфер сообщения
			msg_size,	// Размер буфера
			0,			// Флаги
			( sockaddr* )&srcaddr,// Адрес отправителя
			&size );			  // Размер буфера адреса
}

// создание сокета
SOCKET CreateSocket( unsigned short socket_num, SOCKADDR_IPX *address, int socktype, int proto )
{
	SOCKET s = socket( PF_IPX,  socktype,  proto );
	if ( s == INVALID_SOCKET ) return INVALID_SOCKET;
	
	sockaddr_ipx addrIPX = { 0 };
	int size = sizeof( sockaddr_ipx );
	addrIPX.sa_family = AF_IPX;
	addrIPX.sa_socket = htons( socket_num );
	//addrIPX.sa_nodenum[5] = 0x00;

	// связываем существующий сокет с машиной (сетевой картой)
	if ( bind( s, ( SOCKADDR* )&addrIPX, sizeof( SOCKADDR ) ) == SOCKET_ERROR ) return SOCKET_ERROR;
	// Получаем данные, выданные после привязки
	getsockname( s, ( sockaddr* )&addrIPX, &size );
	// сохраняем
	memcpy( address, &addrIPX, sizeof( SOCKADDR_IPX ) );

	return s;
}

SOCKET CreateIPXSocket( unsigned short socket_num, SOCKADDR_IPX *address )
{
	return CreateSocket(socket_num, address, SOCK_DGRAM, NSPROTO_IPX);
}
SOCKET CreateSPXSocket( unsigned short socket_num, SOCKADDR_IPX *address )
{
	return CreateSocket(socket_num, address, SOCK_SEQPACKET, NSPROTO_SPX);
}

// Человекочитаемый адрес
void PrintIpxAddress(char *lpsNetnum, char *lpsNodenum){
	int i;
	for (i=0; i < 4 ;i++){
		printf("%02X", (UCHAR)lpsNetnum[i]);
	}
	printf(".");
	for (i=0; i < 6 ;i++){
		printf("%02X", (UCHAR) lpsNodenum[i]);
	}
	printf("\n");
}