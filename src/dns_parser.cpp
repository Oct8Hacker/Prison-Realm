#include "dns_parser.hpp"
std::string DNSParser::extractDomainName(const char* buffer, int &offset){
    std::string domain = "";
    bool first = true;
    int current_offset = offset;
    int og_offset = offset;
    int jumps = 0;// this is so that there is not any kind of malicious packet due to which my program gets stuck in while loop
    bool jumped = false;
    while(true){
        // if i have queried google.com then it will transform it to 06google03com00
        // here eventhough i have used a char array the numbers i.e len is stores as int so there is no such impilict conversion needed
        uint8_t len = buffer[current_offset];
        if(jumps > 5){
            std::cerr << "[-] Error: Too many DNS pointer jumps (possible loop)." << std::endl;
            break;
        }
        if((len & 0xC0) == 0xC0){
            // so now this is compression pointer
            if(!jumped){
                og_offset = current_offset + 2;
                jumped = true;
            }
            uint8_t next_byte = buffer[current_offset + 1];
            int jump_target = ((len & 0x3F) << 8) | next_byte;
            current_offset = jump_target;
            jumps++;
            continue;
        }
        if(len == 0){
            if(!jumped){
                og_offset = current_offset + 1;
            }
            break;
        }
        if(!first){
            domain+=".";
        }
        for(int i = 0;i<len;i++){
            domain += buffer[++current_offset];
        }
        current_offset++;
        first = false;
    }
    offset = og_offset;
    return domain;
}