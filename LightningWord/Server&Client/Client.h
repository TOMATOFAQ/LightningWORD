#include "CommonForSocket.cpp"
#include "System.cpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
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

class Client : public System {

private:
    static int cleanTerminal();
    static int pressEnterToBack();
    USERTYPE userType = 0; // 0 for not login. Login as :1 for player while 2 for builder
    string currentUserName = "default"; // Client 唯一需要处理的逻辑部分。存储所登录的用户名。
    int sockClient = socket(AF_INET, SOCK_STREAM, 0);
    int MYPORT = 8888; // 初始端口设置为服务器的登入端口，随后将分配得到独有的端口
    char buf[MAXDATASIZE];
    mutex mut;

public:
    Client()
    {
        sockClient = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in addrSrv; // 配置目标服务器地址
        addrSrv.sin_addr.s_addr = inet_addr(ServerIp);
        addrSrv.sin_family = AF_INET;
        addrSrv.sin_port = htons(MYPORT);

        while (::connect(sockClient, (struct sockaddr*)&addrSrv, sizeof(sockaddr)) == -1) {
            cout << ::connect(sockClient, (struct sockaddr*)&addrSrv, sizeof(sockaddr)) << endl;

            MYPORT++; // 若绑定失败，则继续尝试 // 为什么一改就绑定不上？是没有初始化成功吗
            MYPORT %= 65535;
            addrSrv.sin_port = htons(MYPORT);
            std::cout << "connect failure."
                      << "try " << MYPORT << std::endl;
        } //与服务器进行连接
    }
    int showMenu();
    int enroll();
    int login();
    int logout();
    int showLeaderboard();
    int searchUser();
    int addWord();
    int playGame();
    int multiplayerGame();
    int showOnlinePlayer();
    int chatroom();
    int receiveBroadcast(int*);

    void Send(vector<string>& stringList);
    vector<string> Recv();
};

int Client::showMenu()
{
    cleanTerminal();
    ifstream ifstream_Brand("Brand.txt");
    string BrandLine;
    while (getline(ifstream_Brand, BrandLine))
        cout << BrandLine << endl;

    // 输出选项信息
    string information = "\nWelcome to LightningWord. " + currentUserName + "\n"
                                                                            "Please select your choice :\n"
                                                                            "0.quit\n";
    string infromationForLogin = "1.login\n"
                                 "2.enroll\n";
    string informationForUser = "3.show LeaderBoard\n"
                                "4.search user by attribute\n";
    string informationForPlayer = "5.playgame\n"
                                  "7.show Online Player\n"
                                  "8.multiplayer mode\n"
                                  "9.chatroom\n";
    string informationForBuilder = "6.addword\n"
                                   "9.chatroom\n";

    if (userType == 0)
        information += infromationForLogin;
    else
        information += informationForUser;
    if (userType == 1)
        information += informationForPlayer;
    if (userType == 2)
        information += informationForBuilder;
    cout << information;

    // 进行选择
    int choice;
    cout << "[Your Choice]:";
    cin >> choice;
    switch (choice) {
    case 0:
        logout();
        return 0;
    case 1:
        login();
        break;
    case 2:
        enroll();
        break;
    case 3:
        showLeaderboard();
        break;
    case 4:
        searchUser();
        break;
    case 5:
        playGame();
        break;
    case 6:
        addWord();
        break;
    case 7:
        showOnlinePlayer();
        break;
    case 8:
        multiplayerGame();
        break;
    case 9:
        chatroom();
        break;
    default:
        cout << "选择失败！" << endl;
        break;
    }
    return 1;
}
