#include <string>
#include <vector>
using namespace std;

const char* formMessage(vector<string> stringList, string& tempString)
{
    tempString = "";
    for (string s : stringList) {
        tempString += s + ",";
    }
    return tempString.data();
}

vector<string> splitMessage(char* str)
{
    string seg_s = ",";
    char* seg = (char*)seg_s.data();
    char* substr = strtok(str, seg);
    vector<string> stringList;
    while (substr != NULL) {
        string temp = substr;
        stringList.push_back(temp);
        substr = strtok(NULL, seg);
    }
    return stringList;
}