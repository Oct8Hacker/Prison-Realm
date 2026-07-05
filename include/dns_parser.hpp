#pragma once
#include "necessary.hpp"
#pragma pack(push, 1)
// this is so that there is no padding from the compiler we need this to be the size of DNS header
struct DNSHeader {
    uint16_t id;       // Transaction ID
    uint16_t flags;    // The 16 bits containing QR, Opcode, AA, TC, RD, RA, Z, RCODE
    uint16_t qdcount;  // Number of questions
    uint16_t ancount;  // Number of answers
    uint16_t nscount;  // Number of authority records
    uint16_t arcount;  // Number of additional records
};
#pragma pack(pop)
class DNSParser {
public:
    // Takes the raw packet buffer and an offset, returns the readable "google.com" string
    static std::string extractDomainName(const char* buffer, int& offset);
};