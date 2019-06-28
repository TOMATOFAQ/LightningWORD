#include "Basic.h"

class User {
public:
    User(string name, string password, USERTYPE type, int level = 0, int exp = 0)
        : name(name)
        , password(password)
        , type(type)
        , level(level)
        , exp(exp) {};

    // 属性访问接口
    string getName() { return name; }
    string getPassword() { return password; }
    int getLevel() { return level; }
    int getExp() const { return exp; }
    USERTYPE getType() { return type; }

    // 属性管理接口
    virtual void ExpPlusPlus(int l =0, double timeUsage  =0)  // 默认经验值加一 // 将此设置为虚函数
    {
        exp++;
        level = log(exp); // 如果达到升级条件，自动升级
    }

    void plusExp(int level =0, double timeUsage  =0){
      ExpPlusPlus();
    }; // 根据条件增加经验值


    void present()
    {
        cout << "User: " << name << endl;
        cout << "level: " << level << endl;
        cout << "exp: " << exp << endl;
        cout << "type: " << ((type == PLAYER) ? "player" : "bulider") << endl;
        cout << endl;
    }


private:
    string name;
    string password;
    int level = 0;
    int exp = 0;
    double speed;
    USERTYPE type;
};

class Player : public User {
public:
    Player(User user, int success = 0, int failure = 0, double speed = 0)
        : User(user.getName(), user.getPassword(), user.getType(), user.getLevel(), user.getExp())
        , success(success)
        , failure(failure)
        , speed(speed) {};
    //    属性接口函数
    int getSuccess() { return success; }
    int getFailure() { return failure; }
    int getPassLevelNum() { return success + failure; }
    double getSpeed() { return speed; }

  void present()
  {
      User::present();
      cout << "success: " << success << endl;
      cout << "failure: " << failure << endl;
       cout << "speed: " << speed << endl;
  }

  void ExpPlusPlus(int level,double timeUsage) //根据关卡等级和闯关时间升级
  {
    User * fatherPtr = this;
    for (int i = 0; i < level+1; i++) {
      if (timeUsage) { // timeUsage 不为零，即说明闯关成功
        // 玩家升级策略 f(闯关速度,关卡难度）（象征性的升级策略）
        // 直接取其倒数，越快越高. 一般而言，timeusage 在十秒左右。
        for (int i = 0; i < (100 / timeUsage) * (level + 1); i++) {
          User::ExpPlusPlus();
        }
      }
    }
  }

private:
    int success;
    int failure;
    double speed;
};

class Builder : public User {
public:
    Builder(User user)
        : User(user.getName(), user.getPassword(), user.getType(), user.getLevel(), user.getLevel()) {};
    //    属性接口函数
    int getWordsNum() { return getExp(); } // 其实就是经验值。以此封装

    void ExpPlusPlus(int wordLength,double timeUsage=0)
    {
      User * fatherPtr = this;
      for (int i = 0; i < wordLength; i++) {
        User::ExpPlusPlus();
      }
    }


private:
};
