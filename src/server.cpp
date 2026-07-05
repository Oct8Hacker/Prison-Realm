#include "server.hpp"
#include "dns_parser.hpp"
DNSServer::DNSServer(int port) : _port(port), _server_fd(-1), _epoll_fd(-1), _running(false) {}
DNSServer::~DNSServer(){ stop(); }
bool DNSServer::initSocket(){
    _server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    // create a UDP socket
    if (_server_fd < 0) {
        std::cerr << "[-] Error: Failed to create socket." << std::endl;
        return false;
    }
    // some non_blocking flags such that the port becomes non_blocking
    int flags = fcntl(_server_fd, F_GETFL, 0);
    if (fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        std::cerr << "[-] Error: Failed to set non-blocking flag." << std::endl;
        return false;
    }
    // initialise the struct for that port
    sockaddr_in addr{};
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(_port);
    // here you bind the socket to the port
    if (bind(_server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[-] Error: Failed to bind to port " << _port << std::endl;
        return false;
    }
    std::cout << "[+] Socket bound successfully to port " << _port << std::endl;
    return true;
}
bool DNSServer::initEpoll(){
    // create epoll
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd < 0) {
        std::cerr << "[-] Error: Failed to create epoll instance." << std::endl;
        return false;
    }
    // here i will add the server socket so that the epoll keeps track of it
    epoll_event ev{};
    ev.events = EPOLLIN | EPOLLET;// this is to tell the kernel when to trigger i.e first flag is to
    //                               tell when any socket is reading and second flag is edge-trigger mode
    ev.data.fd = _server_fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &ev) < 0) {
        std::cerr << "[-] Error: Failed to add socket to epoll control." << std::endl;
        return false;
    }
    return true;
}
bool DNSServer::start(){
    if (!initSocket() || !initEpoll()) {
        return false;
    }
    _running = true;
    std::cout << "[+] DNS Server running. Waiting for events..." << std::endl;
    char buffer[4096];
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    while(_running) {
        int num_events = epoll_wait(_epoll_fd, _events, MAX_EVENTS, -1);
        if (num_events < 0) {
            if (errno == EINTR) continue; // some system signal interruption
            std::cerr << "[-] Critical: epoll_wait failed." << std::endl;
            break;
        }
        for (int i = 0; i < num_events; i++) {
            if (_events[i].data.fd == _server_fd) {
                while (true) {
                    // edge triggered so we will drain completely
                    std::memset(buffer, 0, sizeof(buffer));
                    int bytes_read = (int)recvfrom(_server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_len);
                    
                    if (bytes_read < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break; 
                        }
                        std::cerr << "[-] Error reading data from socket." << std::endl;
                        break;
                    }
                    std::cout << "[*] Intercepted raw DNS query! Size: " << bytes_read << " bytes." << std::endl;
                    DNSHeader* header = reinterpret_cast<DNSHeader*>(buffer);
                    int offset = sizeof(DNSHeader);
                    std::string domain = DNSParser::extractDomainName(buffer, offset);
                    std::cout << "[>>>] Client is requesting: " << domain << std::endl;
                }
            }
        }
    }
    return true;
}
void DNSServer::stop(){
    if (!_running) return;
    _running = false;
    if (_server_fd != -1) close(_server_fd);
    if (_epoll_fd != -1) close(_epoll_fd);
    std::cout << "[+] Server shutdown complete." << std::endl;
}