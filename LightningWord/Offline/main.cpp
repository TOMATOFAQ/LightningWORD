#include "system/Interface.cpp"
#include <iostream>
//#include "system/server.cpp"
#ifndef DEBUG
int main()
{
    Interface interface;
    while (interface.showMenu()) {
    }
    return 0;
}
#endif