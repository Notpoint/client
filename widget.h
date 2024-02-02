#ifndef WIDGET_H
#define WIDGET_H
#include <QtConcurrent>
#include <QWidget>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <QThread>
#include <QFuture>
#include <QMessageBox>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QFileDialog>
#include <fstream>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

struct file_header {
    char type;
    long long length;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_ml_editingFinished();

    void on_mb2_clicked();

    void av();

    void destory();
    void on_loadphoto_clicked();

public slots:

    void receive(WSADATA* wsadata,SOCKET socket,sockaddr_in serverAddr);
    void started();
signals:
    void trigger();

private:
    Ui::Widget *ui;
    WSADATA *wsadata=NULL;
    SOCKET socket=0;
    sockaddr_in serverAddr;
    char* img_buffer;
};
#endif // WIDGET_H
