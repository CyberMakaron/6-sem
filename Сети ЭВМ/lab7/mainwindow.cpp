#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , logPOP3(&log_filePOP3), logSMTP(&log_fileSMTP)
{
    ui->setupUi(this);
    //log_file.setFileName(QString("logPOP3_") + QDateTime::currentDateTime().QDateTime::toString("yyyy-MM-dd_hh-mm") + ".txt");
    log_filePOP3.setFileName("logPOP3.txt");
    log_filePOP3.open(QIODevice::WriteOnly| QIODevice::Text);
    log_fileSMTP.setFileName("logSMTP.txt");
    log_fileSMTP.open(QIODevice::WriteOnly| QIODevice::Text);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->stackedWidget->setCurrentIndex(0);
    my_textBrowser = new MyTextBrowser(this);
    ui->browserLayout->addWidget(my_textBrowser);
}

MainWindow::~MainWindow(){
    QString mes;
    mes = "QUIT\r\n";
    transactionSMTP(mes);
    transactionPOP3(mes, 0);
    log_filePOP3.close();
    log_fileSMTP.close();
    closesocket(s_in_mail);
    closesocket(s_out_mail);
    closeWSA();
    delete my_textBrowser;
    delete ui;
}

int connectTCP(SOCKET s, SOCKADDR_IN srv_addr){
    return connect(s, (sockaddr*)&srv_addr, sizeof(srv_addr));
}

void MainWindow::connectToPOP3Server(){
    char host_in_name[] = "mail.ngs.ru";
    int server_in_port = 110;
    char buf[BUF_SIZE];
    int err;
    struct hostent *host;
    struct sockaddr_in clt_addr;
    struct sockaddr_in srv_addr;
    unsigned short socket_clt = 0x0;
    int iResult;
    if (initWSA() != 0){
        close();
        return;
    }
    //создание сокета POP3
    s_in_mail = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset((char *)&clt_addr, 0, sizeof(clt_addr));
    clt_addr.sin_family = AF_INET;
    clt_addr.sin_port = htons(socket_clt);
    err = bind(s_in_mail, (sockaddr*)&clt_addr, sizeof(clt_addr));
    if (err == SOCKET_ERROR){
        logPOP3 << QString("Ошибка связывания сокета: ") << WSAGetLastError() << endl;
        close();
        return;
    }
    memset((char*)&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    host = gethostbyname(host_in_name);     //адрес узла по имени
    memcpy((char *)&srv_addr.sin_addr, host->h_addr, host->h_length); //копирование адреса удаленного узла в структуру srv_addr
    srv_addr.sin_port = htons(server_in_port);  //заполнения адреса порта сервера с сетевым порядком расположения байтов

    logPOP3 << QString("Connecting to \'%1\'").arg(host_in_name) << endl;
    err = connectTCP(s_in_mail, srv_addr);
    if (err == 0) logPOP3 << QString("Соединение с сервером %1 установлено\n").arg(host_in_name);
        else{
        logPOP3 << QString("Ошибка соединения с сервером %1: ").arg(host_in_name) << WSAGetLastError() << endl;
        close();
        return;
    }
    iResult = recv(s_in_mail, buf, BUF_SIZE, 0);
    buf[iResult] = '\0';
    if(iResult == SOCKET_ERROR){
        logPOP3 << QString("Ошибка при получении: %1").arg(WSAGetLastError()) << endl;
        close();
        return;
    } else{
        QString str(buf);
        int i = str.indexOf("\r\n.");
        if (i > 0) str.truncate(i);
        logPOP3 << QString("<< %1").arg(str) << endl;
    }
}

void MainWindow::connectToSMTPServer(){
    char host_out_name[] = "smtp.ngs.ru";
    int server_out_port = 25;
    char buf[BUF_SIZE];
    int err;
    struct hostent *host;
    struct sockaddr_in clt_addr;
    struct sockaddr_in srv_addr;
    unsigned short socket_clt = 0x0;
    int iResult;
    if (initWSA() != 0){
        close();
        return;
    }
    // Создание сокета SMTP
    s_out_mail = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset((char *)&clt_addr, 0, sizeof(clt_addr));
    clt_addr.sin_family = AF_INET;
    clt_addr.sin_port = htons(socket_clt);
    err = bind(s_out_mail, (sockaddr*)&clt_addr, sizeof(clt_addr));
    if (err == SOCKET_ERROR){
        logSMTP << QString("Ошибка связывания сокета: ") << WSAGetLastError() << endl;
        close();
        return;
    }
    memset((char*)&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    host = gethostbyname(host_out_name);     //адрес узла по имени
    memcpy((char *)&srv_addr.sin_addr, host->h_addr, host->h_length); //копирование адреса удаленного узла в структуру srv_addr
    srv_addr.sin_port = htons(server_out_port);  //заполнения адреса порта сервера с сетевым порядком расположения байтов

    logSMTP << QString("Connecting to \'%1\'").arg(host_out_name) << endl;
    err = connectTCP(s_out_mail, srv_addr);
    if (err == 0) logSMTP << QString("Соединение с сервером %1 установлено\n").arg(host_out_name);
        else{
        logSMTP << QString("Ошибка соединения с сервером %1: ").arg(host_out_name) << WSAGetLastError() << endl;
        close();
        return;
    }
    iResult = recv(s_out_mail, buf, BUF_SIZE, 0);
    buf[iResult] = '\0';
    if(iResult == SOCKET_ERROR){
        logSMTP << QString("Ошибка при получении: %1").arg(WSAGetLastError()) << endl;
        close();
        return;
    } else{
        QString str(buf);
        int i = str.indexOf("\r\n.");
        if (i > 0) str.truncate(i);
        logSMTP << QString("<< %1").arg(str) << endl;
    }
    QString mes = "EHLO test\r\n";
    if (transactionSMTP(mes))
        QMessageBox::warning(this, "", "Ошибка!");
}

int MainWindow::initWSA() {
    if (WSA_init) return 0;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        logPOP3 << "WSAStartup failed with error: " << err << ", " << WSAGetLastError() << endl;
        return 1;
    }
    WSA_init = true;
    return 0;
}

int MainWindow::closeWSA() {
    int err;
    err = WSACleanup();
    if (err != 0) {
        logPOP3 << "WSACleanup failed with error: " << err << ", " << WSAGetLastError() << endl;
        return 1;
    }
    return 0;
}

int MainWindow::transactionPOP3(QString &mes, const uint len){
    int iResult;
    char buf[BUF_SIZE];
    logPOP3 << "------------------------------------------------------" << endl;
    logPOP3 << ">> " + QString(mes) << endl;
    strcpy(buf, mes.toStdString().c_str());
    send(s_in_mail, buf, strlen(buf), 0);         //Отправить запрос
    iResult = recv (s_in_mail, buf, BUF_SIZE, 0);  //Получить ответ
    buf[iResult] = '\0';
    if(iResult == SOCKET_ERROR){
        logPOP3 << QString("Ошибка при получении: ") << WSAGetLastError() << endl;
        qDebug() << "Socket error: " << WSAGetLastError();
        return 2;
    } else{
        QString str(buf);
        int i = str.indexOf("\r\n.");
        if (i > 0){
            str.truncate(i);
            logPOP3 << QString("<< %1").arg(str) << endl;
        } else logPOP3 << QString("<< %1").arg(str);
        logPOP3 << endl << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        if (iResult > ERROR_LEN){
            logPOP3 << QString("Сервер вернул результат запроса вместе с информационным сообщением") << endl;
            i = str.indexOf("\r\n");
            i += 4;
            str.remove(0, i);
            mes = str;
            return 3;
        }
        mes = str;
    }
    QString str(buf);
    if (!str.contains("+OK")) return 1;
    if (QString(buf).contains("follow")){
        char buf_custom[len];
        iResult = recv (s_in_mail, buf_custom, len, 0);  //Получить ответ
        QString str(buf_custom);
        str.truncate(str.lastIndexOf("\r\n."));
        mes = str;
        logPOP3 << QString("Принято байт: ") << iResult << endl << endl;
        logPOP3 << "******************************************************" << endl;
    }
    return 0;
}

int MainWindow::transactionSMTP(QString &mes){
    int iResult;
    int len = BUF_SIZE > mes.length() ? BUF_SIZE : mes.length() + 100;
    char *buf = new char[len];
    memset(buf, 0, len);
    logSMTP << "------------------------------------------------------" << endl;
    logSMTP << ">> " + QString(mes);
    strcpy(buf, mes.toStdString().c_str());
    send(s_out_mail, buf, strlen(buf), 0);
    iResult = recv (s_out_mail, buf, BUF_SIZE, 0);
    buf[iResult] = '\0';
    QString str(buf);
    if (str.left(str.indexOf(" ")).toInt() >= 450){
        logSMTP << "<< " << str;
        delete[] buf;
        return 1;
    }
    if (str.contains("334")){
        int pos = str.indexOf(" ");
        logSMTP << "<< " + str;
        str = str.left(pos) + " " + QString(QByteArray::fromBase64(str.right(str.length() - pos - 1).toUtf8()));
        logSMTP << "   " + str;
    } else logSMTP << "<< " << str;
    logSMTP << endl << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    delete[] buf;
    return 0;
}

void MainWindow::refreshMail(){
    ui->centralwidget->setDisabled(true);
    letters.clear();
    ui->listWidget_letters->clear();
    my_textBrowser->clear();
    ui->lineEdit_letterSender->clear();
    ui->lineEdit_letterSubject->clear();
    QString mes;
    int ok;
    mes = "LIST\r\n";
    ok = transactionPOP3 (mes, 0);          //отправка команды LIST
    if (ok == 1 || ok == 2){
        QMessageBox::warning(this, "", "Ошибка!");
        ui->centralwidget->setDisabled(false);
        return;
    }
    QStringList list = mes.split("\r\n");
    for (int i = 1; i < list.size(); i++){ //1-я строка - сообщение от сервера, дальше информация о письмах
        letters << Letter(list[i].remove(0, list[i].indexOf(" ") + 1).toInt() + 10);
        letters[i-1].setIndex(i);
    }
    for(int i = 0; i < letters.length(); i++){
        mes = "TOP " + QString::number(i+1) +  " 0\r\n";
        ok = transactionPOP3(mes, letters[i].getSize());         //отправка команды LIST
        if (ok == 1 || ok == 2){
            QMessageBox::warning(this, "", "Ошибка!");
            ui->centralwidget->setDisabled(false);
            return;
        }
        letters[i].setParametrs(mes);
        ui->listWidget_letters->addItem(letters[i].getPreiew());
    }
    ui->centralwidget->setDisabled(false);
}

void MainWindow::showLetter(int num){
    my_textBrowser->clear();
    ui->centralwidget->setDisabled(true);
    if (!letters[num].isRead()){
        QString mes = "RETR " + QString::number(num + 1) + "\r\n";
        int ok;
        ok = transactionPOP3(mes, letters[num].getSize());
        if (ok == 1 || ok == 2){
            QMessageBox::warning(this, "", "Ошибка!");
            ui->centralwidget->setDisabled(false);
            return;
        }
        int pos = mes.indexOf("Content-Transfer-Encoding:");
        pos = mes.indexOf(" ", pos) + 1;
        QString content_transfer_encoding = mes.mid(pos, mes.indexOf("\r\n", pos) - pos);
        mes.remove(0, mes.indexOf("\r\n\r\n"));
        if (content_transfer_encoding == "utf-8" || content_transfer_encoding == "8bit") letters[num].setBody(mes);
        else if (content_transfer_encoding == "base64"){
            QString tmp = QByteArray::fromBase64(mes.toUtf8());
            letters[num].setBody(tmp);
        }
        else logPOP3 << "Неизвестная кодировка: " << content_transfer_encoding << "!" << endl;
    }
    my_textBrowser->setHtml(letters[num].getLetterBody());
    ui->lineEdit_letterSender->setText(letters[num].getSenderName() + " " + letters[num].getSenderEmail());
    ui->lineEdit_letterSubject->setText(letters[num].getSubject());
    ui->centralwidget->setDisabled(false);
}

void MainWindow::on_pushButton_authorization_clicked(){
    QString mes;
    int ok;
    login = ui->lineEdit_login->text();
    password = ui->lineEdit_password->text();
    mes = "USER " + login + "\r\n";
    ok = transactionPOP3 (mes, 0);          //отправка команды USER <логин>
    if (ok == 1){
        QMessageBox::warning(this, "", "Неверный логин!");
        return;
    }
    if (ok == 2) return;

    mes = "PASS " + password + "\r\n";
    ok = transactionPOP3(mes, 0);          //отправка команды PASS <пароль>
    if (ok == 1){
        QMessageBox::warning(this, "", "Неверный пароль!");
        return;
    }
    if (ok == 2) return;
    QMessageBox::information(this, " ", "Вы успешно авторизованы!");

    mes = "AUTH LOGIN\r\n";
    if (transactionSMTP(mes))
        QMessageBox::warning(this, "", "Ошибка!");
    QByteArray b_arr = login.toUtf8();
    mes = b_arr.toBase64() + "\r\n";
    if (transactionSMTP(mes))
        QMessageBox::warning(this, "", "Ошибка!");
    b_arr = password.toUtf8();
    mes = b_arr.toBase64() + "\r\n";
    if (transactionSMTP(mes))
        QMessageBox::warning(this, "", "Ошибка!");

    ui->stackedWidget->setCurrentIndex(1);
    //refreshMail();
}

void MainWindow::on_pushButton_writeLetter_clicked(){
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pushButton_newLetterCancel_clicked(){
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_refreshMail_clicked(){
    refreshMail();
}

void MainWindow::on_listWidget_letters_currentRowChanged(int currentRow){
    if (currentRow >= 0)
        showLetter(currentRow);
}

void MainWindow::on_pushButton_deleteLetter_clicked(){
    QString mes = "DELE " + QString::number(ui->listWidget_letters->currentRow() + 1) + "\r\n";
    transactionPOP3 (mes, 0);
    refreshMail();
}

void Letter::setParametrs(QString &toParse){
    int pos = toParse.indexOf("Content-Transfer-Encoding:"), pos2;
    pos = toParse.indexOf(" ", pos) + 1;
    QString content_transfer_encoding = toParse.mid(pos, toParse.indexOf("\r\n", pos) - pos);
    pos = toParse.indexOf("From: =?utf-8?", 0, Qt::CaseInsensitive) + 16;     // ?B?
    QString name, email;
    if (pos != -1) {
        pos2 = toParse.indexOf("?= ", pos);
        name = toParse.mid(pos, pos2 - pos);
        pos = pos2 + 3;
        if (toParse[pos] == "<"){
            pos2 = toParse.indexOf("\r\n", pos);
            email = toParse.mid(pos, pos2 - pos);
        }
    }
    QStringList subj;
    pos = toParse.indexOf("Subject: =?utf-8?", 0, Qt::CaseInsensitive) + 19;  // ?B?
    if (pos != -1){
        pos2 = toParse.indexOf("?=", pos);
        subj << toParse.mid(pos, pos2 - pos);
        bool add_subj = true;
        while(add_subj){
            pos = pos2 + 4;
            if (toParse[pos] == "\t"){
                pos = toParse.indexOf("=?utf-8?", pos, Qt::CaseInsensitive) + 10;  // ?B?
                pos2 = toParse.indexOf("?=", pos);
                QString tmp = toParse.mid(pos, pos2 - pos);
                subj << tmp;
            } else add_subj = false;
        }
    }

    sender_email = email;
    if (content_transfer_encoding == "utf-8"){
        sender_name = name;
        for (int i = 0; i < subj.length(); i++)
            subject += subj[i];
    }else if (content_transfer_encoding == "8bit"){ //пока непонятно почему!!!
        sender_name = QByteArray::fromBase64(name.toUtf8());
        for (int i = 0; i < subj.length(); i++)
            subject += QByteArray::fromBase64(subj[i].toUtf8());
    }
    else if (content_transfer_encoding == "base64"){
        sender_name = QByteArray::fromBase64(name.toUtf8());
        for (int i = 0; i < subj.length(); i++)
            subject += QByteArray::fromBase64(subj[i].toUtf8());
    }
    else qDebug() << "Неизвестная кодировка: " << content_transfer_encoding << "!" << endl;
}

QString Letter::getPreiew(){
    QString str;
    str = "----------------------------------------\n";
    str += "№ " + QString::number(index) + " | " + QString::number(size) + " байт\n";
    str += "От: " + sender_name + "\n";
    str += "    " + sender_email + "\n";
    str += subject + "\n";
    str += "----------------------------------------";
    return str;
}

void MainWindow::on_pushButton_newLetterSend_clicked(){
    QString mes;
    mes = "MAIL FROM:<" + ui->lineEdit_newLetterSenderAddres->text() + ">\r\n";
    if (transactionSMTP(mes))
        QMessageBox::warning(this, "", "Ошибка!");
    mes = "RCPT TO:<" + ui->lineEdit_newLetterRecipientAddres->text() + ">\r\n";
    if (transactionSMTP(mes))
        QMessageBox::warning(this, "", "Ошибка!");

    mes = "DATA\r\n";
    if (transactionSMTP(mes))
        QMessageBox::warning(this, "", "Ошибка!");
    mes = "From: " + ui->lineEdit_newLetterSener->text() + " <" + ui->lineEdit_newLetterSenderAddres->text() + ">\r\n";
    mes += "Subject: " + ui->lineEdit_newLetterSubject->text() + "\r\n";
    mes += "To: " + ui->lineEdit_newLetterRecipientAddres->text() + "\r\n";
    mes += "MIME-Version: 1.0\r\n";
    mes += "Content-Type: multipart/mixed; boundary=\"**********\"\r\n";
    mes += "--**********\r\n";
    mes += "Content-Type: text/html; charser=\"utf-8\"\r\n";
    mes += "\r\n" + ui->textEdit_newLetterBody->toHtml() + "\r\n";
    for (int i = 0; i < files_data.length(); i++){
        mes += "--**********\r\n";
        mes += "Content-Type: application/octet-stream; name=\"" + files_names[i] + "\"\r\n";
        mes += "Content-Transfer-Encoding: base64\r\n";
        mes += "\r\n" + QString(files_data[i].toBase64()) + "\r\n";
    }
    mes += "--**********\r\n";
    mes += ".\r\n";
    if (transactionSMTP(mes))
        QMessageBox::warning(this, "", "Ошибка!");
    else QMessageBox::information(this, "", "Успешно!");
    ui->textEdit_newLetterBody->clear();
    ui->lineEdit_newLetterSener->clear();
    ui->lineEdit_newLetterSubject->clear();
    ui->lineEdit_newLetterSenderAddres->clear();
    ui->lineEdit_newLetterRecipientAddres->clear();
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_addAttachment_clicked(){
    QString fname = QFileDialog::getOpenFileName(this, "Выберите файл");
    QFile file(fname);
    file.open(QIODevice::ReadOnly);
    files_data << file.readAll();
    files_names << fname.right(fname.length() - fname.lastIndexOf("/") - 1);
    file.close();
    QMessageBox::information(this, "", "Файл добавлен!");
}
