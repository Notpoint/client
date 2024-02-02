#include "widget.h"
#include "./ui_widget.h"



#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}



void Widget::on_ml_editingFinished()
{

    QString value = ui->ml->text();
    ui->ml->clear();
    if(value.size()>=1023){
        QMessageBox::information(nullptr, "ERROR", "您发送的内容过长");
        return;
    }
    char message[1024];
    strncpy(message, value.toUtf8().data(), 1024);
    message[1023] = '\0'; // ensure null-terminated


    // 发送消息给服务器
    struct file_header header;
    header.length=strlen(message);
    header.type='t';
    char header1[16]; // 创建一个足够大的字符数组
    memcpy(header1, &header, sizeof(header)); // 复制结构体的内容到数组中
    char* header2 = header1;
    int result1 = send(socket,header2,sizeof(header),0);
    if (result1 == SOCKET_ERROR) {
        std::cout << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(socket);
        WSACleanup();
        delete wsadata;
        wsadata=NULL;
        socket=0;
        return ;
    }
    int result = send(socket, message, strlen(message), 0);
    if (result == SOCKET_ERROR) {
        std::cout << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(socket);
        WSACleanup();
        delete wsadata;
        wsadata=NULL;
        socket=0;
        return ;
    }

    std::cout << "Sent: " << message << std::endl;


}


void Widget::on_mb2_clicked()
{

    if(socket!=0){
        qInfo() << "connect close";
        closesocket(socket);
        WSACleanup();
        delete wsadata;
        wsadata=NULL;
        socket=0;
    }
    destory();


    return;
}

void Widget::av()
{
    while(true){
    // 接收服务器的回复
    char buffer[20];
    int result1 = recv(socket, buffer, sizeof(file_header), 0);
    if (result1 == 0) {
        // 服务器关闭了连接
        std::cout << "Server closed the connection" << std::endl;
        closesocket(socket);
        WSACleanup();
        delete wsadata;
        wsadata=NULL;
        socket=0;
        return ;
    }
    else if(result1<0){
        // 接收出错
        std::cout << "recv failed1: " << WSAGetLastError() << std::endl;
        closesocket(socket);
        WSACleanup();
        delete wsadata;
        wsadata=NULL;
        socket=0;
        return ;
    }
    struct file_header* header = (struct file_header*)buffer;
    char* databuffer = new char[header->length+1];
    memset(databuffer, 0, header->length+1);

    int result = recv(socket, databuffer, header->length, 0);
    if (result > 0) {
        if(header->type=='t'){
            ui->mtb->append(databuffer);
        }

    }
    else if (result == 0) {
        // 服务器关闭了连接
        std::cout << "Server closed the connection" << std::endl;
        closesocket(socket);
        WSACleanup();
        delete wsadata;
        wsadata=NULL;
        socket=0;
        delete[] databuffer;
        return ;
    }
    else if(result<0){
        // 接收出错
        std::cout << "recv failed2: " << WSAGetLastError() << std::endl;
        closesocket(socket);
        WSACleanup();
        delete wsadata;
        wsadata=NULL;
        socket=0;
        delete[] databuffer;
        return ;
    }
    delete[] databuffer;
    }
}

void Widget::destory()
{
    delete this;
}

void Widget::receive(WSADATA *wsadata, SOCKET socket, sockaddr_in serverAddr)
{
    this->wsadata=wsadata;
    this->socket=socket;
    this->serverAddr=serverAddr;
    emit trigger();
}

void Widget::started()
{
    std::thread t(av,this);
    t.detach();

}





void Widget::on_loadphoto_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this, "选择图片", "", "图片文件 (*.png *.jpg *.bmp)");
    // 如果用户没有取消选择，且文件路径不为空
    if (!imagePath.isEmpty())
    {
        std::string cuijie = imagePath.toStdString();
        std::ifstream img_file(cuijie, std::ios::binary);
        if (!img_file.is_open()) {
            std::cerr << "无法打开图片文件: " << cuijie << std::endl;
            return ;
        }
        img_file.seekg(0, std::ios::end);
        size_t img_size = img_file.tellg();
        img_file.seekg(0, std::ios::beg);
        // 创建一个缓冲区，用于存储图片数据
        img_buffer = new char[img_size];

        // 将图片数据读取到缓冲区中
        img_file.read(img_buffer, img_size);
        file_header header;
        header.length=img_size;
        if (img_buffer[0] == (char)0x42 && img_buffer[1] == (char)0x4D)
        {
            header.type='b';
        }else if(img_buffer[0] == (char)0xFF && img_buffer[1] == (char)0xD8){
            header.type='j';
            qInfo()<<"jpg";
        }else if(img_buffer[0] == (char)0x89 && img_buffer[1] == (char)0x50 &&
                   img_buffer[2] == (char)0x4E && img_buffer[3] == (char)0x47){
            header.type='p';
        }else{
            qInfo()<<"unkown type";
            delete[] img_buffer;
            img_buffer=NULL;
            img_file.close();
            return;
        }
        char* headerbuffer=(char*)(&header);
        int result = send(socket, headerbuffer, sizeof(header), 0);
        if (result == SOCKET_ERROR) {
            std::cout << "send failed: " << WSAGetLastError() << std::endl;
            closesocket(socket);
            WSACleanup();
            delete wsadata;
            wsadata=NULL;
            socket=0;
            delete[] img_buffer;
            img_buffer=NULL;
            img_file.close();
            return ;
        }
        int result2 = send(socket, img_buffer, header.length, 0);
        if (result2 == SOCKET_ERROR) {
            std::cout << "send failed: " << WSAGetLastError() << std::endl;
            closesocket(socket);
            WSACleanup();
            delete wsadata;
            wsadata=NULL;
            socket=0;
            delete[] img_buffer;
            img_buffer=NULL;
            img_file.close();
            return ;
        }




        delete[] img_buffer;
        img_buffer=NULL;
        img_file.close();
    }
}

