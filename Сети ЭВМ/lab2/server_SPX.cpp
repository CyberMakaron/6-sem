// VS-specific
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wsipx.h>
#include <string>

// VS-specific
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int initWSA() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		cout << "WSAStartup failed with error: " << err << endl;
		cout << WSAGetLastError() << endl;
		return 1;
	}
	return 0;
}

int closeWSA() {
	int err;
	err = WSACleanup();
	if (err != 0) {
		cout << "WSACleanup failed with error: " << err << endl;
		cout << WSAGetLastError() << endl;
		return 1;
	}
	return 0;
}

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

void ReadIpxAddress(char *lpsNetnum, char *lpsNodenum){
	char buffer[3];
	int i;
	for (i=0; i < 4 ;i++){
		scanf("%2s", buffer);
		sscanf(buffer, "%X", (UCHAR *)&(lpsNetnum[i]));
	}
	scanf("%*c");
	for (i=0; i < 6 ;i++){
		scanf("%2s", buffer);
		sscanf(buffer, "%X", (UCHAR *)&(lpsNodenum[i]));
	}
}

long int get_file_size(FILE* f) {
	long int size;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	return size;
};

void sendFile(SOCKET s, const struct sockaddr FAR* saddr, FILE *f) {
	unsigned f_sz = get_file_size(f);
	char *buf = new char[f_sz];
	fread(buf, 1, f_sz, f);
	int err = connect(s, saddr, sizeof(SOCKADDR_IPX));
	if (err){
		printf("Error while connecting! %X\n", WSAGetLastError());
		cin.get();
		return;
	}
	err = send(s, buf, f_sz, 0);
	if (err){
		printf("Error while sending! %X\n", WSAGetLastError());
		cin.get();
		return;
	}
}


int main() {
	int err;
	if (initWSA())
		return 1;
	SOCKET s;
	unsigned short socketID_svr = 0x4444, socketID_clt = 0x4445;
	// Открываем сокет; SEQPACKET - последовательная передача
	// NSPROTO_SPX, очевидно, протокол SPX
	s = socket(AF_IPX, SOCK_SEQPACKET, NSPROTO_SPX);
	if (s == INVALID_SOCKET) {
		cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
		if (closeWSA())
			return 12;
		return 2;
	}

	// Создаем адрес сервера srv_adr
	SOCKADDR_IPX srv_adr, clt_adr;
	srv_adr.sa_family = AF_IPX;
	srv_adr.sa_socket = htons(socketID_svr);
	// Привязываем к сокету
	if (bind(s, (sockaddr*)& srv_adr, sizeof(SOCKADDR_IPX)) == SOCKET_ERROR){
		printf("Bind error %X\n", WSAGetLastError());
		return 10;
	}
	// Получаем адрес присвоенный bind
	int sz = sizeof(SOCKADDR_IPX);
	getsockname(s, (sockaddr*)& srv_adr, &sz);

	printf("Server address: \n");
	PrintIpxAddress(srv_adr.sa_netnum, srv_adr.sa_nodenum);
	printf("Socket: %X\n", srv_adr.sa_socket);
	
	// // Получим по электронному баллу
	// clt_adr.sa_family = AF_IPX;
	// clt_adr.sa_socket = htons(socketID_clt);
	// ReadIpxAddress(clt_adr.sa_netnum, clt_adr.sa_nodenum);

	// printf("Client address: \n");
	// PrintIpxAddress(clt_adr.sa_netnum, clt_adr.sa_nodenum);
	// printf("Socket: %X\n", clt_adr.sa_socket);
	// broadcast невозможно использовать с SPX
	/*int set_broadcast = 1;
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)& set_broadcast, sizeof(set_broadcast));*/

	cout << "Press any key to start transmission" << endl;
	getchar();

	cout << "Listening...\n";
	if ( listen( s, SOMAXCONN ) == SOCKET_ERROR ) {
		printf( "Listen failed with error: %ld\n", WSAGetLastError() );
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "Incoming message\n";
	SOCKET client_socket = INVALID_SOCKET;
	// Accept a client socket
	client_socket = accept(s, NULL, NULL);
	if (client_socket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}
	cout << "Accepted\n";
	// После того как мы получили сокет client_sock, надо ответвиться другим тредом
	// В этом другом треде нужно работать с клиентом, а здесь - продолжать слушать
	#define BUFLEN 256

	char recvbuf[BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = BUFLEN;

	// Receive until the peer shuts down the connection
	do {
		iResult = recv(client_socket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
		} else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(client_socket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);
	bool file_found = true;

	FILE *f = fopen(recvbuf, "rb");
	if (f == NULL) {
		cout << "Can't find file \"" << recvbuf << "\"" << endl;
		file_found = false;
	}
	int f_sz = get_file_size(f);
	char *f_buf = new char[f_sz];
	fread(f_buf, 1, f_sz, f);

	if (file_found){
		iSendResult = send(client_socket, "Ok", BUFLEN, 0);
	}else{
		iSendResult = send(client_socket, "File not found", BUFLEN, 0);
	}
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}
	if (file_found){
		iSendResult = send(client_socket, f_buf, f_sz, 0);
	}
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}
	printf("Bytes sent: %d\n", iSendResult);
	// Конец треда работы с клиентами
	
	err = closesocket(s);
	if (err == SOCKET_ERROR) {
		cout << "Socket closure failed with error: " << WSAGetLastError() << endl;
		if (closeWSA())
			return 12;
		return 2;
	}

	if (closeWSA())
		return 1;
	cout << "Translation complete";
	getchar();
	return 0;
}
