#include "Server.h"

int Server::logout(int sockClient, vector<string> message)
{
    System::logout(message[1]);
    cout << message[1] + " quit." << endl;
    SSV(System::getOnlineUserList());
    return 1;
}

int Server::login(int sockClient, vector<string> message)
{
    USERTYPE userType = System::login(message[1], message[2]);
    if (userType) {
        cout << message[1] + " loging successfully!" << endl;
        UserNameAndSocket[message[1]] = sockClient;
    } else {
        cout << message[1] + " login falure。" << endl;
    }
    vector<string> messageToSend{ to_string(userType) };
    Send(sockClient, messageToSend);
    return 1;
}

int Server::enroll(int sockClient, vector<string> message)
{
    string name = message[1];
    string password = message[2];
    USERTYPE type = stoi(message[3]);

    int flag = System::enroll(name, password, type);
    vector<string> smessage = { to_string(flag) };
    Send(sockClient, smessage);

    if (flag) {
        cout << name + " enroll successful." << endl;
    }

    return 1;
}

int Server::showLeaderboard(int sockClient, vector<string> message)
{
    vector<vector<User>> leaderboards = System::showLeaderboard();
    string smessage = "FANTASTIC PLAYER:\n";
    for (User u : leaderboards[0]) {
        u.present();
    }
    cout << "FANTASTIC BUILDER:" << endl;
    for (User u : leaderboards[1]) {
        u.present();
    }

    return 1;
}

int Server::searchUser(int sockClient, vector<string> message)
{
    string key, value;
    cout << "[Please input the kind of key you want to search:]";
    cin >> key;
    cout << "[Please input the value of the key:]";
    cin >> value;
    vector<User> list = System::getUserbyAttribute(key, value);
    cout << "The search result is beneath:" << endl;
    for (User u : list) {
        u.present();
    }

    return 1;
}

double Server::playGame(int sockClient, vector<string> message)
{
    int level = stoi(message[1]);
    vector<string> levelWordList = System::getLevelWordList(level);
    Send(sockClient, levelWordList);
    message = Recv(sockClient);
    double timeUsage = atof(message[1].data());
    if (timeUsage) {
        Player player = IO::getPlayer(message[2]);
        player.plusExp(level, timeUsage);
        IO::UpdatePlayer(player);
    }
    return timeUsage;
}

int Server::addWord(int sockClient, vector<string> message)
{
    message[0] = to_string(System::addWord(message[1], message[2]));
    Send(sockClient, message);
    return 1;
}
// 7
int Server::showOnlinePlayer(int sockClient, vector<string> message)
{
    message = System::getOnlineUserList();
    Send(sockClient, message);
    return 1;
}
// 8
int Server::multiplayerGame(int sockClient, vector<string> message)
{
    // while (1) {
    // 显示在线的人
    vector<string> UserListOfBattleroom;
    for (string u : UserInBattleroom) {
        UserListOfBattleroom.push_back(u);
    }
    Send(sockClient, UserListOfBattleroom);
    // 接受命令
    message = Recv(sockClient); // 接收到下达的下标
    if (message[0] == "-1") {
        cout << "已返回主菜单" << endl;
        return 1;
    }
    // break;
    if (message[0] == "0") { // 0表示挂起，等待他人匹配
        /*1.收到我的名字*/
        message = Recv(sockClient);
        string myName = message[0];
        UserInBattleroom.insert(myName); // 挂起，等待下面这个分支分配游戏.此时处于 recv 状态
        message = Recv(sockClient); // 等待是否收到确认
        string reply = message[0];
        string rivalName = message[1];
        Send(UserNameAndSocket[rivalName], message); // 告知挑战发起方可以开战斗
        if (reply != "y")
            return 1;
        // 对方服务器已发送单词数据，这里等待接受成绩
        message = Recv(sockClient); // 等待接受自己的成绩
        int level = stoi(message[1]);
        message.pop_back();
        Send(UserNameAndSocket[rivalName], message); // 转交给对方进行判定
        double timeUsage = stof(message[0].data());
        message = Recv(sockClient); // 等待对方成绩
        cout << myName << ":" << timeUsage << endl;
        if (timeUsage) {
            Player player = IO::getPlayer(myName);
            player.plusExp(level, timeUsage);
            IO::UpdatePlayer(player);
        }
        cout << myName << ":" << timeUsage << endl;

        UserInBattleroom.erase(myName);
    } else {
        /*1.获得名字*/
        int index = stoi(message[0].data());
        index--;
        string rivalName = UserListOfBattleroom[index];
        message = Recv(sockClient); // 获得我的名字
        string myName = message[0];
        Send(UserNameAndSocket[rivalName], message); // 向对手发送我的名字
        /*2.获得关卡等级*/
        message = Recv(sockClient); // 接受本方的准备好的确认
        int level = stoi(message[0].data());
        /*3.发送战斗数据*/
        vector<string> levelWordList = System::getLevelWordList(level);
        Send(sockClient, levelWordList); // 发送单词列表和关卡等级
        levelWordList.push_back(to_string(level)); // 在尾追加等级
        Send(UserNameAndSocket[rivalName], levelWordList); // 发送同样的单词列表
        /*4.等待双方成绩*/
        message = Recv(sockClient); // 等待接受双方成绩
        cout << "向对方发送战斗数据" << endl;
        Send(UserNameAndSocket[rivalName], message); // 转发对方战斗数据
        cout << "战斗结束，进入清算阶段" << endl;
        double RivalTimeUsage = stof(message[0].data());
        double MyTimeUsage = stof(message[1].data());
        cout << myName << ":" << MyTimeUsage << endl;

        if (MyTimeUsage) {
            Player player = IO::getPlayer(myName);
            player.plusExp(level, MyTimeUsage);
            IO::UpdatePlayer(player);
        }
        cout << myName << ":" << MyTimeUsage << endl;
    }

    return 1;
}

int Server::chatroom(int sockClient, vector<string> message)
{
    string myName = message[1];
    UserInChattingroom.insert(myName);
    for (string userName : UserInChattingroom) {
        cout << userName << endl;
    }

    message = Recv(sockClient);
    while (message[1] != "-1") {
        for (string userName : UserInChattingroom) {
            cout << userName << endl;
            if (userName != myName)
                Send(UserNameAndSocket[userName], message);
        }
        message = Recv(sockClient);
    }
    UserInChattingroom.erase(myName);
    return 1;
}

void Server::Send(int sockClient, vector<string>& stringList)
{
    mut.lock();
    string tempString; // 必须要有一个实例用来存储数据，防止内存释放
    const char* MessagePtr = formMessage(stringList, tempString);
    send(sockClient, MessagePtr, tempString.length() + 1, 0);
    cout << "send " + to_string(sockClient) + ":" + tempString << endl;
    mut.unlock();
}

vector<string> Server::Recv(int sockClient)
{
    char buf[MAXDATASIZE];
    recv(sockClient, buf, MAXDATASIZE, 0);
    cout << sockClient << " received: ";
    vector<string> message = splitMessage(buf);
    for (string s : message)
        cout << s << ",";
    cout << endl;
    return message;
}

int main()
{
    Server Server;
}