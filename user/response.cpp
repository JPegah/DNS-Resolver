#include <string>
#include <stdlib.h>
#include <iostream>
#include "response.h"
using namespace std;
void responsed:: set_final(int a){
            final = a;
        }
void responsed::set_name(char* t, int length){
  //  cerr << "setting the name" << t << length<< endl;
            len = length;
            name = (char*) malloc(len + 1 * sizeof(char));
            int i = 0;
            for (; i < len; i++){
                *(name + i) = *(t + i);
            }
            *(name + i) = '\0';
        }

        void responsed::set_record(char* t, int length){
  //  cerr << "setting the name" << t << length<< endl;
            record_len = length;
            record = (char*) malloc(record_len + 1 * sizeof(char));
            int i = 0;
            for (; i < record_len; i++){
                *(record+ i) = *(t + i);
            }
            *(record + i) = '\0';
        }

        int responsed:: get_final(){
            return final;
        }

        uint32_t responsed:: get_ip(){
            return ip;
        }
        void responsed:: set_ip(uint32_t a){
            ip = a;
        }

        int responsed::get_len(){
            return len;
        }

        char* responsed::get_name(){
            return name;
        }
        char *responsed::get_record(){
            return record;
        }


