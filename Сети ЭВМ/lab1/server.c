#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <mem.h>
#include <string.h>
#include <dos.h>
#include "ipx.h"

#define BUFFER_SIZE 504

// s_num = -1 - признак начала передачи
// 0 - передаёт кол-во значащих сегментов
// 1..n-1 - значащие сегменты
// n - завершающий сегмент передачи

typedef struct _file_segment{
        long int s_size;
        long int s_num;
} file_segment;

long int get_file_size(FILE* f){
        long int size;
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);
        return size;
};

void main(void) {
        long int file_size; //размер файла в байтах
        long int full_seg_num, seg_num; //кол-во полных сегментов, всего сегментов
        long int last_seg_size; //размер неполного последнего сегмента (если 0 то сегмента нет)
        FILE *file;
        file_segment tmp_seg;
        
        static unsigned Socket = 0x4444;
        struct ECB ServerECB;
        struct IPX_HEADER InHeader, OutHeader;
        unsigned char OutBuffer[BUFFER_SIZE];
        int i, progress_step;
        char filename[255] = "test_img.jpg";
        int value_delay = 10;
		
		clrscr();
		if (ipx_init() != 0xff) {
			printf("IPX driver not loaded!\n");
			exit(-1);
		}
		else {
			printf("IPX driver loaded!\n");
		}

        if(IPXOpenSocket(SHORT_LIVED, &Socket)) {
                printf("IPX socket open error\n");
                exit(-1);
        }else{
                printf("IPX socket %x open\n", Socket);
        };
        memset(&ServerECB, 0, sizeof(ServerECB));

        //открытие файла и рассчет количества сегментов
        file=fopen(filename,"rb+");
        if(file!=NULL){
            file_size = get_file_size(file);
			full_seg_num = file_size / (BUFFER_SIZE);
			last_seg_size = file_size % (BUFFER_SIZE);
            seg_num = full_seg_num + 1;
            printf("file size: %ld bytes\n", file_size);
            printf("full segments number: %ld\n", full_seg_num);
            printf("last segment size: %ld bytes\n", last_seg_size);
        }else{
			printf("File open error!\n");
            exit(1);
        };

        printf("Press eny key to start transmission\n");
        getch();

        clrscr();
        //Пакет "начало передачи"
        tmp_seg.s_size = 0;
        tmp_seg.s_num = -1;      
		memset(OutBuffer, 0, BUFFER_SIZE);

        //заголовок
        OutHeader.PacketType = 4;		// IPX
        memset(OutHeader.DestNetwork, 0, 4);	// локальная сеть
        memset(OutHeader.DestNode, 0xFF, 6);	// всем узлам сети
        OutHeader.DestSocket = IntSwap(Socket);
        //заполнение ECB
        ServerECB.Socket = IntSwap(Socket);
        ServerECB.FragmentCnt = 3;
        ServerECB.Packet[0].Address = &OutHeader;
        ServerECB.Packet[0].Size = sizeof(OutHeader);
        ServerECB.Packet[1].Address = OutBuffer;
        ServerECB.Packet[1].Size = BUFFER_SIZE;
        ServerECB.Packet[2].Address = &tmp_seg;
        ServerECB.Packet[2].Size = 8;
        IPXSendPacket(&ServerECB);
        printf("Start.\n");

        //Пакет размер файла
        tmp_seg.s_num = 0;
        tmp_seg.s_size = sizeof(seg_num);	
        memcpy(OutBuffer, &file_size, sizeof(file_size));
        IPXSendPacket(&ServerECB);
        
        progress_step = seg_num/70-1;
        printf("Start file transmission.\n");
		delay(100);
        for(i = 1; i <= full_seg_num; i++){
                tmp_seg.s_num = i;
                tmp_seg.s_size = BUFFER_SIZE;
                fread(OutBuffer, 1, BUFFER_SIZE, file);
                if(i % progress_step == 0) printf("%c", 219);
                IPXSendPacket(&ServerECB);
                delay(value_delay);
        };
                tmp_seg.s_num = seg_num;
                tmp_seg.s_size = last_seg_size;
                fread(OutBuffer, 1, last_seg_size, file);
                IPXSendPacket(&ServerECB);
                printf("\n%i\n", i);
        printf("Transmission is comlete\n");

        fclose(file);
        IPXCloseSocket(&Socket);
        exit(0);
}
