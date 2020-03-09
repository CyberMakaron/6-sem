#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "ipx.h"

unsigned IntSwap(unsigned i) {
        return((i>>8) | (i & 0xff)<<8);
}

int IPXOpenSocket(int SocketType, unsigned *Socket) {

        struct IPXSPX_REGS iregs;

        iregs.bx = IPX_CMD_OPEN_SOCKET;
        iregs.dx = IntSwap(*Socket);
        iregs.ax = SocketType;
        ipxspx_entry((void far *)&iregs);

        *Socket = IntSwap(iregs.dx);
        return(iregs.ax);
}

void IPXCloseSocket(unsigned *Socket) {

        struct IPXSPX_REGS iregs;

        iregs.bx = IPX_CMD_CLOSE_SOCKET;
        iregs.dx = IntSwap(*Socket);
        ipxspx_entry( (void far *)&iregs );
}

void IPXListenForPacket(struct ECB *RxECB) {

        struct IPXSPX_REGS iregs;

        iregs.es = FP_SEG((void far*)RxECB);
        iregs.si = FP_OFF((void far*)RxECB);
        iregs.bx = IPX_CMD_LISTEN_FOR_PACKET;
        ipxspx_entry( (void far *)&iregs );
}

void IPXSendPacket(struct ECB *TxECB) {

        struct IPXSPX_REGS iregs;

        iregs.es = FP_SEG((void far*)TxECB);
        iregs.si = FP_OFF((void far*)TxECB);
        iregs.bx = IPX_CMD_SEND_PACKET;
        ipxspx_entry( (void far *)&iregs );
}

void IPXRelinquishControl(void) {

        struct IPXSPX_REGS iregs;

        iregs.bx = IPX_CMD_RELINQUISH_CONTROL;
        ipxspx_entry( (void far *)&iregs );
}
