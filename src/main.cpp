#include "server.hpp"
#include "necessary.hpp"

int main() {
    // testing on 5353
    DNSServer server(5353);
    
    if (!server.start()) {
        std::cerr << "[-] Failed to start DNS Server." << std::endl;
        return 1;
    }

    return 0;
}