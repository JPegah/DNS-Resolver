
#include <string.h>
#include <stdlib.h>
#include <vector>
#ifndef _My_Custom_Info_
#define _My_Custom_Info_
#include "partovdef.h"

using namespace std;
struct answer_record{
    int len;
    int final;// if it is final answer in answer packets
    string name;
    string str;
    string record;
    char* tmp;

};



class MyCustomInformation  {
private:

    byte rootMac[6];
    vector<string> NS_Records;
    vector<string> A_Records;
    vector<string> CNAME_Records;
    uint32_t IP;
    char *database;
    uint16_t port;
public:
        void setRootInfo(char* IP_MAC);

        uint32_t getRootIP();
        byte* getRootMac();
        void tester();
        uint8_t toInt(char first);

        // startgin from 8000 needs mutex
        void addPort();
        uint16_t getPort();


        // returns the type and the string with maximum length match
        answer_record find_max_match(string domain);
        int max_match(string a, string b);
        bool compare(string a, string b);
        bool same_size(string a, string b);

};
#endif /* sm.h */

