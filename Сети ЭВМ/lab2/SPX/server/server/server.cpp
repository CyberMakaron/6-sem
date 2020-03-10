// server.cpp: определ€ет точку входа дл€ консольного приложени€.
//
// ѕодключаем заголовочные файлы
#include "stdafx.h"
#include "utils.h"


// Ќастройки
int server_sock_num = 0x4444;

// текуща€ директори€
LPSTR CurrentDir = NULL;

int _tmain(int argc, _TCHAR* argv[]){

	// переменна€ сокета
	SOCKET	server_socket = NULL;
	// переменна€ адреса
	SOCKADDR_IPX address = { 0 };
	// размер приемно-передающего буфера
	char *buff = new char[1025];
	// его очистка
	memset( buff, 0, 1024 );
	// текуща€ директори€
	CurrentDir = new char[256];
	// очищаем его ...
	memset( CurrentDir, 0, 255 );
	// получаем текущую директорию
	GetCurrentDirectoryA( 255, &CurrentDir[0] );

	// »нициализаци€ библиотеки Winsock
	WSADATA wsad;
	
	if ( WSALibInit(&wsad) )	{
		// Ќапечатаем информацию про библиотеку
		printf( "%s is %s.\n", wsad.szDescription, wsad.szSystemStatus );
		
		server_socket = CreateIPXSocket(server_sock_num, &address);
		// провер€ем все ли нормально
		if ( server_socket == INVALID_SOCKET ){
			printf("Can't open socket %i, error %li\n", server_sock_num, WSAGetLastError());
		}

		printf("Address of server:\n");
		PrintIpxAddress(address.sa_netnum, address.sa_nodenum);

		while( true ){
			// клавиша ESC нажата ? - да, выход из программы
			if ( kbhit() && getch() == 0x1B ) break;

			// читаем из сокета
			int size = RecvMessage( server_socket, 0x5648, address,  buff, 1024 );
			// что-то пришло ?
			if ( size != SOCKET_ERROR && size != 0 )
			{
				// пришло сообщение размера size
				buff[size] = '\0';
				// корректировка данных
				printf( "%s\r\n", buff );
				// посылаем в сокет
			}
			// дл€ исключеин€ загрузки процессора ...
			Sleep( 1 );
		}
		// закрывает сокет
		closesocket( server_socket );
		// не забываем сообщить системе, что мы закончили работу с winsock.dll
		WSACleanup( ); 
	}
	else{
		printf("Can't init WinSock lib!\n");
	}
	// очищаем все буферы
	delete [] CurrentDir;
	CurrentDir = NULL;
	delete [] buff;
	buff = NULL;
	// ждем нажати€ клавиши ...
	_getch();
	// все !
	return 0;
}

