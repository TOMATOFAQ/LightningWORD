#include "Basic.h"

class GameLevel {
  public:
    GameLevel(int level, vector<string> wordList);
    vector<string> getWordsList() { return wordList; }
    int getLevel() { return level; }

  private:
    vector<string> wordList;
    int level;
};

GameLevel::GameLevel(int level, vector<string> wordList)
    : level(level), wordList(wordList) {}