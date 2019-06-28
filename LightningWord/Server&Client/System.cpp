#include <utility>

#include "Basic.h"
#include "Game.cpp"

class System : public IO {

private:
    vector<string> OnlineUserList;

public:
    /* System */
    vector<string> getOnlineUserList() { return OnlineUserList; }
    int login(const string& name, const string& password);
    int logout(const string& name);
    static int enroll(const string& name, string password, USERTYPE type);
    static vector<vector<User>> showLeaderboard()
    {
        return IO::getLeaderboard();
    }

    static vector<User> getUserbyAttribute(const string& attribute, const string& value)
    {
        return IO::getUserbyAttribute(attribute, value);
    }

    /* module for Builder */
    static int addWord(const string& word, const string& builderName);

    /* module for Player */
    static double playGame(int level, const string& playerName);
    static double playGame(int level, const string& playerName, vector<string>);
};

int System::login(const string& name, const string& password)
{
    /* 登入成功返回 1，已登录返回 0，登入失败返回-1*/

    /* check if online */
    set<string> test;
    if ((find(OnlineUserList.begin(), OnlineUserList.end(), name)) != OnlineUserList.end())
        return 0;

    /* check cosistency */
    User temp = IO::getUser(name);
    if (temp.getPassword() == password) {
        OnlineUserList.push_back(temp.getName());
        if (temp.getType() == PLAYER) {
            return PLAYER;
        }
        if (temp.getType() == BUILDER) {
            return BUILDER;
        }
    }
    return 0;
}

int System::logout(const string& name)
{
    OnlineUserList.erase(remove(OnlineUserList.begin(), OnlineUserList.end(), name), OnlineUserList.end());
    return 1;
}

int System::enroll(const string& name, string password, USERTYPE type)
{
    if (IO::getUser(name).getName() != "0") // 检查是否已注册
        return 0; // 已注册，注册失败
    User user(name, std::move(password), type);
    if (type == PLAYER) {
        Player player(user);
        IO::addPlayer(player);
    }
    if (type == BUILDER) {
        Player builder(user);
        IO::addBuilder(builder);
    }
    return 1;
}

int System::addWord(const string& word, const string& builderName)
{
    IO::addWord(word);
    Builder builder = IO::getUser(builderName);
    builder.plusExp(word.length());
    IO::UpdateBuilder(builder);
    return 1;
}

/* module for Player */
double System::playGame(int level, const string& playerName)
{
    Player player = IO::getPlayer(playerName);
    if (level == 0)
        level = player.getLevel();
    else {
        level--; //因为去取单词下标从 0 开始
    }
    GameLevel gameLevel = IO::getGameLevel(level);
    Game game(gameLevel);
    double timeUsage = game.playGame();
    if (timeUsage) { // timeUsage 不为零，即说明闯关成功
        // 玩家升级策略 f(闯关速度,关卡难度）（象征性的升级策略）
        // 直接取其倒数，越快越高. 一般而言，timeusage 在十秒左右。
        player.plusExp(level, timeUsage);
        IO::UpdatePlayer(player);
    }
    return timeUsage;
}

double System::playGame(int level, const string& playerName, vector<string> wordListFromServer)
{
    Player player = IO::getPlayer(playerName);
    if (level == 0)
        level = player.getLevel();
    else {
        level--; //因为去取单词下标从 0 开始
    }

    GameLevel gameLevel(level, wordListFromServer);
    Game game(gameLevel);
    double timeUsage = game.playGame();

    if (timeUsage) { // timeUsage 不为零，即说明闯关成功
        // 玩家升级策略 f(闯关速度,关卡难度）（象征性的升级策略）
        // 直接取其倒数，越快越高. 一般而言，timeusage 在十秒左右。
        player.plusExp(level, timeUsage);
        IO::UpdatePlayer(player);
    }
    return timeUsage;
}