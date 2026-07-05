#pragma once
#include "necessary.hpp"
#include <sys/epoll.h>
#include <netinet/in.h>
class DNSServer{
    private: 
        int _port;
        int _server_fd;
        int _epoll_fd;
        bool _running;
        static constexpr int MAX_EVENTS = 64;
        epoll_event _events[MAX_EVENTS];
        bool initSocket();
        bool initEpoll();
    public:
        DNSServer(int port);
        ~DNSServer();
        bool start();
        void stop();
};