**C++ Search Engine (Portfolio Project)**

- **Purpose**: Compact, resume-ready C++ search engine that indexes plain-text files and answers simple queries using an inverted index and TF-IDF ranking.

**Features (MVP)**
- Index all `*.txt` files in a directory (recursive).
- Tokenization with stop-word removal and lowercase normalization.
- Inverted index persisted to disk (simple text format).
- CLI supporting:
  - `index <folder> <index-file>` — build an index from a folder of text files.
  - `query <index-file> <query string>` — simple ranked retrieval using TF-IDF.

**Quick Build (Windows PowerShell)**
```
# from project root
mkdir build; cd build
cmake ..
cmake --build . -- -j
```

**Run examples**
- Index a folder of text files:
```
.\search_engine.exe index C:\path\to\texts index.dat
```
- Query the index (multiple words are treated as terms summed in TF-IDF):
```
.\search_engine.exe query index.dat "apple banana"
```

**Index file format (simple, human-readable)**
- First line: `DOCS <N>` where `<N>` is number of documents.
- Next `N` lines: `<doc_id> <path>` mapping doc id -> file path.
- Remaining lines: one term per line, followed by postings `doc_id:tf` pairs separated by spaces.

Example snippet:
```
DOCS 2
0 C:\texts\a.txt
1 C:\texts\b.txt
apple 0:3 1:1
banana 1:2
```

**Design notes**
- Tokenizer: converts to lowercase, strips non-alphanumerics, splits on whitespace, removes a small stop-word list.
- Indexer: collects per-document term frequencies and appends postings for each term.
- Query scoring: for a query of terms, score each document by summing `tf * idf` for matching terms, where `idf = ln(N / (1 + df))`.

**Limitations (current MVP)**
- Query parsing is basic (no boolean operators or phrase queries).
- Ranking is unnormalized (no cosine normalization or length normalization).
- Persistence uses a simple text format — not optimized for speed or space.
- Tokenization has no stemming; punctuation handling is simple.
- Indexing is single-threaded in this MVP (can be upgraded to multi-threading).

**Next recommended improvements**
- Normalize scores (cosine similarity with TF-IDF vectors).
- Add boolean/phrase/proximity queries and a small query parser (support `AND`, `OR`, `NOT`).
- Add stemming (Porter) and more advanced tokenization.
- Make indexing multi-threaded and add incremental indexing support.
- Replace text persistence with a binary or JSON format (e.g., `nlohmann/json`) or memory-mapped files for larger corpora.
- Add unit tests (Catch2) and a small sample dataset for CI.

**Files of interest**
- `CMakeLists.txt` — build configuration.
- `include/tokenizer.h`, `src/tokenizer.cpp` — tokenization.
- `include/inverted_index.h`, `src/inverted_index.cpp` — inverted index and persistence.
- `src/main.cpp` — CLI entrypoint for `index` and `query` commands.

**How I can help next**

If you want, I can replace the existing `README.md` with this expanded version now — say "replace" and I'll overwrite the original.
**Sample dataset and fetching**

I added a small local sample corpus in `sample_texts/` so you can test immediately without downloading anything. It contains three short public-domain excerpts.

If you want a larger dataset, use Project Gutenberg or a Wikipedia dump. I included `scripts/fetch_gutenberg.ps1` to download three public-domain books into `sample_texts/gutenberg/` (PowerShell script; run from the repository root). Example:

PowerShell:
```
# from repository root
.\scripts\fetch_gutenberg.ps1

# then index the downloaded files
.\build\search_engine.exe index sample_texts/gutenberg index.dat
```

Notes:

**Web GUI (quick test)**

I added a minimal web UI in `web_ui/` so you can try queries from a browser. It's a static `index.html` and a small Flask server (`web_ui/server.py`) that calls the local `search_engine` binary in `build/`.

Requirements:
- Python 3 and `Flask` installed. Install with:

```
py -3 -m pip install flask
```

Run the server (from repository root):

```
py -3 web_ui\server.py
```

Open http://127.0.0.1:5000/ in your browser. The UI sends queries to the Flask server which runs `search_engine query` under the hood.

Notes:
- Ensure you've built the C++ binary (`cmake` + `cmake --build`) so `build/search_engine.exe` exists.
- The Flask server is intentionally simple and executes the `search_engine` binary as a subprocess; for production or higher performance, we can integrate the searcher directly into an HTTP server in C++.