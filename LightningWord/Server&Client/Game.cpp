#include "IO.cpp"
#include <chrono>
#include <ctime>
#include <mutex>
#include <thread>
#define MAX_TIME 5

class Game /*logic*/ {
private:
    std::mutex mut; // 进程锁
    GameLevel gameLevel;
    vector<string> wordList;
    int level;
    int timePerWord;

public:
    explicit Game(GameLevel gameLevel)
        : gameLevel(gameLevel)
        , level(gameLevel.getLevel())
        , wordList(gameLevel.getWordsList())
        , timePerWord((level + 1) * 3)
    {
    }

    void moveTheTime(int* timePtr)
    {
        while (*timePtr) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            mut.lock();
            (*timePtr)--;
            mut.unlock();
        }
    }

    void readInput(string* inputPtr, int* ifInput)
    {
        // NOTE:debug了一个一万年的bug，为什么不可以直接向指针指向的地址写内容？直接写内容导致只读入第一个字符
        //        cin.clear();
        //        cin.ignore();
        string temp;
        cin >> temp; // TODO: 如何可以只识别单个回车。即，让 cin 可以不忽略空白字符
        *inputPtr = temp;
        *ifInput = 1;
    }

    static int cleanTerminal()
    {
#if DELAY
        /* 用来清理终端界面 */
        for (int i = 0; i < 3; i++)
            cout << "\033[2J\033[1; 1H" << endl;
#endif
        return 1;
    }

    static void drawDots(int n)
    {
#if DELAY
        for (int i = 0; i < n; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            cout << "." << flush;
        }
        cout << endl;
#endif
    }

    double playGame()
    { // 返回游戏时长，并作为一个评分依据

        cout << "Welcome." << endl;
        int wordOrder = 0;
        int ifSuccess = 1;
        double timeUsage;
        int wordNum = wordList.size();

        while (!wordList.empty()) {
            // 获得单词
            wordOrder++;
            string word = wordList.back();
            wordList.pop_back();

            // 显示单词并记忆
            int memoryTime = (level + 1) * 2;
            cout << "Round:" << wordOrder << " Current word is " << word << " .You have " << memoryTime << " seconds to remenber it." << endl;
            drawDots(memoryTime);
            cleanTerminal(); // 清空界面，进行答题

            cout << "[Now, time for your answer!!!]";

            // 初始化输入.清空缓冲区
            auto start = std::chrono::system_clock::now(); // 开始计时
            string input;
            cin.clear();
            cin.ignore();
            cin >> input;

            if (input == word) {
                cout << "Good job!" << endl;
            } else {
                cout << "Your input is " << input << " while current word is " << word << endl;
                ifSuccess = false;
                break;
            }
            auto end = std::chrono::system_clock::now(); // 计时结束
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            timeUsage += double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
        }

        if (!ifSuccess) {
            timeUsage = 0;
        } else {
            timeUsage /= wordNum; // 求每个单词的平均答题时间
        }
        return timeUsage;
    }

    double playGameWithTimeLimit()
    { // 返回游戏时长，并作为一个评分依据

        cout << "Welcome." << endl;
        int wordOrder = 0;
        int ifSuccess = 1;
        double timeUsage = 0;
        int wordNum = wordList.size();

        while (!wordList.empty()) {
            // 获得单词
            wordOrder++;
            string word = wordList.back();
            wordList.pop_back();

            // 显示单词并记忆
            int memoryTime = (level + 1) * 2;
            cout << "Round:" << wordOrder << " Current word is " << word << " .You have " << memoryTime << " seconds to remenber it and " << timePerWord << " to answer." << endl;
            drawDots(memoryTime);
            cleanTerminal(); // 清空界面，进行答题

            cout << "[Now, time for your answer!!!]";

            // 初始化输入.清空缓冲区

            // 初始化输入进程
            string input = "";
            string* inputPtr = &input;
            int ifInput = 0;
            int* ifInputPtr = &ifInput;
            thread thread_readInput(&Game::readInput, this, inputPtr, ifInputPtr);
            thread_readInput.detach();

            // 初始化同步进程
            int TIME = timePerWord;
            int* timePtr = &TIME;
            thread thread_moveTheTime(&Game::moveTheTime, this, timePtr);
            //            thread_moveTheTime.detach();

            auto start = std::chrono::system_clock::now(); // 开始计时

            while (TIME) {
                if (input.empty()) {
                } else if (input == word) {
                    cout << "Good job!" << endl;
                    break;
                } else {
                    cout << "your input is " << input << " while current word is " << word << endl;
                    ifSuccess = false;
                    break;
                }
            }

            if (!TIME) {
                cout << "The time is Over!" << endl;
                ifSuccess = 0;
            }

            auto end = std::chrono::system_clock::now(); // 计时结束
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            timeUsage += double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;

            thread_moveTheTime.join();
            if (!ifInput) { // 情况是已经输入了但是还没有结束。如果已结束，那么已经销毁。不能再销毁一个已销毁的对象。
                cout << "Please Input sth to end this game." << endl;
                while (!ifInput) {
                }
            }

            if (!ifSuccess)
                break;
        }

        if (!ifSuccess) {
            timeUsage = 0;
        } else {
            timeUsage /= wordNum; // 求每个单词的平均答题时间
        }
        return timeUsage;
    }
};

//#define DEBUG 1
//int main()
//{
//    IO io;
//    cout << "加载完毕" << endl;
//    GameLevel gameLevel = io.getGameLevel(0);
//    Game game(gameLevel);
//    cout << game.playGameWithTimeLimit();
//}