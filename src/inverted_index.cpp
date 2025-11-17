#include "inverted_index.h"
#include <fstream>
#include <sstream>
#include <iostream>

void InvertedIndex::add_document(int doc_id, const std::string &path, const std::vector<std::string>& tokens) {
    doc_id_to_path_[doc_id] = path;
    std::unordered_map<std::string, int> tf;
    for (const auto &t : tokens) tf[t]++;
    for (const auto &p : tf) {
        index_[p.first].push_back({doc_id, p.second});
    }
}

std::vector<Posting> InvertedIndex::lookup(const std::string &term) const {
    auto it = index_.find(term);
    if (it == index_.end()) return {};
    return it->second;
}

void InvertedIndex::save(const std::string &path) const {
    std::ofstream ofs(path, std::ios::trunc);
    if (!ofs) {
        std::cerr << "Failed to open index file for writing: " << path << '\n';
        return;
    }
    // Write document mapping
    ofs << "DOCS " << doc_id_to_path_.size() << '\n';
    for (const auto &kv : doc_id_to_path_) {
        ofs << kv.first << ' ' << kv.second << '\n';
    }
    // Write index
    for (const auto &kv : index_) {
        ofs << kv.first;
        for (const auto &post : kv.second) ofs << ' ' << post.doc_id << ':' << post.tf;
        ofs << '\n';
    }
}

void InvertedIndex::load(const std::string &path) {
    index_.clear();
    doc_id_to_path_.clear();
    std::ifstream ifs(path);
    if (!ifs) {
        std::cerr << "Failed to open index file for reading: " << path << '\n';
        return;
    }
    std::string line;
    if (!std::getline(ifs, line)) return;
    std::istringstream header(line);
    std::string tag;
    int doccount = 0;
    header >> tag >> doccount;
    for (int i = 0; i < doccount; ++i) {
        if (!std::getline(ifs, line)) return;
        std::istringstream iss(line);
        int id; std::string path;
        iss >> id;
        std::getline(iss, path);
        if (!path.empty() && path[0] == ' ') path.erase(0,1);
        doc_id_to_path_[id] = path;
    }
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string term;
        iss >> term;
        std::string token;
        while (iss >> token) {
            auto pos = token.find(':');
            if (pos == std::string::npos) continue;
            int id = std::stoi(token.substr(0,pos));
            int tf = std::stoi(token.substr(pos+1));
            index_[term].push_back({id, tf});
        }
    }
}

int InvertedIndex::doc_count() const { return static_cast<int>(doc_id_to_path_.size()); }

const std::string& InvertedIndex::doc_path(int doc_id) const {
    static const std::string empty;
    auto it = doc_id_to_path_.find(doc_id);
    if (it == doc_id_to_path_.end()) return empty;
    return it->second;
}
