#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <mem.h>
#include <string.h>
#include "ipx.h"


#define BUFFER_SIZE 504

typedef struct _file_segment
{
        long int s_size;
        long int s_num;
} file_segment;

void main(void) {
    long int file_size;
    long int seg_num;
    FILE* file;
    file_segment tmp_seg;
    int progress_step;
	int i, k;
    static unsigned Socket = 0x4444;
    struct ECB RxECB;
    struct IPX_HEADER RxHeader;
    unsigned char RxBuffer[BUFFER_SIZE];

	char fname[9] = "img.jpg";
	file = fopen(fname,"wb");
    if(file == NULL){
            exit(-1);
    }

    if(ipx_init() != 0xff) {
            printf("IPX not loaded!\n"); 
			exit(-1);
	}
    if(IPXOpenSocket(SHORT_LIVED, &Socket)) {
            printf("Socket open error\n");
            exit(-1);
    };
	clrscr();

    memset(&RxECB, 0, sizeof(RxECB));
    RxECB.Socket = IntSwap(Socket);
    RxECB.FragmentCnt= 3;
    RxECB.Packet[0].Address = &RxHeader;
    RxECB.Packet[0].Size = sizeof(RxHeader);
    RxECB.Packet[1].Address = RxBuffer;
    RxECB.Packet[1].Size = BUFFER_SIZE;
    RxECB.Packet[2].Address = &tmp_seg;
    RxECB.Packet[2].Size = 8;
    
    //ожидание первого пакета
    IPXListenForPacket(&RxECB);
    printf("Await server response");
	while (RxECB.InUse);
	printf("\nResponse accepted.\n");
	if (tmp_seg.s_num != -1) exit(2);
	//ожидание пакета с размером файла
    IPXListenForPacket(&RxECB);
	while (RxECB.InUse);
	if (tmp_seg.s_num != 0) exit(2);
    memcpy(&file_size, RxBuffer, sizeof(file_size));
	seg_num = file_size / BUFFER_SIZE + 1;
    printf("file size: %ld byte; number of segments: %ld\n", file_size, seg_num);
	
	progress_step = seg_num/70-1;
    for(i = 0; i < seg_num; i++){
            IPXListenForPacket(&RxECB);
			while (RxECB.InUse);
			for (k = 0; k < tmp_seg.s_size; k++) {
				fputc(RxBuffer[k], file);
			};
            if(i % progress_step == 0) 
				printf("%c", 219);
            if(tmp_seg.s_num == seg_num)
				break;
    }
	printf("\nTransmission is comlete\n");
    fclose(file);
    IPXCloseSocket(&Socket);
    exit(0);
}