#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QtConcurrent>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <QThread>
#include <QFuture>
#include "widget.h"
#include <QMessageBox>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
signals:
    void finished(WSADATA* wsadata,SOCKET socket,sockaddr_in serverAddr);
public slots:
    void destroy();
private slots:

    void on_loginButton_clicked();
    void av();
    void sendtoserver(QString account,QString password);
private:
    Ui::login *ui;
    WSADATA* wsadata=NULL;
    SOCKET socket=0;
    sockaddr_in serverAddr;
};

#endif // LOGIN_H
