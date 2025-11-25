# Brutal Search Engine

A compact, minimal C++ search engine designed as a portfolio project and learning exercise. It indexes plain-text files using a simple inverted index and returns ranked results using a basic TF–IDF scoring model.

This repository contains:

- A small tokenizer and inverted-index implementation (C++17).
- A command-line interface (`index` and `query`) for building and searching an index.
- Simple persistence (human-readable index format).
- A tiny web UI (Flask) for interactive testing.
- Utility script to fetch a few Project Gutenberg texts for experimentation.

---

## Quick highlights

- Index `.txt` files recursively from a directory.
- Tokenization: lowercase + alphanumeric filtering + stop-word removal.
- Retrieval: TF–IDF scoring (sum over query terms). Top results are returned.
- Lightweight and dependency-free C++ core — easy to extend for research or demos.

---

## Build

Requirements

- C++17 toolchain (MSVC, GCC or Clang)
- CMake 3.16+

Build (recommended — cross-platform)

PowerShell / Windows (from repository root):
```powershell
mkdir build
cmake -S . -B build
cmake --build build --config Release
```

Linux / macOS (from repository root):
```bash
mkdir -p build
cmake -S . -B build
cmake --build build -j
```

After a successful build the executable will be at `build/Release/search_engine.exe` (Windows MSVC) or `build/search_engine` (Unix / Makefile/Ninja).

---

## CLI Usage

Index files:

```powershell
# from repository root (Windows example)
.\build\Release\search_engine.exe index sample_texts index.dat
```

Query an index:

```powershell
.\build\Release\search_engine.exe query index.dat "alice"
```

The `query` command prints JSON results to stdout (fields: `query`, `results` where each result has `docid`, `score`, `path`). This JSON output is consumed by the included web UI.

---

## Index format

The project uses a simple, human-readable index format (suitable for debugging and small corpora). It contains:

- A header with the number of documents: `DOCS <N>`
- `N` lines mapping `<doc_id> <path>`
- Term lines: `<term> <doc_id>:<tf> <doc_id>:<tf> ...`

This format is intentionally simple; for production work consider binary formats or using `nlohmann/json` / memory-mapped structures.

---

## Web UI (development)

There is a minimal browser UI in `web_ui/` for quick exploration.

Prerequisites: Python 3 and Flask

Install Flask:

```powershell
py -3 -m pip install flask
```

Run the server (from repository root):

```powershell
py -3 web_ui\server.py
```

Open `http://127.0.0.1:5000/` in your browser and provide the path to an `index.dat` file.

Notes:

- The Flask server calls the `search_engine` binary as a subprocess. This is convenient for development but not optimal for production. If you prefer a single native binary I can add an embedded HTTP server (e.g., `cpp-httplib`).

---

## Sample data

The repository includes `sample_texts/` with a few short public-domain excerpts to test functionality immediately.

For a larger dataset, `scripts/fetch_gutenberg.ps1` can download a few Project Gutenberg plain-text files into `sample_texts/gutenberg/`.

Example (PowerShell):

```powershell
.\scripts\fetch_gutenberg.ps1
.\build\Release\search_engine.exe index sample_texts\gutenberg index.dat
```

---

## Design notes & limitations

- Tokenizer: lowercase, strip non-alphanumerics, remove short stop-word list.
- Indexer: stores per-term posting lists with term-frequency (tf).
- Ranking: TF–IDF with `idf = ln(N / (1 + df))` and scores summed across query terms (no vector normalization yet).

Limitations (MVP):

- No stemming or advanced tokenization.
- No phrase or boolean query support.
- Single-threaded indexing for simplicity.
- Simple text persistence; not optimized for large corpora.

---

## Development roadmap (suggested next steps)

- Normalize scores (cosine similarity / document vector normalization).
- Add boolean and phrase queries (AND/OR/NOT, quoted phrases).
- Add stemming (Porter) and better tokenization.
- Make indexing multi-threaded and add incremental updates.
- Replace text persistence with efficient binary or memory-mapped storage.
- Add unit tests (Catch2) and CI for reproducible builds.

---

## Contributing

Contributions are welcome. Suggested workflow:

1. Fork the repository and create a feature branch.
2. Implement changes and add tests where appropriate.
3. Open a pull request describing the change and rationale.

If you want, I can open PR templates, CI config (GitHub Actions), and unit-test scaffolding.

---

## License

This project is provided for educational purposes. Add a LICENSE file if you plan to publish or redistribute the code.

---

If you'd like, I can also:

- Add JSON schema examples and a sample `index.dat` in `sample_texts/`.
- Implement snippet extraction and highlighting in the C++ JSON output.
- Replace the Flask UI with an integrated C++ HTTP server to remove the Python dependency.

Tell me which of these you want next and I will implement it.
