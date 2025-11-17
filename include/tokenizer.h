#pragma once
#include <string>
#include <vector>

class Tokenizer {
public:
    Tokenizer();
    std::vector<std::string> tokenize(const std::string &text) const;
};
