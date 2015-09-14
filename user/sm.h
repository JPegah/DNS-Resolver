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

#ifndef _S_M_H_
#define _S_M_H_
#include "MCI.h"
#include "machine.h"

#include <condition_variable>
#include <mutex>
#include <map>
#include <utility>
#include "response.h"

class SimulatedMachine : public Machine {
private:
    MyCustomInformation additionalInfo;
    mutex local;
    uint16_t last_allocated_port;

    std::map<uint16_t, responsed*> port_to_answer;
    std::map<uint16_t, condition_variable*> port_to_thread;
  //  responsed* rs;
   /* vector<Port_CV> port_to_thread;
    vector<port_answer> port_to_answer;
    uint16_t last_allocated_port;
    mutex local;
*/
public:
	SimulatedMachine (const ClientFramework *cf, int count);
	virtual ~SimulatedMachine ();

	virtual void initialize ();
	virtual void run ();
	virtual void processFrame (Frame frame, int ifaceIndex);

	static void parseArguments (int argc, char *argv[]);

    void r(int a);
    void send_recursive_request(string dig_domain);
    responsed* registerAndSend(string body, uint32_t dest_ip, bool recursive); // this is blocking
    uint32_t create_request_frame(byte* data, string body, uint32_t dest_ip, bool recursive); // returns the length of created packet
    responsed* resolve_recursive(char* name, uint32_t start_root_ip, int len); // search from root the given string name
    void resolve_recursive_respond(string name, uint32_t dest_ip, uint16_t dest_port, uint16_t src_port); // answer to the recursive query

	unsigned short udp_checksum_calc(unsigned int len_udp, byte src_addr[], byte dest_addr[], bool padding, byte buff[]);
	unsigned short ip_sum_calc(unsigned short len_ip_header, uint8_t* buff);
	void copyFrame(byte *frame, byte *newFrame, uint32_t len);
	void printFrame(byte* data, int length);
	bool send_packet(uint32_t dest_ip, uint16_t src_port, uint16_t dest_port, uint8_t dns, string packetInfo);
    //bool send_packet(uint32_t dest_ip, uint16_t src_port, uint16_t dest_port, uint8_t dns_h, uint32_t packet, uint32_t len);
    bool send_packet(uint32_t dest_ip, uint16_t src_port, uint16_t dest_port, uint8_t dns_h, uint16_t code, string first, string secod, bool second_is_ip);

    bool is_valid(byte* data, uint len);
};

#endif /* sm.h */

