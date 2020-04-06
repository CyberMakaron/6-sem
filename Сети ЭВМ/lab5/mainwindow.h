#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <cstdlib>
#include <winsock.h>
#include <iphlpapi.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void getArpTable();
    void getMac(const char * IP);
    DWORD addIpNetEntry(const char * ip, const char * mac, unsigned iface);
    DWORD dropIpNetEntry(const char * ip, unsigned iface);
private slots:
    void on_refreshArpTable_clicked();
    void on_getMacFromIp_clicked();
    void on_addEntry_clicked();
    void on_deleteEntry_clicked();
    void on_pushButton_add_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_getMAC_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
