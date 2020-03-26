#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QDebug>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wsipx.h>
#include <process.h>
#define BUF_SIZE 500

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showMessage(QString input_message);
    void showInetInfo(QString srv_adr, QString srv_port, QString clt_adr, QString clt_port);
    int InitInet();

private slots:
    void on_lineEdit_messageToSrv_returnPressed();
    void on_pushButton_sendMessage_clicked();

private:
    Ui::MainWindow *ui;
public:
    SOCKET s;
    SOCKADDR_IN srv_adr, clt_adr;
};
#endif // MAINWINDOW_H
