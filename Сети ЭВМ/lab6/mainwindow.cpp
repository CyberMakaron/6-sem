#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    getDHCPInfo();
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::getDHCPInfo(){
    ui->textBrowser->clear();
    ULONG               ulAdapterOutBufLen;
    DWORD               dwRetVal;
    IP_ADAPTER_INFO  *pAdapterInfo;

    pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    ulAdapterOutBufLen = sizeof(IP_ADAPTER_INFO);
    //get size
    if (GetAdaptersInfo(pAdapterInfo, &ulAdapterOutBufLen) != ERROR_SUCCESS) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulAdapterOutBufLen);
    }
    //get info
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulAdapterOutBufLen)) != ERROR_SUCCESS) {
        qDebug() << "GetAdaptersInfo call failed with " <<  dwRetVal;
    }
    //show info
    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    ui->textBrowser->append("Adapter Name: " + QString(pAdapter->AdapterName));
    ui->textBrowser->append("Adapter Description: " + QString(pAdapter->Description));
    ui->textBrowser->append("IP Address: " + QString(pAdapter->IpAddressList.IpAddress.String));
    if (pAdapter->DhcpEnabled) {
        ui->textBrowser->append("DHCP Enabled: Yes");
        ui->textBrowser->append("DHCP Server: " + QString(pAdapter->DhcpServer.IpAddress.String));
    }
    else
         ui->textBrowser->append("DHCP Enabled: No");

    if (pAdapterInfo) {
        free(pAdapterInfo);
        pAdapterInfo = NULL;
    }
}

void MainWindow::on_pushButton_resetIP_clicked(){
    ULONG               ulInterfaceOutBufLen;
    DWORD               dwRetVal;
    IP_INTERFACE_INFO*  pInterfaceInfo;
    pInterfaceInfo = (IP_INTERFACE_INFO *)malloc(sizeof(IP_INTERFACE_INFO));
    ulInterfaceOutBufLen = sizeof(IP_INTERFACE_INFO);

    if (GetInterfaceInfo(pInterfaceInfo, &ulInterfaceOutBufLen) ==
        ERROR_INSUFFICIENT_BUFFER) {
        free(pInterfaceInfo);
        pInterfaceInfo = (IP_INTERFACE_INFO *)malloc(ulInterfaceOutBufLen);
    }

    if ((dwRetVal = GetInterfaceInfo(pInterfaceInfo, &ulInterfaceOutBufLen)) != NO_ERROR) {
        qDebug() << "  GetInterfaceInfo failed with error: " << dwRetVal;
    }

    // Убираем выданный нам адрес
    if ((dwRetVal = IpReleaseAddress(&pInterfaceInfo->Adapter[0])) == NO_ERROR) {
        QMessageBox::information(this, "", "Ip release succeeded. Now you can't use internet.");
    }

    if (pInterfaceInfo) {
        free(pInterfaceInfo);
        pInterfaceInfo = NULL;
    }
    getDHCPInfo();
}

void MainWindow::on_pushButton_getNewIP_clicked(){
    ULONG               ulInterfaceOutBufLen;
    DWORD               dwRetVal;
    IP_INTERFACE_INFO*  pInterfaceInfo;
    pInterfaceInfo = (IP_INTERFACE_INFO *)malloc(sizeof(IP_INTERFACE_INFO));
    ulInterfaceOutBufLen = sizeof(IP_INTERFACE_INFO);

    if (GetInterfaceInfo(pInterfaceInfo, &ulInterfaceOutBufLen) ==
        ERROR_INSUFFICIENT_BUFFER) {
        free(pInterfaceInfo);
        pInterfaceInfo = (IP_INTERFACE_INFO *)malloc(ulInterfaceOutBufLen);
    }

    if ((dwRetVal = GetInterfaceInfo(pInterfaceInfo, &ulInterfaceOutBufLen)) != NO_ERROR) {
        qDebug() << "  GetInterfaceInfo failed with error: " << dwRetVal;
    }

    if ((dwRetVal = IpRenewAddress(&pInterfaceInfo->Adapter[0])) == NO_ERROR) {
       QMessageBox::information(this, "", "Ip renew succeeded. Internet is back and running.");
    }

    if (pInterfaceInfo) {
        free(pInterfaceInfo);
        pInterfaceInfo = NULL;
    }
    getDHCPInfo();
}

//--------------------------------------------------------------------------------------------------------//

void MainWindow::on_pushButton_getIP_clicked(){
    WSADATA wsaData;
    int iResult;
    DWORD dwRetval;
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;
    struct sockaddr_in  *sockaddr_ipv4;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        qDebug() << "WSAStartup failed: " << iResult;
        return;
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    char hostname[NI_MAXHOST];

    strcpy(hostname, ui->lineEdit_domain->text().toStdString().c_str());
    dwRetval = getaddrinfo(hostname, 0, &hints, &result);
    if (dwRetval != 0) {
        qDebug() << "getaddrinfo failed with error: " << dwRetval;
        WSACleanup();
        return;
    }
    ui->textBrowser_IP->append(QString("IP for host ") + hostname + ":");
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        if (ptr->ai_family == AF_INET) {
            sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
            ui->textBrowser_IP->append(QString("    ") + inet_ntoa(sockaddr_ipv4->sin_addr));
        }
    }
    freeaddrinfo(result);
    WSACleanup();
}

void MainWindow::on_pushButton_getDomain_clicked(){
    WSADATA wsaData;
    int iResult;
    DWORD dwRetval;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        qDebug() << "WSAStartup failed: " << iResult;
        return;
    }
    char hostname[NI_MAXHOST];
    char servInfo[NI_MAXSERV];
    char ip[NI_MAXHOST];
    ushort port = 27015;
    struct sockaddr_in saGNI;

    strcpy(ip, ui->lineEdit_IP->text().toStdString().c_str());
    saGNI.sin_family = AF_INET;
    saGNI.sin_addr.s_addr = inet_addr(ip);
    saGNI.sin_port = htons(port);

    dwRetval = getnameinfo((struct sockaddr *) &saGNI, sizeof(struct sockaddr),
                    hostname, NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);
    if (dwRetval != 0) {
        qDebug() << "getnameinfo failed with error " << WSAGetLastError();
        return;
    }
    else {
        ui->textBrowser_Domains->append(QString("Hostname for IP  ") + ip + ":\n    "+ hostname);
        return;
    }
    WSACleanup();
    getchar();
}

void MainWindow::on_pushButton_Clear_clicked(){
    ui->lineEdit_IP->clear();
    ui->lineEdit_domain->clear();
    ui->textBrowser_IP->clear();
    ui->textBrowser_Domains->clear();
}
