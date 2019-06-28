#include "System.cpp"

class Interface {
private:
    static int cleanTerminal();
    static int pressEnterToBack();
    System sys;
    USERTYPE userType = 0; // 0 for not login. Login as :1 for player while 2 for builder
    string currentUserName = ""; // Interface 唯一需要处理的逻辑部分。存储所登录的用户名。
public:
    int showMenu();
    int enroll();
    int login();
    static int showLeaderboard();
    static int searchUser();
    int addWord();
    int playGame();
};

int Interface::showMenu()
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
    string informationForPlayer = "5.playgame\n";
    string informationForBuilder = "6.addword\n";

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
    default:
        break;
    }

    return 1;
}

int Interface::login()
{
    cout << "Please input your name and password." << endl;
    string name, password;
    cout << "[Your Name:]";
    cin >> name;
    cout << "[Your Password:]";
    cin >> password;
    userType = sys.login(name, password);

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

int Interface::enroll()
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
    if (sys.enroll(name, password, type)) {
        cout << "Enroll successful." << endl;
    } else {
        cout << "Something wrong! Pleas check your name or password "
             << endl;
    }

    pressEnterToBack();
    return 1;
}

int Interface::showLeaderboard()
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

int Interface::searchUser()
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

int Interface::playGame()
{
    cout << "[Please Input the Level of Game you want to play(1~10,0 for your level):]";
    int level;
    cin >> level;
    cleanTerminal();
    double timeUsage = sys.playGame(level, currentUserName);
    if (timeUsage) {
        cout << "<Victory> Congratulations! Your speed is " << timeUsage << " s/word !" << endl;
    } else {
        cout << "<Defeat> Game Over! " << endl;
    }
    pressEnterToBack();
    return 1;
}

int Interface::addWord()
{
    cout << "[Please input the word you want to add:]";
    string word;
    cin >> word;
    if (sys.addWord(word, currentUserName)) {
        cout << "Add word successful." << endl;
    } else {
        cout << "Something wrong!" << endl;
    }

    pressEnterToBack();
    return 1;
}

int Interface::cleanTerminal()
{
#if DELAY
    /* 用来清理终端界面 */
    for (int i = 0; i < 5; i++)
        cout << "\033[2J\033[1; 1H" << endl;
#endif
    return 1;
}

int Interface::pressEnterToBack()
{
    cout << "Press Enter to back." << endl;
    getchar();
    getchar();
    return 1;
}