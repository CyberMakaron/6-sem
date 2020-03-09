#define IPX_CMD_OPEN_SOCKET                     0x00
#define IPX_CMD_CLOSE_SOCKET                    0x01
#define IPX_CMD_GET_LOCAL_TARGET                0x02
#define IPX_CMD_SEND_PACKET                     0x03
#define IPX_CMD_LISTEN_FOR_PACKET               0x04
#define IPX_CMD_SCHEDULE_IPX_EVENT              0x05
#define IPX_CMD_CANCEL_EVENT                    0x06
#define IPX_CMD_GET_INTERVAL_MARKER             0x08
#define IPX_CMD_GET_INTERNETWORK_ADDRESS        0x09
#define IPX_CMD_RELINQUISH_CONTROL              0x0a
#define IPX_CMD_DISCONNECT_FROM_TARGET  	0x0b

#define NO_ERRORS               0
#define ERR_NO_IPX              1
#define ERR_NO_SPX              2
#define NO_LOGGED_ON            3
#define UNKNOWN_ERROR   	0xff

#define SHORT_LIVED     0
#define LONG_LIVED      0xff
#define IPX_DATA_PACKET_MAXSIZE 546

void far    ipxspx_entry(void far *ptr);
int         ipx_init(void);

struct IPXSPX_REGS {
    unsigned int    ax;
    unsigned int    bx;
    unsigned int    cx;
    unsigned int    dx;
    unsigned int    si;
    unsigned int    di;
    unsigned int    es;
};

struct IPX_HEADER {
    unsigned int    Checksum;
    unsigned int    Length;
    unsigned char   TransportControl;
    unsigned char   PacketType;
    unsigned char   DestNetwork[4];
    unsigned char   DestNode[6];
    unsigned int    DestSocket;
    unsigned char   SourceNetwork[4];
    unsigned char   SourceNode[6];
    unsigned int    SourceSocket;
};

struct ECB {
    void far        *Link;
    void far        (*ESRAddress)(void);
    unsigned char   InUse;
    unsigned char   CCode;
    unsigned int    Socket;
    unsigned int    ConnectionId;
    unsigned int    RrestOfWorkspace;
    unsigned char   DriverWorkspace[12];
    unsigned char   ImmAddress[6];
    unsigned int    FragmentCnt;
    struct {
            void far        *Address;
            unsigned int Size;
    } Packet[3];
};

unsigned IntSwap(unsigned i);
int IPXOpenSocket(int SocketType, unsigned *Socket);
void IPXCloseSocket(unsigned *Socket);
void IPXListenForPacket(struct ECB *RxECB);
void IPXRelinquishControl(void);
void IPXSendPacket(struct ECB *TxECB);