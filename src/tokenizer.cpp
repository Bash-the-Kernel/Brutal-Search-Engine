#include "tokenizer.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_set>

static const std::unordered_set<std::string> STOP_WORDS = {
    "the","and","is","in","at","of","a","an","to","for",
    "on","by","with","that","this","it"
};

Tokenizer::Tokenizer() {}

std::vector<std::string> Tokenizer::tokenize(const std::string &text) const {
    std::string tmp;
    tmp.reserve(text.size());
    for (char c : text) {
        if (std::isalnum((unsigned char)c)) tmp.push_back(std::tolower((unsigned char)c));
        else tmp.push_back(' ');
    }
    std::istringstream iss(tmp);
    std::vector<std::string> tokens;
    std::string w;
    while (iss >> w) {
        if (STOP_WORDS.find(w) == STOP_WORDS.end()) tokens.push_back(w);
    }
    return tokens;
}
