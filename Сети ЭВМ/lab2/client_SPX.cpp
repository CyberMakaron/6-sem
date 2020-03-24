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
#include <stdlib.h>

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

void receiveFile(SOCKET s, struct sockaddr FAR* saddr, FILE* f) {
	char msg[] = "Hello!\n";
	int err = connect(s, saddr, sizeof(SOCKADDR_IPX));
	if (err){
		printf("Error while connecting! %X\n", WSAGetLastError());
		cin.get();
		return;
	}
	err = send(s, msg, sizeof(msg), 0);
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
	unsigned short socketID_svr = 0x4444, socketID_clt = 0;
	// Открываем сокет; SEQPACKET - последовательная передача
	// NSPROTO_SPX - протокол SPX
	s = socket(AF_IPX, SOCK_SEQPACKET, NSPROTO_SPX);
	if (s == INVALID_SOCKET) {
		cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
		if (closeWSA())
			return 12;
		return 2;
	}

	SOCKADDR_IPX srv_adr, clt_adr;

	clt_adr.sa_family = AF_IPX;
	clt_adr.sa_socket = htons(socketID_clt);
	// Привязываем сокет s к некоторому сетевому интерфейсу
	bind(s, (sockaddr*)& clt_adr, sizeof(SOCKADDR_IPX));
	// Получаем настоящий адрес, присвоенный bind
	int sz = sizeof(SOCKADDR_IPX);
	getsockname(s, (sockaddr*)& clt_adr, &sz);

	printf("My address is: ");
	PrintIpxAddress(clt_adr.sa_netnum, clt_adr.sa_nodenum);
	printf("Socket: %X\n", htons(clt_adr.sa_socket));

	srv_adr.sa_family = AF_IPX;
	srv_adr.sa_socket = htons(socketID_svr);
	memset(srv_adr.sa_netnum, 0, 4);
	char *c = srv_adr.sa_nodenum;
	c[0] = 0x08; c[1] = 0; c[2] = 0x27; c[3] = 0xCD; c[4] = 0x06; c[5] = 0xC4;

	printf("Server address is: ");
	PrintIpxAddress(srv_adr.sa_netnum, srv_adr.sa_nodenum);
	printf("Socket: %X\n", htons(srv_adr.sa_socket));

	// Запрашиваемый файл
	string f_name;
	cout << "Enter file name to load: \n";
	cin >> f_name;
    cin.get();

	cout << " Connecting to server..." << endl;

	err = connect(s, (sockaddr*)&srv_adr, sizeof(SOCKADDR_IPX));
	if (err){
		printf("Error while connecting! %X\n", WSAGetLastError());
		cin.get();
		return 2;
	}
	err = send(s, f_name.c_str(), f_name.length()+1, 0);
	if (err == SOCKET_ERROR){
		printf("Error while sending! %X\n", WSAGetLastError());
		cin.get();
		return 3;
	}

    int iResult;
    int f_sz;

	iResult = recv(s, (char*)&f_sz, sizeof(int), 0);
    if ( iResult > 0 )
        printf("Bytes received: %d\n", iResult);
    else if ( iResult == 0 )
        printf("Connection closed\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError());
    printf("file size: %i, \n", f_sz);

    if (f_sz == 0) cout << "File not found!" << endl;
    else {
        char* recvbuf = new char[f_sz];
        iResult = recv(s, recvbuf, f_sz, 0);
        if ( iResult > 0 )
            printf("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
        char tmp[4];
        string new_file_name(itoa(htons(clt_adr.sa_socket), tmp, 16));
        FILE* f = fopen((new_file_name + f_name).c_str(), "wb");
        fwrite(recvbuf, f_sz, 1, f);
        delete recvbuf;
        fclose(f);
    }
	err = closesocket(s);
	if (err == SOCKET_ERROR) {
		cout << "Socket closure failed with error: " << WSAGetLastError() << endl;
		if (closeWSA())
			return 12;
		return 2;
	}

	if (closeWSA())
		return 1;
	cout << "Translation complete" << endl;
	getchar();
	return 0;
}
