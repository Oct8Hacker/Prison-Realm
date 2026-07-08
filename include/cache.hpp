#pragma once
#include <list>
#include <chrono>
#include <unordered_map>
#include <mutex> 
#include "necessary.hpp"

struct CacheEntry{
    std::vector<char> raw_response;
    int og_query_size;
    std::chrono::steady_clock::time_point expires_at;
};

class LRUCache{
    private:
        size_t capacity;
        std::list<std::string> lru;
        std::unordered_map<std::string, std::pair<std::list<std::string>::iterator, CacheEntry>>umpp;
        
        std::mutex _mtx; 

    public:
        LRUCache(int capacity);
        bool get(const std::string& domain, std::vector<char>&output_data, int &out_query_size);
        void put(const std::string& domain, const char* response_buffer, int response_size, int query_size, int ttl_seconds);
};