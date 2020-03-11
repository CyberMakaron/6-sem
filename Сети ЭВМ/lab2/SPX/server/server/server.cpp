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

// Человекочитаемый адрес
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
	unsigned max_buf_sz;
	unsigned full_packet_num;
	unsigned last_packet_size;
};

void sendFile(SOCKET s, const struct sockaddr FAR* saddr, FILE *f) {
	unsigned f_sz = get_file_size(f);
	char *buf = new char[f_sz];
	fread(buf, 1, f_sz, f);
	// Пытаемся подключиться к заданному адресу
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
	//unsigned optlen = sizeof(unsigned);
	//unsigned optval;
	//if (getsockopt(s, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)& optval, (int*)&optlen)){
	//	printf("getsockopt returned -1. Error %X\n", WSAGetLastError());
	//}
	//printf("Optval = %i\n", optval);
	//// Размер буфера для отправки
	//unsigned buf_sz = optval;
	//unsigned full_packet_num = f_sz / buf_sz;
	//unsigned last_packet_sz = f_sz % buf_sz;
	//// Общее количество пакетов
	//unsigned packet_num = full_packet_num + (last_packet_sz != 0 ? 1 : 0);
	//cout << "File size: " << f_sz << " bytes" << endl;
	//cout << "Max data size in packet: " << buf_sz << " bytes" << endl;
	//cout << "Will be sent: " << full_packet_num << " full packets";
	//if (last_packet_sz != 0)
	//	cout << " and 1 incomplete packet of " << last_packet_sz << " bytes";
	//cout << endl;

	//first_packet fp;
	//fp.file_sz = f_sz;
	//fp.max_buf_sz = buf_sz;
	//fp.full_packet_num = full_packet_num;
	//fp.last_packet_size = last_packet_sz;
	//// 0-й пакет с размером файла, максимальным размером пакета и количеством пакетов
	//sendto(s, (char*)& fp, sizeof(fp), 0, saddr, sizeof(SOCKADDR_IPX));
	//// даём клиентам время на подгготовку к приёму и подготавливаем буфер
	//char** buf;
	//buf = new char*[packet_num];
 //   for (int i = 0; i < packet_num; i++)
 //       buf[i] = new char[buf_sz];
	//for (int i = 0; i < full_packet_num; i ++)
 //       fread(buf[i], buf_sz, 1, f);
 //   if (last_packet_sz > 0)
 //       fread(buf[full_packet_num], last_packet_sz, 1, f);
	//Sleep(10);
	//// отправляем файл
	//float progress_step = 70.0 / packet_num;
	//float step_count = 0;
	//for (int i = 0; i < full_packet_num ; i ++) {
 //       sendto(s, buf[i], buf_sz, 0, saddr, sizeof(SOCKADDR_IPX));
 //       step_count += progress_step;
 //       while(step_count > 1){
 //           cout << char(219);
 //           step_count--;
 //       }
	//	//Sleep(1);
	//	int x = 1000;
	//	while(x--); // задержка меньше 1 мс
	//}
	//if (last_packet_sz != 0){
	//	sendto(s, buf[full_packet_num], last_packet_sz, 0, saddr, sizeof(SOCKADDR_IPX));
	//	step_count += progress_step;
 //       while(step_count > 1){
 //           cout << char(219);
 //           step_count--;
 //       }
	//}
	//cout << endl;
 //   for (int i = 0; i < packet_num; i++)
 //       delete buf[i];
 //   delete buf;
}


int main() {
	int err;
	if (initWSA())
		return 1;
	SOCKET s;
	unsigned short socketID_svr = 0x4444, socketID_clt = 0x4445;
	// SEQPACKET - передача последовательных пакетов
	// NSPROTO_SPX - протокол SPX
	s = socket(AF_IPX, SOCK_SEQPACKET, NSPROTO_SPX);
	if (s == INVALID_SOCKET) {
		cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
		if (closeWSA())
			return 12;
		return 2;
	}
	SOCKADDR_IPX srv_adr, clt_adr;
	srv_adr.sa_family = AF_IPX;
	srv_adr.sa_socket = htons(socketID_svr);
	if (bind(s, (sockaddr*)& srv_adr, sizeof(SOCKADDR_IPX)) == SOCKET_ERROR){
		printf("Bind error %X\n", WSAGetLastError());
		return 10;
	}
	// Получаем адрес, присвоенный функцией bind
	int sz = sizeof(SOCKADDR_IPX);
	getsockname(s, (sockaddr*)& srv_adr, &sz);
	clt_adr.sa_family = AF_IPX;
	clt_adr.sa_socket = htons(socketID_clt);
	memset(clt_adr.sa_netnum, 0, 4);		// локальная сеть
	memset(clt_adr.sa_nodenum, 0xFF, 6);	// всем узлам сети
	printf("Server address: \n");
	PrintIpxAddress(srv_adr.sa_netnum, srv_adr.sa_nodenum);
	printf("Socket: %X\n", srv_adr.sa_socket);
	printf("Client address: \n");
	PrintIpxAddress(clt_adr.sa_netnum, clt_adr.sa_nodenum);
	printf("Socket: %X\n", clt_adr.sa_socket);
	// SPX несовместим с Broadcast-пакетами
	// устанавливаем флаг для посылки широковещательных пакетов
	/*int set_broadcast = 1;
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)& set_broadcast, sizeof(set_broadcast));*/

	string f_name;
	f_name = "test_img.jpg";
	FILE *f_in = fopen(f_name.c_str(), "rb");
	if (f_in == NULL) {
		cout << "Unable to open file \"" << f_name << "\"" << endl;
		return 3;
	}

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
	// Теперь этот client_socket должен передаваться в другой поток, и там уже с ним мы должны работать
	// ДЕЛАТЬ ЭТО Я КОНЕЧНО ЖЕ НЕ БУДУ

	// Осторожно, копипаста из мсдн
	#define DEFAULT_BUFLEN 512

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(client_socket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(client_socket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(client_socket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		} else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(client_socket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);
	return 0;
	sendFile(s, (sockaddr*)& clt_adr, f_in);	// отправка файла
	fclose(f_in);

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
