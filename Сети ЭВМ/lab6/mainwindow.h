#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <cstdlib>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void getDHCPInfo();

private slots:
    void on_pushButton_getIP_clicked();
    void on_pushButton_getDomain_clicked();
    void on_pushButton_Clear_clicked();
    void on_pushButton_resetIP_clicked();

    void on_pushButton_getNewIP_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
