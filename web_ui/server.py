"""
Simple Flask server that exposes a /search endpoint. It calls the compiled `search_engine` binary
in query mode and parses its output. Requires Python 3 and Flask installed.

Run from repo root (PowerShell example):
    py -3 -m pip install flask
    py -3 web_ui\server.py

Then open http://127.0.0.1:5000/ in your browser.
"""
from flask import Flask, request, jsonify, send_from_directory
import subprocess
import shlex
import os
import sys

app = Flask(__name__, static_folder='')

def find_search_bin():
    candidates = []
    exe_name = 'search_engine.exe' if os.name == 'nt' else 'search_engine'
    # common locations
    candidates.append(os.path.join('build', exe_name))
    candidates.append(os.path.join('build', 'Release', exe_name))
    candidates.append(os.path.join('build', 'Debug', exe_name))
    candidates.append(os.path.join('build', 'bin', exe_name))
    # absolute paths relative to repo
    here = os.path.dirname(__file__)
    for c in candidates:
        p = os.path.normpath(os.path.join(here, '..', c))
        if os.path.exists(p):
            return p
    # fallback: return first candidate (non-existing) so caller can show helpful error
    return os.path.normpath(os.path.join(here, '..', candidates[0]))

SEARCH_BIN = find_search_bin()

@app.route('/')
def index():
    return send_from_directory(os.path.join(os.path.dirname(__file__)), 'index.html')

@app.route('/search', methods=['POST'])
def search():
    data = request.get_json() or {}
    index_file = data.get('index', 'index.dat')
    query = data.get('query', '')
    if not query:
        return jsonify({'error':'empty query'}), 400
    # Build command: search_engine query <index-file> <query>
    cmd = [SEARCH_BIN, 'query', index_file, query]
    if not os.path.exists(SEARCH_BIN):
        return jsonify({'error': f"search binary not found at {SEARCH_BIN}. Build the project first."}), 500
    try:
        # call the binary and collect output
        proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=False)
    except Exception as e:
        print(f"[server] exception running command: {cmd}: {e}")
        return jsonify({'error': str(e)}), 500
    if proc.returncode != 0:
        # Log debug info to server console to help troubleshooting
        print(f"[server] search binary returned non-zero ({proc.returncode})")
        print("[server] cmd:", cmd)
        print("[server] stdout:\n", proc.stdout)
        print("[server] stderr:\n", proc.stderr)
        return jsonify({'error': 'search binary error', 'stderr': proc.stderr, 'stdout': proc.stdout, 'rc': proc.returncode}), 500

    # Expect JSON output from the binary. Try to parse it.
    try:
        import json
        payload = json.loads(proc.stdout)
        # Basic validation
        if isinstance(payload, dict) and 'results' in payload:
            return jsonify(payload)
        else:
            # fallthrough to error
            print('[server] unexpected JSON shape:', payload)
            return jsonify({'error': 'unexpected json from search binary', 'raw': proc.stdout}), 500
    except Exception as e:
        # Parsing failed — try legacy plain-text parsing as a fallback
        print('[server] failed to parse JSON from search binary:', e)
        print('[server] stdout:\n', proc.stdout)

        # Legacy parser: lines like "doc <id> score=<score> path=<path>"
        results = []
        for line in proc.stdout.splitlines():
            line = line.strip()
            if not line: continue
            if line.startswith('doc '):
                try:
                    parts = line.split(' ')
                    docid = int(parts[1])
                    score = 0.0
                    path = ''
                    for p in parts[2:]:
                        if p.startswith('score='):
                            try:
                                score = float(p[len('score='):])
                            except Exception:
                                score = 0.0
                        elif p.startswith('path='):
                            path = p[len('path='):]
                        else:
                            if 'path=' in line:
                                path = line.split('path=',1)[1]
                                break
                    results.append({'docid': docid, 'score': score, 'path': path})
                except Exception:
                    continue
        if results:
            return jsonify({'results': results})
        return jsonify({'error': 'failed to parse json and no legacy results', 'stdout': proc.stdout, 'exc': str(e)}), 500

if __name__ == '__main__':
    port = int(os.environ.get('PORT','5000'))
    print(f"Starting web UI on http://127.0.0.1:{port}/ — search binary: {SEARCH_BIN}")
    app.run(host='127.0.0.1', port=port, debug=False)
