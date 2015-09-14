#include "MCI.h"
#include <iostream>
#include <string.h>

using namespace std;

byte* MyCustomInformation:: getRootMac(){
    return this->rootMac;
}

uint32_t MyCustomInformation:: getRootIP(){
    return this->IP;
}

void MyCustomInformation:: setRootInfo(char* IP_MAC){
  //  cerr << "this is test!";
    int counter = 0;
    int i = 0;
    this->port = 8000;

    // set the root Mac address here
    for( ; counter < 6;){
        this-> rootMac[counter] = toInt(IP_MAC[i])*16 + toInt(IP_MAC[i + 1]);
        cerr << (int) this->rootMac[counter] << " ";
        i += 3;
        counter++;
    }

    cerr << "\n IP: ";
    counter = 0;
    this->IP = 0;
    // set the root IP address here
    for (;counter < 4;){
        this-> IP *= 256;
        uint8_t temp = (int) (IP_MAC[i] - '0');

        i++;
        while (IP_MAC[i] <= '9' && IP_MAC[i] >= '0'){
            temp *= 10;
            temp += (int) (IP_MAC[i] - '0');
            i++;
        }
        IP += temp;
        i++;
      //  cerr << "IP till now: " << temp << " " << IP << endl;
        counter++;
    }


    // set the pointer to the start of record database here
    this->database = &(IP_MAC[i]);

    /// set the data base here 2 sequential strings in vectors are for one record
    char *record;
    vector<string> temp;
  //  printf ("Splitting string \"%s\" into tokens:\n", database);
    record = strtok (database, ",");
    while (record != NULL)
    {
        printf ("%s\n", record);
        string s = record;
        temp.push_back(s);
        record = strtok (NULL, ",");
    }

    for (uint i = 0; i < temp.size(); i++){
        char* record =  &temp.at(i)[0];
        char *record_type = strtok(record, " ");
        char *record_first = strtok(NULL, " ");
        char *record_second = strtok(NULL, " ");
        if (record_type[0] == 'A'){
     //       cerr << "A type " << record_second <<" "<< record_first << endl;
            this->A_Records.push_back(record_first);
            this->A_Records.push_back(record_second);
        }else if (record_type[0] == 'C'){
            this->CNAME_Records.push_back(record_first);
            this->CNAME_Records.push_back(record_second);
       //     cerr << "Cname type\n";
        }else if (record_type[0] == 'N'){
            this->NS_Records.push_back(record_first);
            this->NS_Records.push_back(record_second);
         //   cerr << "NS type\n";
        }

    }

 //   free(temp);

}

bool MyCustomInformation:: compare(string a, string b){
  //  cerr << "comparing " << a << " " << b;
    if (a.length() != b.length())
        return false;
    for (uint i = 0; i < a.length(); i++){
        if (a[i] != b[i])
            return false;
    }
    return true;
}

struct answer_record MyCustomInformation:: find_max_match(string domain){
  //  char* res = &domain[0];
    answer_record res;
    char* str;
    res.len = 0;
    int NS_ind = -1;
    int max_NS = 0;
    res.len = 0;
    int t = strlen(&domain[0]);
    if (t < 1)
        return res;
    if (domain[t - 1] != '.')
        return res;
  //  int max_length = 0;

    //search in A type
    for (uint i = 0; i < this->A_Records.size(); i+= 2){
        if (same_size(A_Records.at(i), domain)){
            str = &this->A_Records.at(i + 1)[0];
            res.name = "A";
            res.len = 1;
            res.str = str;
            res.record = &this->A_Records.at(i)[0];
            cerr << "found A record";
            return res;
        }
    }
     for (uint i = 0; i < this->CNAME_Records.size(); i+= 2){
        if (same_size(CNAME_Records.at(i), domain)){
            str = &this->CNAME_Records.at(i + 1)[0];
            res.name = "CNAME";
            res.len = 1;
            res.str = str;
            res.record = &this->CNAME_Records.at(i)[0];
            cerr << "found CNAME record";
            return res;
        }
    }

    // search in NS type
    for (uint i = 0; i < this->NS_Records.size(); i+=2){
        int t = max_match(NS_Records.at(i), domain);
        if (t > max_NS){
            max_NS = t;
            NS_ind = i;
        }
    }

    // search in cname type

    if (max_NS > 0){
       str = &this->NS_Records.at(NS_ind + 1)[0];
        res.name = "NS";
        res.len = 1;
        res.str = str;
        res.record = &this->NS_Records.at(NS_ind)[0];
        cerr << "found NS record";
        return res;
    }
    return res;
}

bool MyCustomInformation:: same_size(string a, string b){
    vector<string> splitted_b;

  //  cerr << "Splitting string" << b << " into tokens:\n";
    char* record = strtok (&b[0], ".");
    while (record != NULL)
    {
       // printf ("%s\n", record);
        string s = record;
        splitted_b.push_back(s);
        record = strtok (NULL, ".");
    }

        vector<string> splitted_a;
    //    cerr << "Splitting string " << a << " into tokens:\n";
        record = strtok (&a[0], ".");
        while (record != NULL)
        {
  //      printf ("%s\n", record);
            string s = record;
            splitted_a.push_back(s);
            record = strtok (NULL, ".");
        }

      //  cerr << "number of .: " << splitted_a.size() << endl;

//    cerr << "number of .: " << splitted_b.size() << endl;

        if (splitted_a.size() != splitted_b.size())
            return false;


        for (uint i = 0; i < splitted_a.size(); i++){
            if (!compare(splitted_a.at(splitted_a.size() - 1 - i), splitted_b.at(splitted_b.size() - 1 - i))){
              //  cerr << " same" << endl;
                return false;
            }
            //cerr << " were not same" << endl;

        }
        return true;
}
// number of .x parts that matches a = domain    b = query
int MyCustomInformation:: max_match(string a, string b){

    vector<string> splitted_b;

  //  cerr << "Splitting string" << b << " into tokens:\n";
    char* record = strtok (&b[0], ".");
    while (record != NULL)
    {
       // printf ("%s\n", record);
        string s = record;
        splitted_b.push_back(s);
        record = strtok (NULL, ".");
    }

        vector<string> splitted_a;
    //    cerr << "Splitting string " << a << " into tokens:\n";
        record = strtok (&a[0], ".");
        while (record != NULL)
        {
  //      printf ("%s\n", record);
            string s = record;
            splitted_a.push_back(s);
            record = strtok (NULL, ".");
        }

      //  cerr << "number of .: " << splitted_a.size() << endl;

//    cerr << "number of .: " << splitted_b.size() << endl;

        if (splitted_a.size() > splitted_b.size())
            return -1;
        else if (splitted_a.size() ==  splitted_b.size()){
            if (!same_size(a, b))
                return -1;
            else
                return splitted_a.size();
        }
        for (uint i = 0; i < splitted_a.size(); i++){
            if (compare(splitted_a.at(splitted_a.size() - 1 - i), splitted_b.at(splitted_b.size() - 1 - i))){
              //  cerr << " same" << endl;
            continue;
            }
            //cerr << " were not same" << endl;
            return -1;
        }

      //  cerr << "max_match : " << splitted_a.size() << " \n";
        return splitted_a.size();


}

void MyCustomInformation:: tester(){
    for (int i = 0; i < 10; i++){
      //  cerr << this->database[i];
    }

}
void MyCustomInformation:: addPort(){
    this->port ++;
}

uint16_t MyCustomInformation:: getPort(){
    uint16_t res = this->port;
    return res;
}

uint8_t MyCustomInformation:: toInt(char first){
  //  cerr << "Inside int: "<< first << " ";
    if (first <= '9' && first >= '0'){
        uint8_t res = first - '0';
        return res;
    }else if(first <= 'Z' && first >= 'A'){
        return first -'A' + 10;
    }else{
        return first - 'a' + 10;
    }
}



