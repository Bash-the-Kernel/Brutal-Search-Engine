#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct Posting { int doc_id; int tf; };

class InvertedIndex {
public:
    void add_document(int doc_id, const std::string &path, const std::vector<std::string>& tokens);
    std::vector<Posting> lookup(const std::string &term) const;
    void save(const std::string &path) const;
    void load(const std::string &path);
    int doc_count() const;
    const std::string& doc_path(int doc_id) const;
private:
    std::unordered_map<std::string, std::vector<Posting>> index_;
    std::unordered_map<int, std::string> doc_id_to_path_;
};
