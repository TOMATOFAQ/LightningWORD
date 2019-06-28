#include "Client.h"

int Client::logout()
{
    vector<string> message{ "0", currentUserName };
    Send(message);
    return 1;
}

int Client::login()
{
    cout << "Please input your name and password." << endl;
    string name, password;
    cout << "[Your Name:]";
    cin >> name;
    cout << "[Your Password:]";
    cin >> password;

    vector<string> message{ "1", name, password };
    Send(message);
    message = Recv();
    userType = stoi(message[0]);

    if (userType) {

        cout << "Login successful." << endl;
        currentUserName = name;
    } else {
        cout << "Something wrong! Pleas check your name and password "
                "or if you have logined."
             << endl;
    }

    pressEnterToBack();
    return 1;
}

int Client::enroll()
{
    cout << "Please input your name, password and the type." << endl;
    string name, password;
    USERTYPE type;
    cout << "[Your Name:]";
    cin >> name;
    cout << "[Your Password:]";
    cin >> password;
    cout << "[Your Type(1 for player, 2 for builder):]";
    cin >> type;

    vector<string> message = { "2", name, password, to_string(type) };
    Send(message);
    message = Recv();

    if (stoi(message[0])) {
        cout << "Enroll successful." << endl;
    } else {
        cout << "Something wrong! Pleas check your name or password "
             << endl;
    }

    pressEnterToBack();
    return 1;
}

int Client::showLeaderboard()
{
    vector<vector<User>> leaderboards = System::showLeaderboard();
    cout << "FANTASTIC PLAYER:" << endl;
    for (User u : leaderboards[0]) {
        u.present();
    }
    cout << "FANTASTIC BUILDER:" << endl;
    for (User u : leaderboards[1]) {
        u.present();
    }

    pressEnterToBack();
    return 1;
}

int Client::searchUser()
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

    pressEnterToBack();
    return 1;
}

int Client::playGame()
{

    cout << "[Please Input the Level of Game you want to play(1~10,0 for your level):]";
    int level;
    cin >> level;

    vector<string> message = { "5", to_string(level) };
    Send(message);
    message = Recv();

    cleanTerminal();
    double timeUsage = System::playGame(level, currentUserName, message);
    if (timeUsage) {
        cout << "<Victory> Congratulations! Your speed is " << timeUsage << " s/word !" << endl;
    } else {
        cout << "<Defeat> Game Over! " << endl;
    }

    message[1] = to_string(timeUsage);
    message[2] = currentUserName;

    Send(message);

    pressEnterToBack();
    return 1;
}

int Client::addWord()
{
    cout << "[Please input the word you want to add:]";
    string word;
    cin >> word;
    vector<string> message = { "6", word, currentUserName };
    Send(message);
    message = Recv();

    if (stoi(message[0])) {
        cout << "Add word successful." << endl;
    } else {
        cout << "Something wrong!" << endl;
    }

    pressEnterToBack();
    return 1;
}

int Client::showOnlinePlayer()
{
    vector<string> message = { "7" };
    Send(message);
    message = Recv();
    cout << "Online User:" << endl;

    for (int i = 0; i < message.size(); i++) {
        cout << i << "." << message[i] << endl;
    }
    pressEnterToBack();
    return 1;
}

int Client::multiplayerGame()
{
    int index = 0; // ???，这个 index 居然不是存储在栈里面的吗？
    while (index != -1) {
        // 展示在线的人
        vector<string> message = { "8" };
        Send(message);
        message = Recv();
        cout << "Online User:" << endl;
        for (int i = 0; i < message.size(); i++) {
            cout << i + 1 << "." << message[i] << endl;
        }
        // 发出命令
        cout << "[Choose one as your rival or type 0 for waiting while -1 for quit:]";
        cin >> index;
        CLEAR_VECTOR(message);
        message.push_back(to_string(index)); //发送命令
        Send(message);
        if (index == -1) {
            cout << "已返回主菜单" << endl;
            return 1; // 回主菜单
        }
        if (index == 0) {
            CLEAR_VECTOR(message);
            message.push_back(currentUserName);
            Send(message); // 发送名字
            cout << "wating for other guys~" << endl;
            message = Recv(); // 等待匹配，收到的是对手的名字
            string rivalName = message[0]; // 接收到对手的名字
            cout << "[Matching successful! Would you want to play with " + rivalName + " now?(y/n):]";
            string reply;
            cin >> reply;
            CLEAR_VECTOR(message);
            message.push_back(reply);
            message.push_back(rivalName);
            Send(message); // 发送回复
            if (reply != "y")
                break; // 回到主菜单

            message = Recv(); // 等待对方服务器传来的单词列表
            int level = stoi(*(message.end() - 1)); // 去除最后的等级
            HELLO
            cout << level << endl;
            message.pop_back();

            double timeUsage = System::playGame(level, currentUserName, message);

            CLEAR_VECTOR(message);
            message.push_back(to_string(timeUsage));
            message.push_back(to_string(level));
            Send(message); // 向服务器发送自己的成绩，并附加关卡等级

            message = Recv(); // 接收对方成绩
            Send(message); // 转交给己方服务器

            if (stof(message[1]) >= stof(message[0]) && stof(message[0]) != 0) {
                cout << "<Victory> Congratulations! Your speed is " << timeUsage << " s/word !" << endl;
            } else {
                cout << "<Defeat> Game Over! " << endl;
            }
            // 当前循环结束，等待下一轮

        } else if (index != -1) {
            /*1.获得名字*/
            message[0] = currentUserName;
            Send(message); //发送我的名字
            message = Recv(); // 等待对方服务器发动的确认
            if (message[0] != "y") {
                cout << message[1] + " didn't accept your challenge!" << endl;
                break;
            }
            /*2.获得关卡等级*/
            cout << "[Please Input the Level of Game you want to play(1~10,0 for your level):]";
            int level;
            cin >> level;
            message[0] = to_string(level);
            Send(message); // 向己方服务器发送确认。准备接受战斗数据

            message = Recv(); //接收单词列表
            double timeUsage = System::playGame(level, currentUserName, message);

            CLEAR_VECTOR(message);
            message = Recv(); // 等待对方的结果
            message.push_back(to_string(timeUsage)); // 0存的是对方战绩，1 存的是我方成绩
            Send(message); // 向服务器发送双方成绩

            if (stof(message[1]) <= stof(message[0])) {
                cout << "<Victory> Congratulations! Your speed is " << timeUsage << " s/word !" << endl;
            } else {
                cout << "<Defeat> Game Over! " << endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return 1;
}

int Client::chatroom()
{
    vector<string> message = { "9", currentUserName };
    Send(message);
    cout << "Welcome to chatroom." << endl;
    string input = currentUserName + " log in.";
    message[0] = currentUserName;
    int flag = 1;
    int* flagPtr = &flag;
    thread threadForReceiveBroadcast(&Client::receiveBroadcast, this, flagPtr);
    threadForReceiveBroadcast.detach();
    while (message[1] != "-1") {
        cout << "[" << currentUserName << ":]";
        cin >> input;
        message[1] = input;
        Send(message);
    }
    flag = -1;
    return 1;
}

int Client::receiveBroadcast(int* flag)
{
    vector<string> message;
    while (*flag != -1) {
        message = Recv();
        cout << message[0] << ":" << message[1] << endl;
    }
    return 1;
}

int Client::cleanTerminal()
{
#if DELAY
    /* 用来清理终端界面 */
    for (int i = 0; i < 5; i++)
        cout << "\033[2J\033[1; 1H" << endl;
#endif
    return 1;
}

int Client::pressEnterToBack()
{
    cout << "Press Enter to back." << endl;
    getchar();
    getchar();
    return 1;
}

void Client::Send(vector<string>& stringList)
{
    mut.lock();
    string tempString; // 必须要有一个实例用来存储数据，防止内存释放
    const char* MessagePtr = formMessage(stringList, tempString);
    send(sockClient, MessagePtr, tempString.length() + 1, 0);
    mut.unlock();
    // this_thread::sleep_for(std::chrono::seconds(1));
}

vector<string> Client::Recv()
{
    char buf[MAXDATASIZE];
    recv(sockClient, buf, MAXDATASIZE, 0);
    vector<string> message = splitMessage(buf);
    // for (string s : message)
    //     cout << s << ",";
    // cout << endl;
    return message;
}

int main()
{
    Client Client;
    while (Client.showMenu()) {
    }
}