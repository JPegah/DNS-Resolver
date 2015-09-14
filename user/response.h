#include <string>
#include <iostream>

using namespace std;
class responsed  {
private:
    char* name;
    uint32_t ip;
    int len;
    int final;
    char* record;
    int record_len;
public:
        void set_final(int a);
        void set_name(char* t, int length);
        void set_record(char*t, int length);
        int get_final();
        int get_len();
        void set_ip(uint32_t a);
        uint32_t get_ip();

        char* get_name();
        char* get_record();

};

