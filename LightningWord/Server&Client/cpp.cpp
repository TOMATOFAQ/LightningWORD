#include <chrono>
#include <ctime>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <unistd.h>
using json = nlohmann::json;
using namespace std;
using namespace chrono;
#define SECOND 1000000

int main(void)
{
    auto start = std::chrono::system_clock::now(); // 开始计时

    int a;
    cin >> a;

    auto end = std::chrono::system_clock::now(); // 计时结束
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    cout << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << endl;
}