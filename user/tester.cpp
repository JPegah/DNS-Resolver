#include <iostream>
#include <map>
#include <condition_variable>
#include <mutex>


class test{
    private:
    mutex local;
    map<uint16_t, condition_variable> port_to_thread;
    uint16_t last_port;

    public:

    void add_to_map(){
        condition_variable cv;{
    }
    }
};
