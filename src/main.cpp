#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include "tokenizer.h"
#include "inverted_index.h"

namespace fs = std::filesystem;

static std::vector<std::string> split_terms(const std::string &q) {
    std::istringstream iss(q);
    std::vector<std::string> terms;
    std::string w;
    while (iss >> w) terms.push_back(w);
    return terms;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: search_engine <index|query> [args...]\n";
        return 1;
    }
    std::string cmd = argv[1];
    if (cmd == "index") {
        if (argc < 4) {
            std::cerr << "Usage: search_engine index <folder> <index-file>\n";
            return 1;
        }
        std::string folder = argv[2];
        std::string index_file = argv[3];
        Tokenizer tokenizer;
        InvertedIndex index;
        int docid = 0;
        for (auto &p : fs::recursive_directory_iterator(folder)) {
            if (!p.is_regular_file()) continue;
            if (p.path().extension() != ".txt") continue;
            std::ifstream ifs(p.path());
            if (!ifs) continue;
            std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            auto tokens = tokenizer.tokenize(content);
            index.add_document(docid++, p.path().string(), tokens);
        }
        index.save(index_file);
        std::cout << "Indexed " << docid << " documents to " << index_file << "\n";
    } else if (cmd == "query") {
        if (argc < 4) {
            std::cerr << "Usage: search_engine query <index-file> <query string>\n";
            return 1;
        }
        std::string index_file = argv[2];
        std::string q;
        // combine remaining args into query
        for (int i = 3; i < argc; ++i) {
            if (i > 3) q += ' ';
            q += argv[i];
        }
        InvertedIndex index;
        index.load(index_file);
        auto terms = split_terms(q);
        // normalize terms to lowercase
        for (auto &t : terms) {
            std::transform(t.begin(), t.end(), t.begin(), [](unsigned char c){ return std::tolower(c); });
        }
        int N = index.doc_count();
        if (N == 0) {
            std::cout << "Index is empty.\n";
            return 0;
        }
        // collect scores: simple TF-IDF sum
        std::unordered_map<int,double> scores;
        for (const auto &term : terms) {
            auto postings = index.lookup(term);
            int df = static_cast<int>(postings.size());
            double idf = std::log( (double)N / (1 + df) );
            for (const auto &p : postings) {
                scores[p.doc_id] += (double)p.tf * idf;
            }
        }
        // gather results
        std::vector<std::pair<int,double>> res;
        for (auto &kv : scores) res.push_back(kv);
        std::sort(res.begin(), res.end(), [](auto &a, auto &b){ return a.second > b.second; });
        int shown = 0;
        for (auto &r : res) {
            std::cout << "doc " << r.first << " score=" << r.second << " path=" << index.doc_path(r.first) << '\n';
            if (++shown >= 20) break;
        }
        if (res.empty()) std::cout << "No results.\n";
    } else {
        std::cerr << "Unknown command: " << cmd << "\n";
        return 1;
    }
    return 0;
}
