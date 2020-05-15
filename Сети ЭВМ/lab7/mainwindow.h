#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QStringList>
#include <QVector>
#include "mytextbrowser.h"
#include <QFileDialog>

#define BUF_SIZE 10000
#define ERROR_LEN 100

class Letter{
    uint size, index;
    QString sender_name, sender_email;
    QString subject, body;
    bool read = false;
public:
    Letter(uint size):size(size){}
    Letter(){}
    void setParametrs(QString &toParse);
    QString &getSenderName(){return sender_name;}
    QString &getSenderEmail(){return sender_email;}
    QString &getSubject(){return subject;}
    QString &getLetterBody(){return body;}
    void setBody(QString &body){this->body = body; read = true;}
    uint getSize(){return size;}
    void setSize(uint size){this->size = size;}
    uint getIndex(){return index;}
    void setIndex(uint index){this->index = index;}
    bool isRead(){return read;}
    QString getPreiew();
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void connectToPOP3Server();
    void connectToSMTPServer();
    int initWSA();
    int closeWSA();
    int transactionPOP3(QString &mes, const uint len);
    int transactionSMTP(QString &mes);
    void refreshMail();
    void showLetter(int num);

private slots:
    void on_pushButton_authorization_clicked();
    void on_pushButton_writeLetter_clicked();
    void on_pushButton_newLetterCancel_clicked();
    void on_pushButton_refreshMail_clicked();
    void on_listWidget_letters_currentRowChanged(int currentRow);
    void on_pushButton_deleteLetter_clicked();

    void on_pushButton_newLetterSend_clicked();

    void on_pushButton_addAttachment_clicked();

private:
    Ui::MainWindow *ui;
    bool WSA_init = false;
    QFile log_filePOP3, log_fileSMTP;
    QTextStream logPOP3, logSMTP;
    SOCKET s_in_mail, s_out_mail;
    QVector<Letter> letters;
    MyTextBrowser *my_textBrowser;
    QString login, password;
    QVector<QByteArray> files_data;
    QStringList files_names;
};
#endif // MAINWINDOW_H
