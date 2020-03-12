
// VS-specific
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

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

//  
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

//  
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

struct first_packet {
	unsigned file_sz;
	int max_buf_sz;
	int full_packet_num;
	int last_packet_size;
};

void receiveFile(SOCKET s, struct sockaddr FAR* saddr, FILE* f) {
	char msg[] = "Hello! My address is: 0xGTFO\n";
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


/* Задача клиента:
1. Устанавливает соединение с сервером
2. Передает ему имя файла. Примем пакет длиной 256 байт (символов), содержащий строку в стиле C. Только имя.
3. Ждет ответ от сервера
4. Ответ от сервера тоже имеет длину 256 байт. Это может быть пакет "No file", тогда заканчиваем прием. Если "Ok", продолжаем
5. Приняв пакет, закрываем сокет
*/

int main() {
	int err;
	if (initWSA())
		return 1;

	SOCKET s;
	// Константы константочки
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

	SOCKADDR_IPX srv_adr, clt_adr;

	clt_adr.sa_family = AF_IPX;
	clt_adr.sa_socket = htons(socketID_clt);
	// Привязываем сокет s к некоторому сетевому интерфейсу
	bind(s, (sockaddr*)& clt_adr, sizeof(SOCKADDR_IPX));
	// Получаем настоящий адрес, присвоенный bind
	int sz = sizeof(SOCKADDR_IPX);
	getsockname(s, (sockaddr*)& clt_adr, &sz);
	
	printf("My address is\n");
	PrintIpxAddress(clt_adr.sa_netnum, clt_adr.sa_nodenum);
	printf("Socket: %X\n", clt_adr.sa_socket);

	// Нам необходимо знать адрес сервера
	// 2DO: узнавать адрес сервера широковещательным запросом с помощью IPX
	srv_adr.sa_family = AF_IPX;
	srv_adr.sa_socket = htons(socketID_svr);
	printf("Enter server address:\n");
	ReadIpxAddress(srv_adr.sa_netnum, srv_adr.sa_nodenum);

	printf("So, server address is: \n");
	PrintIpxAddress(srv_adr.sa_netnum, srv_adr.sa_nodenum);
	printf("Socket: %X\n", srv_adr.sa_socket);
	
	// Запрашиваемый файл
	string f_name;
	f_name = "test_img_in.jpg";

	cout << " Connecting to server..." << endl;

	// char msg[] = "Hello! My address is: 0xGTFO\n";
	int err = connect(s, saddr, sizeof(SOCKADDR_IPX));
	if (err){
		printf("Error while connecting! %X\n", WSAGetLastError());
		cin.get();
		return;
	}
	err = send(s, f_name.c_str(), f_name.length(), 0);
	if (err){
		printf("Error while sending! %X\n", WSAGetLastError());
		cin.get();
		return;
	}
	cout << "Shutdown connection" << endl;
    // shutdown the connection since no more data will be sent
	// взял функцию из сервера TCP, надо проверить

	#define DEFAULT_BUFLEN 512

    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = shutdown(s6_bytes, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(s, recvbuf, recvbuflen, 0);
		// А че дальше? в файл писать надо наверное
        if ( iResult > 0 )
            printf("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( iResult > 0 );

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

