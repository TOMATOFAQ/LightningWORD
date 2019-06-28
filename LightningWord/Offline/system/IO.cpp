#include <utility>

#include "Basic.h"
#include "GameLevel.cpp"
#include "User.cpp"

// 必须保证访问一致性。然而 json 文件并不能在类里面建立，所以放到全局函数中。（大概是因为其实现机制吧。）
static json UserList;
static json WordList;

string filpString(string password) // 密码转换机制，禁止明文存储
{
  for (char& c : password) {
    c = 128 - c;
  }
  return password;
}

class IO {
private:
    static void load();
    static void refurbish();

public:
    IO() { load(); }

    /* for all */
    // 选择返回两个榜。其一为player榜，其二为 builder 榜。
    static bool compUserByExp(const User& a, const User& b)
    {
        return a.getExp() > b.getExp();
    }
    static vector<vector<User>> getLeaderboard();

    /* for User */
    static User getUser(const string& name);
    static vector<User> getUserbyAttribute(const string& attribute, const string& value);
    static Player getPlayer(const string& name);
    static Builder getBuilder(const string& name);
    static void UpdatePlayer(Player player);
    static void UpdateBuilder(Builder builder);
    static void addPlayer(Player player);
    static void addBuilder(Builder builder);
    static void deleteUser(const string& name);

    /* for Level */
    static void addWord(string word);
    static string getWord(int level);
    static GameLevel getGameLevel(int level);
};

/* machanism of IO */
void IO::load()
{
    /* load User list */
    ifstream ifstream_UserList("UserList.json");
    ifstream_UserList >> UserList;
    /* load Word list */
    ifstream ifstream_WordList("WordList.json");
    ifstream_WordList >> WordList;
}

void IO::refurbish()
{
    //将当前文件写进后又读入。（事实上不需要 load 了，因为就刚写进去，一定是一样的）
    /* refurbish User list */
    ofstream ofstream_UserList("UserList.json");
    ofstream_UserList << UserList << endl;
    /* refurbish Word list */
    ofstream ofstream_WordList("WordList.json");
    ofstream_WordList << WordList << endl;
    load();
}

vector<vector<User>> IO::getLeaderboard()
{
    vector<User> leaderboardOfPlayer = getUserbyAttribute("type", to_string(PLAYER));
    sort(leaderboardOfPlayer.begin(), leaderboardOfPlayer.end(), compUserByExp);
    vector<User> leaderboardOfBuilder = getUserbyAttribute("type", to_string(BUILDER));
    sort(leaderboardOfBuilder.begin(), leaderboardOfBuilder.end(), compUserByExp);
    vector<vector<User>> leaderboard;
    leaderboard.push_back(leaderboardOfPlayer);
    leaderboard.push_back(leaderboardOfBuilder);
    return leaderboard;
}
/* for User */
User IO::getUser(const string& name)
{
    if (UserList[name].is_null()) {
        User user("0", "0", 0, 0, 0);
        return user;
    } else {
        string password = filpString(UserList[name]["password"]);
        USERTYPE type = UserList[name]["type"];
        User user(name, password, type, UserList[name]["level"].get<int>(), UserList[name]["exp"].get<int>());
        return user;
    }
}

vector<User> IO::getUserbyAttribute(const string& attribute, const string& value)
{
    vector<User> targetUserList;
    if (attribute == "name") {
        targetUserList.push_back(getUser(value));
        if (targetUserList[0].getName() == "0")
            targetUserList.pop_back();
    } else { // 除了名字其他确实都是数字
        int valueInteger = stoi(value);
        for (const auto& el : UserList.items()) {
            if (el.value()[attribute] == valueInteger)
                targetUserList.push_back(getUser(el.key()));
        }
    }
    return targetUserList;
}

Player IO::getPlayer(const string& name)
{
    User user = getUser(name);
    int success = UserList[name]["success"];
    int failure = UserList[name]["failure"];
    int speed = UserList[name]["speed"];
    Player player(user, success, failure, speed);
    return player;
}

Builder IO::getBuilder(const string& name)
{
    User user = getUser(name);
    Builder builder(user);
    return builder;
}

void IO::UpdatePlayer(Player player)
{
    addPlayer(std::move(player));
}

void IO::UpdateBuilder(Builder builder)
{
    addBuilder(std::move(builder));
}

void IO::addPlayer(Player player)
{
    UserList[player.getName()] = {
        { "password", filpString(player.getPassword()) },

        { "success", player.getSuccess() },
        { "failure", player.getFailure() },
        { "speed", player.getSpeed() },

        { "level", player.getLevel() },
        { "exp", player.getExp() },
        { "type", player.getType() },
    };
    refurbish();
}

void IO::addBuilder(Builder builder)
{
    UserList[builder.getName()] = {
        { "password", filpString(builder.getPassword()) },
        { "level", builder.getLevel() },
        { "exp", builder.getExp() },
        { "type", builder.getType() },
    };
    refurbish();
}

void IO::deleteUser(const string& name)
{
    UserList.erase(name);
    refurbish();
}

/* for Level */
void IO::addWord(string word)
{
    int gradeStandard = 2; // 以2个字符来分级
    int totalLevelNum = 9; // 一共分为 10 级,即最高级的最短单词为 45
    int wordLevel = (word.length() / gradeStandard) % totalLevelNum;
    WordList[wordLevel].push_back(word);
    refurbish();
}

string IO::getWord(int level)
{
    return WordList[level][rand() % (WordList[level].size())];
}

GameLevel IO::getGameLevel(int level)
{
    int wordnum = level + 2; // 单词数量与关卡难度有关


    set<string> stringSet;
    while(stringSet.size()<wordnum){
      stringSet.insert(getWord(level));
    }


  vector<string> stringList;
  stringList.reserve(wordnum); // 优化相关，提前分配空间

  for (string word:stringSet) {
        stringList.push_back(word);
    }

    GameLevel gameLevel(level, stringList);
    return gameLevel;
}




//
//#define DEBUG 1
//int main()
//{
//    IO io;
//    string target = "level";
//    vector<User> list = io.getUserbyAttribute("level", "0");
//    for (User u : list)
//        u.present();
//}
