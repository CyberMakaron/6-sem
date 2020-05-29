#ifndef NETDIALOG_H
#define NETDIALOG_H

#include <QDialog>
#include <QVector>
#include <QPair>
#include <QMessageBox>

namespace Ui {
class NetDialog;
}

class NetDialog : public QDialog{
    Q_OBJECT
public:
    explicit NetDialog(QWidget *parent = nullptr);
    ~NetDialog();
    void setMode(bool is_create);
    void getParametrs(QString &name, QString &password);
    void clearAll();
    void setTable(QVector<QPair<QString, QString>> table);

private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_connect_clicked();
    void on_pushButton_check_pass_clicked();
    void on_pushButton_connect_back_clicked();

private:
    Ui::NetDialog *ui;
    QString name, password;
    void paintEvent(QPaintEvent* event);
};

#endif // NETDIALOG_H
