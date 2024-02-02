#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
}

login::~login()
{
    delete ui;
}

void login::destroy()
{
    delete this;
}


void login::on_loginButton_clicked()
{
    QString account = ui->accountline->text();
    QString password = ui->passwordline->text();
    using namespace std;
    if(wsadata==NULL&&socket==0){
        wsadata =new WSADATA();
        int result = WSAStartup(MAKEWORD(2, 2), wsadata);
        if (result != 0) {
            cout << "WSAStartup failed: " << result << endl;
            QMessageBox::information(nullptr, "登录失败", "socket服务初始化失败");
            delete wsadata;
            wsadata=NULL;
            return ;
        }

        // 创建客户端套接字
        socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socket == INVALID_SOCKET) {
            cout << "socket failed: " << WSAGetLastError() << endl;
            QMessageBox::information(nullptr, "登录失败", "创建socket失败");
            WSACleanup();
            delete wsadata;
            wsadata=NULL;
            socket=0;
            return ;
        }

        // 连接到服务器的地址和端口
        serverAddr.sin_family = AF_INET;
        // serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP); // 修改前
        inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr); // 修改后
        serverAddr.sin_port = htons(SERVER_PORT);

        result = ::connect(socket, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (result == SOCKET_ERROR) {
            cout << "connect failed: " << WSAGetLastError() << endl;
            QMessageBox::information(nullptr, "登录失败", "无法与服务器建立连接");
            closesocket(socket);
            WSACleanup();
            delete wsadata;
            wsadata=NULL;
            socket=0;
            return ;
        }

        cout << "Connected to server " << SERVER_IP << ":" << SERVER_PORT << endl;
        sendtoserver(account,password);
        av();






    }else{
        qInfo() << "already connected";
    }


}

void login::av()
{

        // 接收服务器的回复
        char buffer[1024];
        int result = recv(socket, buffer, 1024, 0);
        if (result > 0) {
            // 打印接收到的消息
            buffer[result] = '\0';
            if(strcmp(buffer, "accept") == 0){
                std::cout << "login success" << buffer << std::endl;
                emit finished(wsadata,socket,serverAddr);
            }else{
                QMessageBox::information(nullptr, "登录失败", "账户名或密码错误");
                std::cout << "Server closed the connection" << std::endl;
                closesocket(socket);
                WSACleanup();
                delete wsadata;
                wsadata=NULL;
                socket=0;
                return ;
            }

        }
        else if (result == 0) {
            QMessageBox::information(nullptr, "登录失败", "账户名或密码错误");
            // 服务器关闭了连接
            std::cout << "Server closed the connection" << std::endl;
            closesocket(socket);
            WSACleanup();
            delete wsadata;
            wsadata=NULL;
            socket=0;
            return ;
        }
        else {
            // 接收出错
            QMessageBox::information(nullptr, "登录失败", "已建立连接但与服务器的连接出现错误");
            std::cout << "recv failed: " << WSAGetLastError() << std::endl;
            closesocket(socket);
            WSACleanup();
            delete wsadata;
            wsadata=NULL;
            socket=0;
            return ;
        }
}

void login::sendtoserver(QString account,QString password)
{
    char message[1024];
    strcat_s(message, sizeof(message), account.toUtf8().data());
    strcat_s(message, sizeof(message), " ");
    strcat_s(message, sizeof(message), password.toUtf8().data());

    int result = send(socket, message, strlen(message), 0);
    if (result == SOCKET_ERROR) {
        std::cout << "send failed: " << WSAGetLastError() << std::endl;
        QMessageBox::information(nullptr, "登录失败", "已建立连接但发送给服务器登录状态失败");
        closesocket(socket);
        WSACleanup();
        delete wsadata;
        wsadata=NULL;
        socket=0;
        return ;
    }

    std::cout << "Sent: " << message << std::endl;
}

