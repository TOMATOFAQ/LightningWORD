#include "CommonForSocket.cpp"
#include "System.cpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#define BACKLOG 5
#include <vector>
#define HELLO cout << "hello." << endl;
#define SSV(v)                 \
    {                          \
        for (string s : v)     \
            cout << s << endl; \
    }
#define CLEAR_VECTOR(v)   \
    {                     \
        while (v.size())  \
            v.pop_back(); \
    }
#define MAXDATASIZE 99999
using namespace std;
const char* ServerIp = "127.0.0.1"; // 本机IP 北京市海淀区教育网

class Server : public System {

private:
    char buf[MAXDATASIZE];
    int MYPORT = 8888;
    int sockServer;
    map<string, int> UserNameAndSocket;
    set<string> UserInChattingroom;
    set<string> UserInBattleroom;
    mutex mut;

public:
    Server()
    {
        sockServer = socket(AF_INET, SOCK_STREAM, 0); //面向连接的可靠性服务SOCK_STRAM

        struct sockaddr_in addrServer; //配置服务器地址
        addrServer.sin_addr.s_addr = inet_addr(ServerIp);
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = htons(MYPORT); //htos用来将端口转换成字符，1024以上的数字即可

        while (::bind(sockServer, (struct sockaddr*)&addrServer, sizeof(struct sockaddr)) == -1) {
            // NOTE:在这里折腾了一万年。socket 下有个 bind 函数，std 命名空间下居然还有一个 bind 函数
            MYPORT++; // 若绑定失败，则继续尝试
            MYPORT %= 65535;
            addrServer.sin_port = htons(MYPORT); //htos用来将端口转换成字符，1024以上的数字即可

            std::cout << "bind failure."
                      << "try " << MYPORT << std::endl;
        }

        std::cout << "bind successfully."
                  << "PORT: " << MYPORT << std::endl;

        thread threadWaitingForCommand(&Server::waitingForClient, this);
        threadWaitingForCommand.detach();
        int exit = 0;
        while (!exit) {
        }
    }

    void waitingForClient() // waiting for command 应该单独开出一个线程。这个线程作为结构专门接受消息。并将解析到的命令传到系统
    {
        listen(sockServer, BACKLOG); //等待队列中的最大长度为BACKLOG

        unsigned int len = sizeof(struct sockaddr);

        sockaddr_in addrClient;

        while (1) {
            std::cout << "\nWaiting for clients." << std::endl;
            int sockClient = accept(sockServer, (struct sockaddr*)&addrClient, &len); // 使用新的socket进行通信
            //阻塞调用进程直至新的连接出现

            if (sockClient == -1) {
                continue; //继续监听
            }

            std::cout << "New Client Joins In:" + to_string(sockClient) << std::endl;

            thread userThread(&Server::userThread, this, sockClient);
            userThread.detach();
        }
    }

    int userThread(int sockClient);
    int enroll(int sockClient, vector<string> message);
    int login(int sockClient, vector<string> message);
    int logout(int sockClient, vector<string> message);
    int showLeaderboard(int sockClient, vector<string> message);
    int searchUser(int sockClient, vector<string> message);
    int addWord(int sockClient, vector<string> message);
    double playGame(int sockClient, vector<string> message);
    int showOnlinePlayer(int sockClient, vector<string> message);
    int multiplayerGame(int sockClient, vector<string> message);
    int chatroom(int sockClient, vector<string> message);

    void Send(int sockClient, vector<string>& stringList);
    vector<string> Recv(int sockClient);
};

int Server::userThread(int sockClient)
{
    int quitFlag = 0;
    while (!quitFlag) {
        vector<string> message = Recv(sockClient);
        int choice = stoi(message[0]);
        switch (choice) {
        case 0:
            logout(sockClient, message);
            quitFlag = 1;
            return 0;
        case 1:
            login(sockClient, message);
            break;
        case 2:
            enroll(sockClient, message);
            break;
        case 3:
            showLeaderboard(sockClient, message);
            break;
        case 4:
            searchUser(sockClient, message);
            break;
        case 5:
            playGame(sockClient, message);
            break;
        case 6:
            addWord(sockClient, message);
            break;
        case 7:
            showOnlinePlayer(sockClient, message);
            break;
        case 8:
            multiplayerGame(sockClient, message);
            break;
        case 9:
            chatroom(sockClient, message);
            break;
        default:
            break;
        }
    }
    return 1;
}
