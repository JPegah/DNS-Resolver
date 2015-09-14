//                   In the name of GOD
/**
 * Partov is a simulation engine, supporting emulation as well,
 * making it possible to create virtual networks.
 *
 * Copyright © 2009-2014 Behnam Momeni.
 *
 * This file is part of the Partov.
 *
 * Partov is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Partov is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Partov.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include "sm.h"
#include <utility>
#include "interface.h"
#include "frame.h"
#include <chrono>
#include <netinet/in.h>
#include "MCI.h"
#include <sstream>
#include <cstring>
#include "sr_protocol.h"
#include <thread>
#include <pthread.h>
#include <atomic>

using namespace std;

SimulatedMachine::SimulatedMachine (const ClientFramework *cf, int count) :
	Machine (cf, count) {
	// The machine instantiated.
	// Interfaces are not valid at this point.
}

SimulatedMachine::~SimulatedMachine () {
	// destructor...
}

void SimulatedMachine::initialize () {
	// TODO: Initialize your program here; interfaces are valid now.
	string info = getCustomInformation();

	this->additionalInfo.setRootInfo(&info[0]);
	this->additionalInfo.tester();
	this->last_allocated_port = 8000;
	responsed s;
	uint16_t a = 1;
	//port_to_answer.insert(pair<uint16_t, string>(a, s));
	//port_to_answer.emplace(a, &s);
   // port_to_answer();

   // std::map<uint16_t,responsed*>::iterator it = port_to_thread.begin();
 //   port_to_thread.insert (it, std::make_pair(a,&s));  // max efficiency inserting
     port_to_answer.insert(std::pair<uint16_t,responsed*>(a, &s));
	//this->rs = (responsed*) malloc(sizeof(uint32_t) * 2 + 2);
    //rs->set_final(3);
}
bool SimulatedMachine:: is_valid(byte* data, uint len){
 struct sr_ethernet_hdr *eth = (struct sr_ethernet_hdr*) data;
    /// gereftane mace khodemum va maci ke barash bayad pas befrestim??
    if (len < sizeof(struct sr_ethernet_hdr) + sizeof(struct sr_udp) + sizeof(struct ip) + sizeof(struct sr_dns))
        return false;
    if (ntohs(eth->ether_type) != ETHERTYPE_IP)
        return false;



    struct ip *ip_hdr = (struct ip*)(data + sizeof(sr_ethernet_hdr));
    if (ip_hdr->ip_p != IPPROTO_UDP)
        return false;

    if ((int) ip_hdr->ip_v != 4)
        return false;


    if (ntohs(ip_hdr->ip_len) != (len - sizeof(struct sr_ethernet_hdr)))
        return false;

    if (ntohl(ip_hdr->ip_dst.s_addr) != iface[0].getIp())
        return false;


   uint ip_hdr_size = sizeof(struct ip);
   if (ip_hdr->ip_hl < 5)
        return false;
    if (ip_hdr->ip_hl > 5)
        ip_hdr_size += (ip_hdr->ip_hl - 5) * sizeof(uint32_t);
   /// hl ham baghi munde ke ba baghie sink shavad

    struct sr_udp *udp = (struct sr_udp*) (data + sizeof(struct sr_ethernet_hdr) + ip_hdr_size);
  //  cerr << ntohs(udp->length) << " ---............................" << len - sizeof(struct sr_ethernet_hdr) - sizeof(struct ip) << endl;
    if (ntohs(udp->length) != (len - sizeof(struct sr_ethernet_hdr) - ip_hdr_size))
        return false;


    struct sr_dns *dns = (struct sr_dns*)(data + sizeof(struct sr_ethernet_hdr) + ip_hdr_size + sizeof(struct sr_udp));

    // cerr << "----------------IS still valid------------" << endl;
    if ((int) dns->header % 2 == 0){ // is Query
        if ((int) dns->header > 2)
            return false;
        if (ntohs(udp->port_dst) != 53)
            return false;
        if (ntohs(udp->port_src) < 8000 && ntohs(udp->port_src) > last_allocated_port)
            return false;
       // cerr << "--------------------Query is valid ---------" << endl;
    }else{ // is respond
        if (ntohs(udp->port_src) != 53)
            return false;
        if (ntohs(udp->port_dst) < 8000 || ntohs(udp->port_dst) > last_allocated_port)
            return false;
        int i = (int) dns->header;
        if (i % 4 != 1)
            return false;
      //  cerr << "---------------------anwer till port is valid------" << endl;
        i /= 4;
        if (i == 1 || i == 8){
        }else{
            return false;
        }

        if (len < sizeof(struct sr_ethernet_hdr) + sizeof(struct sr_udp) + ip_hdr_size + sizeof(struct sr_dns) + 2)
            return false;

           //      cerr << "---------------------anwer till port is valid------" << endl;



        uint16_t * type_ptr = (uint16_t *) (data + sizeof(struct sr_ethernet_hdr) + sizeof(struct sr_udp) + ip_hdr_size + sizeof(struct sr_dns));
        uint16_t record_type = ntohs(*type_ptr);
       // cerr << "record type in valid ---------^^^^^^^^^^^^^[" << record_type << endl;
        if (record_type == 1) {// a type response
             //cerr << "-------------------------A record type recieved valid" << endl;
            if (i != 1)
                return false;
        //     cerr << "-------------------------A record type recieved valid" << endl;
            if (len < sizeof(struct sr_ethernet_hdr) + sizeof(struct sr_udp) + ip_hdr_size + sizeof(struct sr_dns) + 4)
                return false;
         //   cerr << "-------------------------A record type recieved valid" << endl;
        }else if (record_type == 2){// NS record
            if (i != 8)
                return false;
            if (len < sizeof(struct sr_ethernet_hdr) + sizeof(struct sr_udp) + ip_hdr_size + sizeof(struct sr_dns) + 2)
                return false;
            // cerr << "-------------------------NSrecord type recieved valid" << endl;
        }else if (record_type == 5){ // CNAME record
            if (len < sizeof(struct sr_ethernet_hdr) + sizeof(struct sr_udp) + ip_hdr_size + sizeof(struct sr_dns) + 1)
                return false;
            if (i != 1)
                return false;
                // cerr << "-------------------------CNAMErecord type recieved valid" << endl;
        }
        else {
            return false;
        }
    }

    return true;
}
void SimulatedMachine::processFrame (Frame frame, int ifaceIndex) {
	// TODO: process the raw frame; frame.data points to the frame's byte stream
	cout << "Frame received at iface " << ifaceIndex <<
		" with length " << frame.length << endl;


    /// tabeyi ke valid budane packet ro tayin kone
    byte *tmp  = new byte[frame.length];
    int frame_length = frame.length;

    copyFrame(frame.data, tmp, frame.length);


 //   printFrame(tmp, frame.length);

    /* check kardane kollie in ke baste dorost ast ya na -------------*/
    //struct sr_ethernet_hdr *eth = (struct sr_ethernet_hdr*) tmp;


    struct ip *ip_hdr = (struct ip*)(tmp + sizeof(sr_ethernet_hdr));

    int ip_size = sizeof(struct ip);
    if (ip_hdr->ip_hl > 5)
        ip_size += (ip_hdr->ip_hl - 5) * sizeof(uint32_t);
    struct sr_udp *udp = (struct sr_udp*) (tmp + sizeof(struct sr_ethernet_hdr) + ip_size);

 //     cerr << "Frame is valid: " << ip_hdr->ip_sum <<  "," << htons(sum) << endl;
      uint16_t last_sum = ntohs(ip_hdr->ip_sum);
      ip_hdr->ip_sum = 0;
     // cerr << "Frame is valid: " <<  last_sum <<  "," << ip_sum_calc((ip_hdr->ip_hl)*4,(uint8_t*)(ip_hdr)) << endl;
    if ( last_sum  != ip_sum_calc((ip_hdr->ip_hl)*4,(uint8_t*)(ip_hdr)))
        return;
   // cerr << "frame is valid?" << is_valid(tmp, frame_length) << endl;
    if (!is_valid(tmp, frame_length))
        return;

    cerr << "frame is valid" << endl;
    /*- ------------------ ta in ja farz kardam ke baste hatta DNS ash dorost ast------------*/
    // hala in ja basteye DNS bar asase modeli ke dashte daste band ip va portesh baraye ersale basteye bargasht ahamiat dare

    struct sr_dns *dns = (struct sr_dns*)(tmp + sizeof(struct sr_ethernet_hdr) + ip_size + sizeof(struct sr_udp));
    uint16_t header = (int) dns->header;
  //  cerr << "dns header: " << header << " +" << (int) dns-> header;

    if (header % 2 == 0){ // it is query
       // cerr <<"\n recieved query :";
        if (header % 4 == 0){ // iterative query
         //   cerr << "iterative";
            char* packetInfo = (char *) (tmp + sizeof(struct sr_ethernet_hdr) + ip_size + sizeof(struct sr_udp) + sizeof(struct sr_dns));
            answer_record res = this->additionalInfo.find_max_match(packetInfo);
            if (res.len == 1){
                cerr << "found answer for this query:[" << packetInfo << "] " << res.record << " "<< res.str<< endl;
            }else{
                cerr << "couldnt make answer for "<< packetInfo  << endl;
                return;}
         ///   if (res.len == 0)
            uint8_t dns_respond;
            char* first_packet = new char[strlen(&res.record[0]) + 1];
            char*second_packet = new char[strlen(&res.str[0]) + 1];
            bool a_record = false;
            uint16_t record_type;
            if (res.name[0] == 'N'){
                dns_respond = 33; // htons
                record_type = 2;

            }else if (res.name[0] == 'C'){
                dns_respond = 5;
                record_type = 5;

            }else{
                dns_respond = 5;
                record_type = 1;
                a_record = true;
            }

            for (uint i = 0; i < strlen(&res.record[0]); i++){
                *(first_packet + i) = res.record[i];
            }
            *(first_packet + strlen(&res.record[0])) = '\0';

            for (uint i = 0; i < strlen(&res.str[0]); i++){
                *(second_packet + i) = res.str[i];
            }
            *(second_packet + strlen(&res.str[0])) = '\0';
            //cerr << "records to send: " << first_packet << " " << second_packet << endl;

            if (!send_packet(ntohl(ip_hdr->ip_src.s_addr), ntohs(udp->port_dst), ntohs(udp->port_src), dns_respond, record_type, first_packet, second_packet, a_record))
                return;


        }else{ // recursive query
            //cerr << "recursive" << endl;
            char *a = (char *) (tmp + sizeof(struct sr_ethernet_hdr) + ip_size + sizeof(struct sr_udp) + sizeof(struct sr_dns));
            string str = a;
            uint32_t ip = ntohl(ip_hdr->ip_src.s_addr);
            //cerr << "thread is going to add";
            std::thread myThread (&SimulatedMachine ::resolve_recursive_respond, this, a, ip, ntohs(udp->port_src), ntohs(udp->port_dst));
            myThread.detach ();
            //cerr << "thread is added";

        }
    }else{ // this is rmesponse of query
        //cerr << "recieved response"<< endl;
        unique_lock<mutex> lk(local);

        /// in ja baraye empty budan check shavad hatman!!!! 1 ra ham bayad az baghiey in ja joda konam
        std::map<uint16_t,condition_variable*>::iterator it;
        it = port_to_thread.find(ntohs(udp->port_dst));
        //cerr << "\n------------writing answer on this port" << ntohs(udp->port_dst) << "dns header: " << (int) header<< endl;
        /// if it is not empty , ....
        condition_variable *cv = it->second;
        if(cv){
        //    cerr << "got condition variable" << endl;
            responsed* r = (responsed*) malloc(sizeof(uint32_t)*2 + sizeof(char) * (frame_length+ 1 -  (sizeof(struct sr_ethernet_hdr) + ip_size + sizeof(struct sr_udp) + sizeof(struct sr_dns))));

          //  string tr = packetInfo;
         //   byte *tmp  = new byte[frame.length];
            char* real_record = (char*)(tmp + sizeof(struct sr_ethernet_hdr) + ip_size + sizeof(struct sr_udp) + sizeof(struct sr_dns) + 2);
          //  cerr << " real record of this response is: " << real_record << endl;
            char* packetInfo = (char *) (tmp + sizeof(struct sr_ethernet_hdr) + ip_size + sizeof(struct sr_udp) + sizeof(struct sr_dns) + 3 + strlen(&real_record[0]));

           if (header == 7 || header == 5){ // A or cname

            //    cerr <<  "set final to 1" << endl;
                uint16_t* record_type_ptr = (uint16_t*)(tmp+ sizeof(struct sr_ethernet_hdr) + ip_size + sizeof(struct sr_udp) + sizeof(struct sr_dns));
                uint16_t record_type = ntohs(*(record_type_ptr));
              //  cerr << "dns header type: " << (int) record_type << endl;
         //       printFrame(tmp, frame.length);
                // in ja in ghadr bayad beravad ta be null beresad


                if ((int) record_type == 1){ // is A
                    r->set_final(1);
                //    cerr << "seting the final" << endl;
                    uint32_t* ip_ptr =(uint32_t*) (tmp + sizeof(struct sr_ethernet_hdr) + ip_size + sizeof(struct sr_udp) + sizeof(struct sr_dns) + 3 + strlen(&real_record[0]));
                    uint32_t ip_ptr_num = ntohl(*(ip_ptr));
                    r->set_ip(ip_ptr_num);
                  // cerr << "answer set for this A record: " << r->get_ip() << endl;
                }else if (record_type == 5){ // cname
                    r->set_final(2);
                    r->set_name (packetInfo, frame_length -  sizeof(struct sr_ethernet_hdr) + ip_size + sizeof(struct sr_udp) + sizeof(struct sr_dns) + 3 + strlen(&real_record[0]));
                }else{// bug
                }
                // bayad ba khundane bite aval befahme kodomeshone
            }else if (header == 33 || 35){ // NS
                r->set_final(0);
                r->set_name (packetInfo, frame_length -  sizeof(struct sr_ethernet_hdr) + ip_size + sizeof(struct sr_udp) + sizeof(struct sr_dns) + 3 + strlen(&real_record[0]));
            }else{
                //bug
            }
            r->set_record(real_record, strlen(real_record));
            port_to_answer.insert(std::pair<uint16_t,responsed*>(ntohs(udp->port_dst), r));
        //    port_to_answer.emplace(ntohs(udp->port_dst), r);
            cv->notify_one();
        }

    }
//    printFrame(tmp, frame.length);
}



bool SimulatedMachine ::send_packet(uint32_t dest_ip, uint16_t src_port, uint16_t dest_port, uint8_t dns_h, uint16_t code, string first, string second, bool second_is_ip){
        uint32_t packet_size;
        if (second_is_ip)
            packet_size = sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp) + sizeof(struct sr_dns) + 7 + strlen(&first[0]);

        else
           packet_size = sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp) + sizeof(struct sr_dns) + 2 + 2 + strlen(&first[0]) + strlen(&second[0]);


        byte* data = new byte[packet_size];

    // make ethernet header
    struct sr_ethernet_hdr *eth = (struct sr_ethernet_hdr*)  data;
    for(int i = 0; i < 6; i++){
        eth->ether_shost[i] = iface[0].mac[i];
        eth->ether_dhost[i] = this->additionalInfo.getRootMac()[i];
    }
    eth->ether_type = htons(ETHERTYPE_IP);

    // make ip header
    struct ip *ip_hdr = (struct ip*) (data + sizeof(struct sr_ethernet_hdr));
    ip_hdr->ip_tos = 0;
    ip_hdr->ip_id = 0;
    ip_hdr->ip_off = 0;
    ip_hdr->ip_ttl = 64;
    ip_hdr->ip_v = 4;
    ip_hdr->ip_hl = 5;
    ip_hdr->ip_len = htons(packet_size - sizeof(struct sr_ethernet_hdr));
    ip_hdr->ip_p = IPPROTO_UDP;
    ip_hdr->ip_src.s_addr = htonl(iface[0].getIp());
    ip_hdr->ip_dst.s_addr = htonl(dest_ip); // in ja ro dobare chek konam
    // check sum mohasebe shavad

    // make udp header
    struct sr_udp *udp = (struct sr_udp*) (data + sizeof(struct sr_ethernet_hdr) + sizeof(struct ip));
  ///  udp->port_src = htons(this->additionalInfo.getPort()); // in hamon jayie ke az 8000 shoro mishe
    udp->port_dst = htons(dest_port);
    udp->length = htons(packet_size - sizeof(struct sr_ethernet_hdr) - sizeof(struct ip));
    // mohasebeye meghdare sum

    struct sr_dns *dns = (struct sr_dns*)(data + sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp));
    dns-> header = dns_h;

    uint16_t* dns_code = (uint16_t *)(data + sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp) + 1);
    *dns_code = htons(code);

    char* first_record = (char*)(data + sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp) + 3);
    for (uint i = 0; i < strlen(&first[0]); i++){
        *(first_record + i) = first[i];
    }
    *(first_record + strlen(&first[0])) = '\0';

    if (second_is_ip){
        uint32_t* A_ip = (uint32_t *) (data + sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp) + 4 + strlen(&first[0]));
        // compute IP heare


        int counter = 0;
        int i = 0;
        uint32_t IP = 0;
        counter = 0;

        // set the root IP address here
        for (;counter < 4;){
            IP *= 256;
            uint8_t temp = (int) (second[i] - '0');

            i++;
            while (second[i] <= '9' && second[i] >= '0'){
                temp *= 10;
                temp += (int) (second[i] - '0');
                i++;
            }
            IP += temp;
            i++;
      //  cerr << "IP till now: " << temp << " " << IP << endl;
            counter++;
        }
        *A_ip = htonl(IP);
    }else{
        char* second_record = (char*)(data + sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp) + 4 + strlen(&first[0]));
        for (uint i = 0; i < strlen(&second[0]); i++){
            *(second_record + i) = second[i];
        }
        *(second_record + strlen(&second[0])) = '\0';
    }

   // cerr << "this step set the dns header to send: [" << (int) dns->header << "]"<< endl;


        udp->port_src = htons(src_port);
        udp->udp_sum = 0;
 //       udp->udp_sum = htons(udp_checksum_calc(udp->length, (uint8_t*)(&(ip_hdr->ip_src.s_addr)), (uint8_t*)(&(ip_hdr->ip_dst.s_addr)),0,(uint8_t*)udp));
        ip_hdr->ip_sum = 0;
        ip_hdr->ip_sum = htons(ip_sum_calc((ip_hdr->ip_hl)*4,(uint8_t*)(ip_hdr)));

/*fateme mozahem shode */
        Frame new_frame(packet_size, data);
     //   cerr << "answering  about: " << ntohl(*x) << " from port " << src_port << " to "<< dest_port   << " with length: " << new_frame.length << endl;
   //     printFrame(data, new_frame.length);
        /// if correctly sent ? check it
        return sendFrame(new_frame, 0);

}



void SimulatedMachine:: send_recursive_request(string dig_name){

   // uint32_t ip = this->additionalInfo.getRootIP();
    responsed *res = registerAndSend(dig_name, iface[0].getIp(), true);
//    cerr << "resolved";
    if (!res){
        cout << "Timeout!\n";
        return;
    }
    if (res->get_final() == 2){
        cout << res->get_name() << endl;
    }else if (res->get_final() == 1){
        uint32_t ip = res->get_ip();
        unsigned char c1 = 0;
        unsigned char c2 = 0;
        unsigned char c3 = 0;
        unsigned char c4 = 0;
        c1 = (ip & 0xff000000UL) >> 24;
        c2 = (ip & 0x00ff0000UL) >> 16;
        c3 = (ip & 0x0000ff00UL) >>  8;
        c4 = (ip & 0x000000ffUL) ;

        cout << (uint32_t)c1 << "." <<(uint32_t) c2 << "." << (uint32_t)c3 << "." <<(uint32_t) c4 <<" \n";

    }else{
        cout << "Timeout!\n";
    }
}
responsed* SimulatedMachine::resolve_recursive(char* name, uint32_t start_root_ip, int len){ // search from root the given string name
    uint32_t next_dns_server = start_root_ip;
    responsed* ans = registerAndSend(name, start_root_ip, false);
    //cerr << "got resoponse"<< endl;
    if (!ans)
    {
   //     cerr << "final check99999999999999999999999999 null pointer";
        return NULL;

    }
    if (ans->get_final() < 3){ // it has response
       if (ans->get_final() == 1 || ans->get_final() == 2){
            return ans;
        }else{
      //      cerr << "got NS record" << ans->get_name();
           // if (ans->get_name() == NULL){
           //     cerr << "dump will occur";
          //  string s(ans->get_name(), ans->get_len());
            responsed* ns_server = resolve_recursive(ans->get_name(), this->additionalInfo.getRootIP(), ans->get_len());
            if (!ns_server)
                return NULL;
            if (ns_server->get_final() < 3){
                if (ns_server->get_final() == 1){ /// check if is A record in ja ro bayad dorost konam
                    next_dns_server = 0;
//                    uint8_t counter = 0;
  //                  uint8_t i = 0;
        //            cerr << "in A record";
                    //char *str = ns_server->get_name();
                    next_dns_server = ns_server->get_ip();
          //          cerr << "conversion ";// << str << " to:" << next_dns_server << endl;
                    return resolve_recursive(name, next_dns_server, len);
                }else{ /// if is cname go and search for that one or ns what to do?
                }
            }

        }
    }

    //responsed* r = (responsed *) malloc(sizeof(uint32_t) * 2);
   // r->set_final(3);
    //return rs;
    return NULL;
}

void SimulatedMachine::resolve_recursive_respond(string name, uint32_t dest_ip, uint16_t dest_port, uint16_t src_port){
    //cerr << "before";
    responsed* respond = resolve_recursive(&name[0], additionalInfo.getRootIP(), strlen(&name[0]));
    //cerr << "after";
    if (!respond){
      //  cerr << "Final checking 0000000000000000000000000000 got null pointer";
        return;}
    if (respond->get_final() == 2) { // is CNAME
        // create and send packet response here
     //   string s(respond->get_name(), respond->get_len());

        send_packet(dest_ip, src_port, dest_port, 5, 5, respond->get_record(), respond->get_name(), false);
      // cerr << "final answer to recursive request if found!: " << endl;
      // cerr << respond->get_name() << endl;
    }else if (respond->get_final() == 1){ // is A record

        uint32_t ip = respond->get_ip();
        unsigned char c1 = 0;
        unsigned char c2 = 0;
        unsigned char c3 = 0;
        unsigned char c4 = 0;
        c1 = (ip & 0xff000000UL) >> 24;
        c2 = (ip & 0x00ff0000UL) >> 16;
        c3 = (ip & 0x0000ff00UL) >>  8;
        c4 = (ip & 0x000000ffUL) ;

         char buffer [50];
        int n;// a=5, b=3;
        n=sprintf (buffer, "%d.%d.%d.%d", c1, c2, c3, c4);
        char* s = &buffer[0];
        *(s + n) = '\0';
      //  cout << (uint32_t)c1 << "." <<(uint32_t) c2 << "." << (uint32_t)c3 << "." <<(uint32_t) c4 <<"  "<< n << " " << s<<"\n";

         send_packet(dest_ip, src_port, dest_port, 5, 1, respond->get_record(), s, true);
    }else{
        //cerr << "could not find answer recursive";
        // could not find the answer return then
    }

}
// sends a request frame and if the answer is recieved on time answer is returned;
// invoked when wants to send a request
responsed* SimulatedMachine:: registerAndSend(string body, uint32_t dest_ip, bool recursive){
    byte* data = new byte[sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp) + strlen(&body[0]) + 1 + sizeof(struct sr_dns)];

    // make ethernet header
    struct sr_ethernet_hdr *eth = (struct sr_ethernet_hdr*)  data;
    for(int i = 0; i < 6; i++){
        eth->ether_shost[i] = iface[0].mac[i];
        eth->ether_dhost[i] = this->additionalInfo.getRootMac()[i];
    }
    eth->ether_type = htons(ETHERTYPE_IP);

    // make ip header
    struct ip *ip_hdr = (struct ip*) (data + sizeof(struct sr_ethernet_hdr));
    ip_hdr->ip_tos = 0;
    ip_hdr->ip_id = 0;
    ip_hdr->ip_off = 0;
    ip_hdr->ip_ttl = 64;
    ip_hdr->ip_v = 4;
    ip_hdr->ip_hl = 5;
    ip_hdr->ip_len = htons(sizeof(struct ip) + sizeof(struct sr_udp) + sizeof(struct sr_dns)  +strlen(&body[0]) + 1);
    ip_hdr->ip_p = IPPROTO_UDP;
    ip_hdr->ip_src.s_addr = htonl(iface[0].getIp());
    ip_hdr->ip_dst.s_addr = htonl(dest_ip); // in ja ro dobare chek konam
    // check sum mohasebe shavad

    // make udp header
    struct sr_udp *udp = (struct sr_udp*) (data + sizeof(struct sr_ethernet_hdr) + sizeof(struct ip));
    udp->port_dst = htons(53);
    udp->length = htons(sizeof(struct sr_udp) + sizeof(struct sr_dns) + strlen(&body[0]) + 1);
    // mohasebeye meghdare sum

    struct sr_dns *dns = (struct sr_dns*)(data + sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp));
    if (recursive)
        dns->header = 2;
    else
        dns-> header = 0;


   // cerr << "this step set the dns header to send: [" << (int) dns->header << "]"<< endl;
    char* packetInfo = (char *) (data + sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp) + sizeof(struct sr_dns));

    for (uint i = 0; i < body.length(); i++){
        *(packetInfo + i) = body[i];
    }
   // packetInfo = &dig[0];
    *(packetInfo + strlen(&body[0])) = '\0';



    condition_variable cv;

    {
        unique_lock<mutex> lk (local);
        uint16_t port_number = last_allocated_port++;
        port_to_thread.insert(std::pair<uint16_t,condition_variable*>(port_number, &cv));
        //port_to_thread.emplace(port_number, &cv);

        udp->port_src = htons(port_number);
        udp->udp_sum = 0;
      //  udp->udp_sum = htons(udp_checksum_calc(udp->length, (uint8_t*)(&(ip_hdr->ip_src.s_addr)), (uint8_t*)(&(ip_hdr->ip_dst.s_addr)),0,(uint8_t*)udp));
        ip_hdr->ip_sum = 0;
        ip_hdr->ip_sum = htons(ip_sum_calc((ip_hdr->ip_hl)*4,(uint8_t*)(ip_hdr)));

/*fateme mozahem shode */
        Frame new_frame(sizeof(struct sr_ethernet_hdr) + sizeof(struct ip) + sizeof(struct sr_udp) + sizeof(struct sr_dns) + strlen(&body[0]) + 1, data);


        if (!sendFrame(new_frame, 0))
            return NULL;
        //cerr << "Asking from "<< dest_ip << " about: " << packetInfo << " on port " << port_number  << " with " << ntohs(ip_hdr->ip_sum)<<"length: " << new_frame.length << endl;
      //  printFrame(data, new_frame.length);
       // free(data);
        cv.wait_for(lk, std::chrono::seconds(30)); /// change this one remember 30
        std::map<uint16_t,responsed*>::iterator it;
        if (port_to_answer.size() <= 1){ /// dorost shavad in ghesmat
        }
        else{
            it = port_to_answer.find(port_number); // returns if the port has revieved an answer
            if (it == port_to_answer.end()){
            }
            else{
                responsed* res = it-> second;
          return res;
            }
        }
    }

    //responsed* rans;//(responsed*) malloc(sizeof(uint32_t) * 2 + 2);
    //rans->set_final(3);
  //  return this-> rs;
    return NULL;

}
//string SimulatedMachine:: resolve_recursive()





void SimulatedMachine:: printFrame(byte* data, int length) {
	cerr << hex;
	for(int i = 0; i < length; i++) {
		if(data[i] < 16) cerr << "0";
		cerr << (int)data[i];
	//	if (i % 2 == 0)
		cerr << " ";
		if (i % 20 == 0)
		cerr << endl;
	}
	cerr << dec << endl;
}
/**
 * This method will be run from an independent thread. Use it if needed or simply return.
 * Returning from this method will not finish the execution of the program.
 */
void SimulatedMachine::run () {
	// TODO: write your business logic here...
	while(true){
	    string a, b;
	    cin >> a >> b;
        send_recursive_request(b);
	}
}


/**
 * You could ignore this method if you are not interested on custom arguments.
 */
void SimulatedMachine::parseArguments (int argc, char *argv[]) {
	// TODO: parse arguments which are passed via --args
}


unsigned short  SimulatedMachine::udp_checksum_calc(unsigned int len_udp, byte src_addr[], byte dest_addr[], bool padding, byte buff[])
{
    unsigned short prot_udp=17;
    unsigned short padd=0;
    unsigned short word16;
    unsigned long  sum;

	// Find out if the length of data is even or odd number. If odd,
	// add a padding byte = 0 at the end of packet
	if ((padding&1)==1){
		padd=1;
		buff[len_udp]=0;
	}

	//initialize sum to zero
	sum=0;

	// make 16 bit words out of every two adjacent 8 bit words and
	// calculate the sum of all 16 vit words
	for (uint i=0;i<len_udp+padd;i=i+2){
		word16 =((buff[i]<<8)&0xFF00)+(buff[i+1]&0xFF);
		sum = sum + (unsigned long)word16;
	}
	// add the UDP pseudo header which contains the IP source and destinationn addresses
	for (uint i=0;i<4;i=i+2){
		word16 =((src_addr[i]<<8)&0xFF00)+(src_addr[i+1]&0xFF);
		sum=sum+word16;
	}
	for (uint i=0;i<4;i=i+2){
		word16 =((dest_addr[i]<<8)&0xFF00)+(dest_addr[i+1]&0xFF);
		sum=sum+word16;
	}
	// the protocol number and the length of the UDP packet
	sum = sum + prot_udp + len_udp;

	// keep only the last 16 bits of the 32 bit calculated sum and add the carries
    	while (sum>>16)
		sum = (sum & 0xFFFF)+(sum >> 16);

	// Take the one's complement of sum
	sum = ~sum;

return ((unsigned short) sum);
}


unsigned short SimulatedMachine:: ip_sum_calc(unsigned short len_ip_header, uint8_t* buff)
{
	unsigned short word16;
	unsigned long sum=0;
	unsigned short i;

		// make 16 bit words out of every two adjacent 8 bit words in the packet
		// and add them up
		for (i=0;i<len_ip_header;i=i+2){
					word16 =((buff[i]<<8)&0xFF00)+(buff[i+1]&0xFF);
							sum = sum + (unsigned long) word16;
								}

			// take only 16 bits out of the 32 bit sum and add up the carries
			while (sum>>16)
					  sum = (sum & 0xFFFF)+(sum >> 16);

				// one's complement the result
				sum = ~sum;

				return ((unsigned short) sum);
}


void SimulatedMachine::copyFrame(byte *frame, byte *newFrame, uint32_t len){
    //cerr << "inside copy function!";
    for (uint i = 0; i < len; i++){
        *(newFrame + i) = *(frame + i);
    }
}
