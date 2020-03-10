// server.cpp: ���������� ����� ����� ��� ����������� ����������.
//
// ���������� ������������ �����
#include "stdafx.h"
#include "utils.h"


// ���������
int server_sock_num = 0x4444;

// ������� ����������
LPSTR CurrentDir = NULL;

int _tmain(int argc, _TCHAR* argv[]){

	// ���������� ������
	SOCKET	server_socket = NULL;
	// ���������� ������
	SOCKADDR_IPX address = { 0 };
	// ������ �������-����������� ������
	char *buff = new char[1025];
	// ��� �������
	memset( buff, 0, 1024 );
	// ������� ����������
	CurrentDir = new char[256];
	// ������� ��� ...
	memset( CurrentDir, 0, 255 );
	// �������� ������� ����������
	GetCurrentDirectoryA( 255, &CurrentDir[0] );

	// ������������� ���������� Winsock
	WSADATA wsad;
	
	if ( WSALibInit(&wsad) )	{
		// ���������� ���������� ��� ����������
		printf( "%s is %s.\n", wsad.szDescription, wsad.szSystemStatus );
		
		server_socket = CreateIPXSocket(server_sock_num, &address);
		// ��������� ��� �� ���������
		if ( server_socket == INVALID_SOCKET ){
			printf("Can't open socket %i, error %li\n", server_sock_num, WSAGetLastError());
		}

		printf("Address of server:\n");
		PrintIpxAddress(address.sa_netnum, address.sa_nodenum);

		while( true ){
			// ������� ESC ������ ? - ��, ����� �� ���������
			if ( kbhit() && getch() == 0x1B ) break;

			// ������ �� ������
			int size = RecvMessage( server_socket, 0x5648, address,  buff, 1024 );
			// ���-�� ������ ?
			if ( size != SOCKET_ERROR && size != 0 )
			{
				// ������ ��������� ������� size
				buff[size] = '\0';
				// ������������� ������
				printf( "%s\r\n", buff );
				// �������� � �����
			}
			// ��� ���������� �������� ���������� ...
			Sleep( 1 );
		}
		// ��������� �����
		closesocket( server_socket );
		// �� �������� �������� �������, ��� �� ��������� ������ � winsock.dll
		WSACleanup( ); 
	}
	else{
		printf("Can't init WinSock lib!\n");
	}
	// ������� ��� ������
	delete [] CurrentDir;
	CurrentDir = NULL;
	delete [] buff;
	buff = NULL;
	// ���� ������� ������� ...
	_getch();
	// ��� !
	return 0;
}

