//#define _CRT_SECURE_NO_WARNINGS /// for Visual Studio
#include <iostream>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wsipx.h>
#include <string>

// Need to link with Ws2_32.lib (for Visual Studio)
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
	unsigned optlen = sizeof(unsigned);
	unsigned optval;
	getsockopt(s, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)& optval, (int*)&optlen);
	unsigned buf_sz = optval;
	unsigned full_packet_num = f_sz / buf_sz;
	unsigned last_packet_sz = f_sz % buf_sz;
	unsigned packet_num = full_packet_num + (last_packet_sz != 0 ? 1 : 0);
	cout << "File size: " << f_sz << " bytes" << endl;
	cout << "Max data size in packet: " << buf_sz << " bytes" << endl;
	cout << "Will be sent: " << full_packet_num << " full packets";
	if (last_packet_sz != 0)
		cout << " and 1 incomplete packet of " << last_packet_sz << " bytes";
	cout << endl;

	first_packet fp;
	fp.file_sz = f_sz;
	fp.max_buf_sz = buf_sz;
	fp.full_packet_num = full_packet_num;
	fp.last_packet_size = last_packet_sz;
	// 0-й пакет с информацией о размере файла, размерах сегментов и количестве сегментов
	sendto(s, (char*)& fp, sizeof(fp), 0, saddr, sizeof(SOCKADDR_IPX));
	// подготовка буфера для отправки
	char** buf;
	buf = new char*[packet_num];
    for (int i = 0; i < packet_num; i++)
        buf[i] = new char[buf_sz];
	for (int i = 0; i < full_packet_num; i ++)
        fread(buf[i], buf_sz, 1, f);
    if (last_packet_sz > 0)
        fread(buf[full_packet_num], last_packet_sz, 1, f);
	Sleep(10);
	// отправка файла
	float progress_step = 70.0 / packet_num;
	float step_count = 0;
	for (int i = 0; i < full_packet_num ; i ++) {
        sendto(s, buf[i], buf_sz, 0, saddr, sizeof(SOCKADDR_IPX));
        step_count += progress_step;
        while(step_count > 1){
            cout << char(219);
            step_count--;
        }
		//Sleep(1);
		int x = 1000;
		while(x--); // ожидание меньше 1 мс
	}
	if (last_packet_sz != 0){
		sendto(s, buf[full_packet_num], last_packet_sz, 0, saddr, sizeof(SOCKADDR_IPX));
		step_count += progress_step;
        while(step_count > 1){
            cout << char(219);
            step_count--;
        }
	}
	cout << endl;
    for (int i = 0; i < packet_num; i++)
        delete buf[i];
    delete buf;
}


int main() {
	int err;
	if (initWSA())
		return 1;
	SOCKET s;
	unsigned short socketID_svr = 0x4444, socketID_clt = 0x4445;
	s = socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);
	if (s == INVALID_SOCKET) {
		cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
		if (closeWSA())
			return 12;
		return 2;
	}
	SOCKADDR_IPX svr_adr, clt_adr;
	svr_adr.sa_family = AF_IPX;
	svr_adr.sa_socket = htons(socketID_svr);
	bind(s, (sockaddr*)& svr_adr, sizeof(SOCKADDR_IPX));
	clt_adr.sa_family = AF_IPX;
	clt_adr.sa_socket = htons(socketID_clt);
	memset(clt_adr.sa_netnum, 0, 4);		// локальная сеть
	memset(clt_adr.sa_nodenum, 0xFF, 6);	// широковещательный канал
	// настройка широковещательного канала
	int set_broadcast = 1;
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)& set_broadcast, sizeof(set_broadcast));

	string f_name;
	f_name = "test_img.jpg";
	FILE *f_in = fopen(f_name.c_str(), "rb");
	if (f_in == NULL) {
		cout << "Unable to open file \"" << f_name << "\"" << endl;
		return 3;
	}

	cout << "Press any key to start transmission" << endl;
	getchar();
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
