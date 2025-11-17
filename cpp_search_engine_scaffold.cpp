# README.md

# C++ Search Engine (Portfolio Project)

A compact, resume-ready C++ search engine that indexes plain text files and answers boolean and ranked queries using a simple inverted index and TF-IDF ranking. This scaffold includes a build system (CMake), basic tokenizer, indexer, and a simple CLI to run indexing and queries.

---

## Features (MVP)
- Index all `.txt` files in a directory (multi-threaded indexing).
- Tokenization with stop-word removal and lowercase normalization.
- Inverted index persisted to disk (simple binary or JSON format).
- Query CLI supporting:
  - Single-word queries
  - AND / OR boolean queries
  - Ranked results using TF-IDF

## Roadmap / Extensions
1. Phrase queries, proximity search
2. Stemming (Porter stemmer)
3. Distributed indexing / sharding
4. HTTP/REST API and web UI
5. Compression for index (variable byte, gamma)
6. Better persistence (memory-mapped files)

---

## Build & Run

Requirements:
- C++17 compatible compiler (g++ 9+, clang 10+, MSVC 2019+)
- CMake 3.16+

Build:

```bash
mkdir build && cd build
cmake ..
cmake --build . -- -j
```

Run (index a folder):

```bash
./search_engine index /path/to/text/files --index-file ./index.dat
```

Run (query):

```bash
./search_engine query --index-file ./index.dat "apple AND banana"
```

---

# CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(cpp_search_engine VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(search_engine ${SRC_FILES})

target_link_libraries(search_engine PRIVATE Threads::Threads)
```

---

# File: include/tokenizer.h

```cpp
#pragma once
#include <string>
#include <vector>

class Tokenizer {
public:
    Tokenizer();
    std::vector<std::string> tokenize(const std::string &text) const;
};
```

# File: src/tokenizer.cpp

```cpp
#include "tokenizer.h"
#include <sstream>
#include <algorithm>
#include <cctype>

static const std::unordered_set<std::string> STOP_WORDS = {
    "the","and","is","in","at","of","a","an","to","for"
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
```

# File: include/inverted_index.h

```cpp
#pragma once
#include <string>
#include <unordered_map>
#include <vector>

// Posting: document id and term frequency
struct Posting { int doc_id; int tf; };

class InvertedIndex {
public:
    void add_document(int doc_id, const std::vector<std::string>& tokens);
    std::vector<Posting> lookup(const std::string &term) const;
    void save(const std::string &path) const;
    void load(const std::string &path);
private:
    std::unordered_map<std::string, std::vector<Posting>> index_;
    std::unordered_map<int, std::string> doc_id_to_path_;
};
```

# File: src/inverted_index.cpp

```cpp
#include "inverted_index.h"
#include <fstream>
#include <nlohmann/json.hpp> // optional: if you add it later

void InvertedIndex::add_document(int doc_id, const std::vector<std::string>& tokens) {
    std::unordered_map<std::string, int> tf;
    for (auto &t : tokens) tf[t]++;
    for (auto &p : tf) {
        index_[p.first].push_back({doc_id, p.second});
    }
}

std::vector<Posting> InvertedIndex::lookup(const std::string &term) const {
    auto it = index_.find(term);
    if (it == index_.end()) return {};
    return it->second;
}

void InvertedIndex::save(const std::string &path) const {
    // Simple text dump (replace with binary or json later)
    std::ofstream ofs(path, std::ios::trunc);
    for (auto &kv : index_) {
        ofs << kv.first;
        for (auto &post : kv.second) ofs << ' ' << post.doc_id << ':' << post.tf;
        ofs << '\n';
    }
}

void InvertedIndex::load(const std::string &path) {
    // implement if needed
}
```

# File: src/main.cpp

```cpp
#include <iostream>
#include <filesystem>
#include "tokenizer.h"
#include "inverted_index.h"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: search_engine <index|query> <args...>\n";
        return 1;
    }
    std::string cmd = argv[1];
    if (cmd == "index") {
        std::string folder = argv[2];
        Tokenizer tokenizer;
        InvertedIndex index;
        int docid = 0;
        for (auto &p : fs::recursive_directory_iterator(folder)) {
            if (!p.is_regular_file()) continue;
            if (p.path().extension() != ".txt") continue;
            std::ifstream ifs(p.path());
            std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            auto tokens = tokenizer.tokenize(content);
            index.add_document(docid++, tokens);
        }
        index.save("index.dat");
        std::cout << "Indexed " << docid << " documents.\n";
    } else if (cmd == "query") {
        std::cerr << "Query mode not implemented yet.\n";
    }
    return 0;
}
```

---

## Next steps (what I'll implement next if you want me to continue now)
1. Implement query parsing and TF-IDF ranking.
2. Make indexing multi-threaded.
3. Add persistence using a simple binary format.
4. Add unit tests (Catch2) and benchmarks (Google Benchmark).

---

## Notes
- This scaffold intentionally keeps dependencies minimal. If you want JSON persistence or HTTP server, I recommend adding `nlohmann/json` and `cpp-httplib` or `Boost.Beast` later.


