#pragma once
#include "necessary.hpp"
#include <memory>
#include <array>
#include <fstream>
#include <sstream>
struct Vertex{
    int next[38];
    bool output = false;
    Vertex();
};
class Trie {
private:
    std::vector<Vertex>trie;
    int charToIndex(char &c);
public:
    Trie() {
        trie.emplace_back();
    }
    void insert(const std::string &word); 
    bool search(const std::string &word);
    bool loadFromFile(const std::string &filepath);
};
