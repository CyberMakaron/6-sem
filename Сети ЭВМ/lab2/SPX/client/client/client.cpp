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
	int max_buf_sz;
	int full_packet_num;
	int last_packet_size;
};

void receiveFile(SOCKET s, struct sockaddr FAR* saddr, FILE* f) {
	char msg[] = "Hello! My address is: 0xGTFO\n";
	// Пытаемся подключиться к заданному адресу
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
	//unsigned f_sz, buf_sz;
	//int full_packet_num, last_packet_sz, packet_num;
	//first_packet fp;
 //   char tmp;
	//// приём 0-го пакета
	//int sz = sizeof(SOCKADDR_IPX);
	//recvfrom(s, (char*) &fp, sizeof(fp), 0, saddr, &sz);
	//recvfrom(s, &tmp, 1, 0, saddr, &sz);
	//f_sz= fp.file_sz;
	//buf_sz = fp.max_buf_sz;
	//full_packet_num = fp.full_packet_num;
	//last_packet_sz = fp.last_packet_size;
	//packet_num = full_packet_num + (last_packet_sz != 0 ? 1 : 0);
	//cout << "File size: " << f_sz << " bytes" << endl;
	//cout << "Max data size in packet: " << buf_sz << " bytes" << endl;
	//cout << "Will be received: " << full_packet_num << " full packets";
	//if (last_packet_sz != 0)
	//	cout << " and 1 incomplete packet of " << last_packet_sz << " bytes";
	//cout << endl;

	////подготовка к приёму файла
	//char** buf;
	//buf = new char*[packet_num];
	//for (int i = 0; i < packet_num; i++)
 //       buf[i] = new char[buf_sz];
	//// приём файла
	//float progress_step = 70.0 / packet_num;
	//float step_count = 0;
	//for (int i = 0; i < full_packet_num; i++){
	//	recvfrom(s, buf[i], buf_sz, 0, saddr, &sz);
	//	recvfrom(s, &tmp, 1, 0, saddr, &sz);
	//	step_count += progress_step;
 //       while(step_count > 1){
 //           cout << char(219);
 //           step_count--;
 //       }
	//}
	//if (last_packet_sz != 0){
	//	recvfrom(s, buf[full_packet_num], buf_sz, 0, saddr, &sz);
	//	recvfrom(s, &tmp, 1, 0, saddr, &sz);
	//	step_count += progress_step;
 //       while(step_count > 1){
 //           cout << char(219);
 //           step_count--;
 //       }
	//}
	//cout << endl;
	//for (int i = 0; i < full_packet_num; i++)
 //       for (int k = 0; k < buf_sz; k++)
 //           fputc(buf[i][k], f);
 //   for (int k = 0; k < last_packet_sz; k++)
 //           fputc(buf[full_packet_num][k], f);
	//for (int i = 0; i < packet_num; i++)
 //       delete buf[i];
 //   delete buf;
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

	SOCKADDR_IPX srv_adr, clt_adr;
	clt_adr.sa_family = AF_IPX;
	clt_adr.sa_socket = htons(socketID_clt);
	bind(s, (sockaddr*)& clt_adr, sizeof(SOCKADDR_IPX));
	// Получаем адрес, присвоенный функцией bind
	int sz = sizeof(SOCKADDR_IPX);
	getsockname(s, (sockaddr*)& clt_adr, &sz);
	srv_adr.sa_family = AF_IPX;
	srv_adr.sa_socket = htons(socketID_svr);
	printf("My address is\n");
	PrintIpxAddress(clt_adr.sa_netnum, clt_adr.sa_nodenum);
	printf("Socket: %X\n", clt_adr.sa_socket);
	//memset(srv_adr.sa_netnum, 0, 4);		// локальная сеть
	//memset(srv_adr.sa_nodenum, 0xFF, 6);	// всем узлам сети
	printf("Enter server address:\n");
	//srv_adr.sa_netnum[0] = 'F';
	ReadIpxAddress(srv_adr.sa_netnum, srv_adr.sa_nodenum);
	srv_adr.sa_socket = socketID_svr;
	printf("You entered: \n");
	PrintIpxAddress(srv_adr.sa_netnum, srv_adr.sa_nodenum);
	printf("Socket: %X\n", srv_adr.sa_socket);
	string f_name;
	f_name = "test_img_in.jpg";
	FILE* f_out = fopen(f_name.c_str(), "wb");
	if (f_out == NULL) {
		cout << "Unable to open file \"" << f_name << "\"" << endl;
		return 3;
	}
	cout << " Waiting for start..." << endl;
	receiveFile(s, (sockaddr*)& srv_adr, f_out);
	fclose(f_out);

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

