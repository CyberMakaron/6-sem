// VS-specific
 //#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wsipx.h>
#include <string>
#include <process.h>
#define BUFLEN 256
// VS-specific
//#pragma comment(lib, "ws2_32.lib")

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

long int get_file_size(FILE* f) {
	long int size;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	return size;
};

DWORD WINAPI workWithClient(CONST LPVOID lpParam){
	SOCKET client_socket = *((SOCKET*)(lpParam));
	char recvbuf[BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = BUFLEN;

	// Receive until the peer shuts down the connection
	iResult = recv(client_socket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		printf("Bytes received: %d\n", iResult);
	} else if (iResult == 0)
		printf("Connection closing...\n");
	else {
		printf("recv failed: %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		ExitThread(1);
	}
	bool file_found = true;

	cout << "Opening file " << recvbuf << endl;
	FILE *f = fopen(recvbuf, "rb");
	if (f == NULL) {
		cout << "Can't find file \"" << recvbuf << "\"" << endl;
		file_found = false;
	}
	int f_sz = file_found ? get_file_size(f) : 0;
	char *f_buf = new char[f_sz];
	if (file_found) fread(f_buf, f_sz, 1, f);

    iSendResult = send(client_socket, (char*)&f_sz, sizeof(int), 0);
    if (file_found){
        if (iSendResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(client_socket);
            WSACleanup();
            ExitThread(1);
        }
        iSendResult = send(client_socket, f_buf, f_sz, 0);
        fclose(f);
        if (iSendResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(client_socket);
            WSACleanup();
            ExitThread(1);
        }
        printf("Bytes sent: %d\n", iSendResult);
	}
	ExitThread(0);
}

DWORD WINAPI listenClients(CONST LPVOID lpParam){
	SOCKET s = *((SOCKET*)lpParam);
	SOCKET client_socket;
	SOCKADDR_IPX clt_addr;
	int sz = sizeof(clt_addr);
	// Accept a client socket
	while(1){
		client_socket = accept(s, (sockaddr*)&clt_addr, &sz);
		if (client_socket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(s);
			WSACleanup();
			ExitThread(1);
		}
		printf("New client accept: \n");
		printf("Client address: ");
		PrintIpxAddress(clt_addr.sa_netnum, clt_addr.sa_nodenum);
		printf("Socket: %X\n", htons(clt_addr.sa_socket));
		CreateThread(NULL, 0, &workWithClient, &client_socket, 0, NULL);
		Sleep(1);
	}
}

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
	s = socket(AF_IPX, SOCK_SEQPACKET, NSPROTO_SPX);
	if (s == INVALID_SOCKET) {
		cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
		if (closeWSA())
			return 12;
		return 2;
	}

	// Создаем адрес сервера srv_adr
	SOCKADDR_IPX srv_adr;
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
	printf("Socket: %X\n", htons(srv_adr.sa_socket));

	cout << "Listening...\n";
	if ( listen( s, SOMAXCONN ) == SOCKET_ERROR ) {
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	// поток принятия запросов клиентов
	CreateThread(NULL, 0, &listenClients, &s, 0, NULL);

	cout << "Press \'e\' to exit.." << endl;
	while (getchar() != 'e');

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
	ExitProcess(0);
}
