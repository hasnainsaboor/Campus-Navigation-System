
from flask import Flask, request, jsonify
from flask_cors import CORS
import sqlite3
import heapq
import hashlib
import os

app = Flask(__name__)
CORS(app)

DB_PATH = "data/campus.db"


# ───────── DB CONNECTION ─────────
def get_db():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn


def hash_pw(pw):
    return hashlib.sha256(pw.encode()).hexdigest()


# ───────── DB INIT ─────────
def init_db():
    os.makedirs("data", exist_ok=True)
    conn = get_db()
    conn.executescript("""
        CREATE TABLE IF NOT EXISTS buildings (
            name TEXT PRIMARY KEY
        );

        CREATE TABLE IF NOT EXISTS paths (
            source      TEXT NOT NULL,
            destination TEXT NOT NULL,
            weight      INTEGER NOT NULL
        );

        CREATE TABLE IF NOT EXISTS building_info (
            name  TEXT PRIMARY KEY,
            dept  TEXT DEFAULT 'N/A',
            hours TEXT DEFAULT 'N/A'
        );

        CREATE TABLE IF NOT EXISTS users (
            username TEXT PRIMARY KEY,
            password TEXT NOT NULL
        );
    """)
    # Insert default admin if no users exist yet
    existing = conn.execute("SELECT COUNT(*) as c FROM users").fetchone()["c"]
    if existing == 0:
        conn.execute(
            "INSERT INTO users (username, password) VALUES (?, ?)",
            ("admin", hash_pw("admin123"))
        )
        print("Default admin created  →  username: admin  password: admin123")
    conn.commit()
    conn.close()


# ───────── AUTH ─────────
@app.route("/login", methods=["POST"])
def login():
    data = request.get_json() or {}
    username = data.get("username", "").strip()
    password = data.get("password", "")
    if not username or not password:
        return jsonify({"error": "Username and password required"}), 400
    conn = get_db()
    row = conn.execute(
        "SELECT * FROM users WHERE username=? AND password=?",
        (username, hash_pw(password))
    ).fetchone()
    conn.close()
    if not row:
        return jsonify({"error": "Invalid username or password"}), 401
    return jsonify({"message": "ok", "username": username})


# ───────── BUILDINGS ─────────
@app.route("/buildings", methods=["GET"])
def get_buildings():
    try:
        conn = get_db()
        rows = conn.execute("SELECT name FROM buildings").fetchall()
        conn.close()
        return jsonify({"buildings": [r["name"] for r in rows]})
    except Exception as e:
        return jsonify({"error": str(e)}), 500


@app.route("/buildings", methods=["POST"])
def add_building():
    data  = request.get_json() or {}
    name  = data.get("name",  "").strip()
    dept  = data.get("dept",  "").strip() or "N/A"
    hours = data.get("hours", "").strip() or "N/A"
    if not name:
        return jsonify({"error": "Building name is required"}), 400
    conn = get_db()
    try:
        conn.execute("INSERT OR IGNORE INTO buildings VALUES (?)", (name,))
        conn.execute("""
            INSERT INTO building_info (name, dept, hours) VALUES (?, ?, ?)
            ON CONFLICT(name) DO UPDATE SET dept=excluded.dept, hours=excluded.hours
        """, (name, dept, hours))
        conn.commit()
    except Exception as e:
        conn.close()
        return jsonify({"error": str(e)}), 500
    conn.close()
    return jsonify({"message": "added"})


@app.route("/buildings/<name>", methods=["DELETE"])
def delete_building(name):
    conn = get_db()
    try:
        conn.execute("DELETE FROM buildings WHERE name = ?", (name,))
        conn.execute("DELETE FROM paths WHERE source=? OR destination=?", (name, name))
        conn.execute("DELETE FROM building_info WHERE name = ?", (name,))
        conn.commit()
    except Exception as e:
        conn.close()
        return jsonify({"error": str(e)}), 500
    conn.close()
    return jsonify({"message": "deleted"})


# ───────── PATHS ─────────
@app.route("/paths", methods=["GET"])
def get_paths():
    try:
        conn = get_db()
        rows = conn.execute("SELECT source, destination, weight FROM paths").fetchall()
        conn.close()
        return jsonify({"paths": [{"from": r["source"], "to": r["destination"], "weight": r["weight"]} for r in rows]})
    except Exception as e:
        return jsonify({"error": str(e)}), 500


@app.route("/paths", methods=["POST"])
def add_path():
    data   = request.get_json() or {}
    src    = data.get("from",   "").strip()
    dest   = data.get("to",     "").strip()
    weight = data.get("weight")
    if not src or not dest or weight is None:
        return jsonify({"error": "from, to, and weight are required"}), 400
    if src == dest:
        return jsonify({"error": "from and to must be different"}), 400
    conn = get_db()
    try:
        conn.execute("INSERT INTO paths (source, destination, weight) VALUES (?, ?, ?)", (src, dest, int(weight)))
        conn.commit()
    except Exception as e:
        conn.close()
        return jsonify({"error": str(e)}), 500
    conn.close()
    return jsonify({"message": "path added"})


@app.route("/paths", methods=["DELETE"])
def delete_path():
    data = request.get_json() or {}
    src  = data.get("from", "").strip()
    dest = data.get("to",   "").strip()
    if not src or not dest:
        return jsonify({"error": "from and to are required"}), 400
    conn = get_db()
    try:
        conn.execute(
            "DELETE FROM paths WHERE (source=? AND destination=?) OR (source=? AND destination=?)",
            (src, dest, dest, src)
        )
        conn.commit()
    except Exception as e:
        conn.close()
        return jsonify({"error": str(e)}), 500
    conn.close()
    return jsonify({"message": "path deleted"})


# ───────── DIJKSTRA ─────────
@app.route("/navigate", methods=["GET"])
def navigate():
    start = request.args.get("from")
    end   = request.args.get("to")
    if not start or not end:
        return jsonify({"error": "from and to query params required"}), 400
    conn = get_db()
    buildings = [r["name"] for r in conn.execute("SELECT name FROM buildings")]
    paths     = conn.execute("SELECT source, destination, weight FROM paths").fetchall()
    conn.close()
    if start not in buildings or end not in buildings:
        return jsonify({"error": "unknown building"}), 404
    graph  = {b: [] for b in buildings}
    for p in paths:
        graph[p["source"]].append((p["destination"], p["weight"]))
        graph[p["destination"]].append((p["source"],  p["weight"]))
    dist   = {b: float("inf") for b in buildings}
    parent = {b: None         for b in buildings}
    dist[start] = 0
    pq = [(0, start)]
    while pq:
        d, node = heapq.heappop(pq)
        if node == end:
            break
        for nb, w in graph[node]:
            nd = d + w
            if nd < dist[nb]:
                dist[nb] = nd
                parent[nb] = node
                heapq.heappush(pq, (nd, nb))
    if dist[end] == float("inf"):
        return jsonify({"error": "no path found"}), 404
    path, cur = [], end
    while cur:
        path.append(cur)
        cur = parent[cur]
    path.reverse()
    return jsonify({"path": path, "distance": dist[end]})


# ───────── BUILDING INFO ─────────
@app.route("/building-info/<name>", methods=["GET"])
def get_building_info(name):
    try:
        conn = get_db()
        row  = conn.execute("SELECT dept, hours FROM building_info WHERE name=?", (name,)).fetchone()
        conn.close()
        if not row:
            return jsonify({"dept": "N/A", "hours": "N/A"})
        return jsonify({"dept": row["dept"], "hours": row["hours"]})
    except Exception:
        return jsonify({"dept": "N/A", "hours": "N/A"})


@app.route("/building-info/<name>", methods=["POST"])
def set_building_info(name):
    data  = request.get_json() or {}
    dept  = data.get("dept",  "").strip() or "N/A"
    hours = data.get("hours", "").strip() or "N/A"
    conn  = get_db()
    try:
        conn.execute("""
            INSERT INTO building_info (name, dept, hours) VALUES (?, ?, ?)
            ON CONFLICT(name) DO UPDATE SET dept=excluded.dept, hours=excluded.hours
        """, (name, dept, hours))
        conn.commit()
    except Exception as e:
        conn.close()
        return jsonify({"error": str(e)}), 500
    conn.close()
    return jsonify({"message": "info saved"})


#-------------------Password Change----------------
@app.route("/change-password", methods=["POST"])
def change_password():
    data = request.get_json() or {}

    username = data.get("username", "").strip()
    old_password = data.get("old_password", "")
    new_password = data.get("new_password", "")

    conn = get_db()

    user = conn.execute(
        "SELECT * FROM users WHERE username=? AND password=?",
        (username, hash_pw(old_password))
    ).fetchone()

    if not user:
        conn.close()
        return jsonify({"error": "Current password is incorrect"}), 401

    conn.execute(
        "UPDATE users SET password=? WHERE username=?",
        (hash_pw(new_password), username)
    )

    conn.commit()
    conn.close()

    return jsonify({"message": "Password changed successfully"})
 

# ───────── START ─────────
if __name__ == "__main__":
    init_db()
    print("Campus Navigation API  →  http://localhost:5000")
    app.run(debug=True, port=5000)
    
    
    
  