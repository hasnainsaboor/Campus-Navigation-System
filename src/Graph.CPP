#include "Graph.h"
extern "C" {
#include "sqlite3.h"
}
#include <climits>
#include <limits>

// ─── EDGE ─────────────────────
Edge::Edge(string V, int w)
{
    next = nullptr;
    vertices = V;
    weight = w;
}

// ─── VERTEX ───────────────────
Vertex::Vertex(string V)
{
    data = V;
    next = nullptr;
    Edgelist = nullptr;
}

// ─── GRAPH ─────────────────────
Graph::Graph()
{
    head = nullptr;
}

// ─── ADD VERTEX ───────────────
void Graph::addVertex(string value)
{
    Vertex *temp = head;

    while (temp != nullptr)
    {
        if (temp->data == value)
        {
            cout << "Building already exists!\n";
            return;
        }
        temp = temp->next;
    }

    Vertex *newNode = new Vertex(value);

    if (head == nullptr)
    {
        head = newNode;
        return;
    }

    temp = head;
    while (temp->next != nullptr)
        temp = temp->next;

    temp->next = newNode;
}

// ─── ADD EDGE ─────────────────
void Graph::AddEdge(string source, string destination, int weight)
{
     if (source.empty() || destination.empty()) {
        cout << "Invalid input!\n";
        return;
    }
    Vertex *curr = head;

    while (curr != nullptr)
    {
        if (curr->data == source)
        {
            Edge *temp = curr->Edgelist;

            while (temp != nullptr)
            {
                if (temp->vertices == destination)
                {
                    cout << "Path already exists!\n";
                    return;
                }
                temp = temp->next;
            }

            Edge *newEdge = new Edge(destination, weight);

            if (curr->Edgelist == nullptr)
                curr->Edgelist = newEdge;
            else
            {
                Edge *t = curr->Edgelist;
                while (t->next != nullptr)
                    t = t->next;
                t->next = newEdge;
            }

            return;
        }

        curr = curr->next;
    }

    cout << "Source building not found!\n";
}

// ─── PRINT PATH ───────────────
void Graph::printPath(map<string, string> &parent, string dest)
{
    stack<string> path;
    string curr = dest;

    while (curr != "")
    {
        path.push(curr);
        curr = parent[curr];
    }

    cout << "Path: ";
    while (!path.empty())
    {
        cout << path.top();
        path.pop();
        if (!path.empty())
            cout << " -> ";
    }
    cout << endl;
}

// ─── DFS HELPER ───────────────
void Graph::DFShelper(string start)
{
    stack<string> S;
    map<string, bool> visited;

    S.push(start);
    visited[start] = true;

    while (!S.empty())
    {
        string source = S.top();
        S.pop();

        cout << source << " ";

        Vertex *curr = head;
        while (curr && curr->data != source)
            curr = curr->next;

        if (!curr)
            continue;

        Edge *temp = curr->Edgelist;

        while (temp)
        {
            string adj = temp->vertices;

            if (!visited[adj])
            {
                visited[adj] = true;
                S.push(adj);
            }

            temp = temp->next;
        }
    }
}

// ─── DFS ──────────────────────
void Graph::DFS()
{
    string depart;
    cout << "Enter starting building: ";
    getline(cin, depart);  // 

    Vertex *curr = head;
    while (curr && curr->data != depart)
        curr = curr->next;

    if (!curr)
    {
        cout << "Building not found!\n";
        return;
    }

    cout << "DFS: ";
    DFShelper(depart);
    cout << endl;
}

// ─── BFS ──────────────────────
void Graph::BFS(string start)
{
    queue<string> q;
    map<string, bool> visited;

    q.push(start);
    visited[start] = true;

    while (!q.empty())
    {
        string source = q.front();
        q.pop();

        cout << source << " ";

        Vertex *curr = head;
        while (curr && curr->data != source)
            curr = curr->next;

        if (!curr)
            continue;

        Edge *temp = curr->Edgelist;

        while (temp)
        {
            string adj = temp->vertices;

            if (!visited[adj])
            {
                visited[adj] = true;
                q.push(adj);
            }

            temp = temp->next;
        }
    }

    cout << endl;
}

// ─── DIJKSTRA ─────────────────
void Graph::Dijkstra(string start, string destination)
{
    map<string, int> dist;
    map<string, string> parent;
    map<string, bool> visited;

    Vertex *temp = head;

    while (temp)
    {
        dist[temp->data] = INT_MAX;
        parent[temp->data] = "";
        temp = temp->next;
    }

    dist[start] = 0;

    priority_queue<pair<int, string>,
                   vector<pair<int, string>>,
                   greater<pair<int, string>>>
        pq;

    pq.push({0, start});

    while (!pq.empty())
    {
        string node = pq.top().second;
        pq.pop();

        if (visited[node])
            continue;

        visited[node] = true;

        if (node == destination)
            break;

        Vertex *curr = head;
        while (curr && curr->data != node)
            curr = curr->next;

        if (!curr)
            continue;

        Edge *temp = curr->Edgelist;

        while (temp)
        {
            string adj = temp->vertices;
            int w = temp->weight;

            if (dist[node] != INT_MAX &&
                dist[adj] > dist[node] + w)
            {
                dist[adj] = dist[node] + w;
                parent[adj] = node;
                pq.push({dist[adj], adj});
            }

            temp = temp->next;
        }
    }

    if (dist[destination] == INT_MAX)
    {
        cout << "No path found!\n";
        return;
    }

    printPath(parent, destination);
    cout << "Distance: " << dist[destination] << endl;
}

// ─── DISPLAY ───────────────────
void Graph::Display()
{
    Vertex *curr = head;

    while (curr)
    {
        cout << curr->data << " -> ";

        Edge *temp = curr->Edgelist;

        while (temp)
        {
            cout << temp->vertices << "(" << temp->weight << ") ";
            temp = temp->next;
        }

        cout << endl;
        curr = curr->next;
    }
}
void Graph::setBuildingInfo(string name, string hours, string dept)
{
    // Check building exists
    Vertex* temp = head;
    while (temp) {
        if (temp->data == name) {
            buildingHours[name] = hours;
            buildingDept[name]  = dept;
            cout << "Info saved for " << name << "!\n";
            return;
        }
        temp = temp->next;
    }
    cout << "Building not found!\n";
}

string Graph::getBuildingInfo(string name)
{
    if (buildingHours.find(name) == buildingHours.end())
        return "No info available for " + name;

    return "Building: " + name +
           "\nDept: "   + buildingDept[name] +
           "\nHours: "  + buildingHours[name];
};

// ─── MENU ──────────────────────
void Graph::Menu(sqlite3* db)
{
    int choice;

    do
    {
        cout << "\n1.Add Building\n2.Add Path\n3.Display\n4.BFS\n5.DFS\n6.Dijkstra\n7Set Building Info\n8.Get Building Info\n9.Save\n10.Load\n11.Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // ← clear leftover newline after every choice

        if (choice == 1)
        {
            string b;
            cout << "Building: ";
            getline(cin, b);  // ← use getline instead of cin >>
            addVertex(b);
        }
        else if (choice == 2)
        {
            string s, d;
            int w;
            cout << "Source: ";
            getline(cin, s);
            cout << "Destination: ";
            getline(cin, d);
            cout << "Distance: ";
            cin >> w;
            cin.ignore();
            AddEdge(s, d, w);
        }
        else if (choice == 3)  Display();
        else if (choice == 4)
        {
            string s;
            cout << "Start: ";
            getline(cin, s);
            BFS(s);
        }
        else if (choice == 5)  DFS();
        else if (choice == 6)
        {
            string s, d;
            cout << "Start: ";
            getline(cin, s);
            cout << "Destination: ";
            getline(cin, d);
            Dijkstra(s, d);
        }
        else if (choice == 7)
{
    string name, hours, dept;
    cout << "Building name: ";
    getline(cin, name);
    cout << "Department: ";
    getline(cin, dept);
    cout << "Opening hours: ";
    getline(cin, hours);
    setBuildingInfo(name, hours, dept);
}
else if (choice == 8)
{
    string name;
    cout << "Building name: ";
    getline(cin, name);
    cout << getBuildingInfo(name) << endl;
}
        else if (choice == 9)  saveDB(db);
        else if (choice == 10)  loadDB(db);

    } while (choice !=11 );
};

void Graph::initDB(sqlite3* db)
{
    const char* sql =
        "CREATE TABLE IF NOT EXISTS buildings (name TEXT PRIMARY KEY);"
        "CREATE TABLE IF NOT EXISTS paths (source TEXT, destination TEXT, weight INTEGER);";

    char* err;
    sqlite3_exec(db, sql, nullptr, nullptr, &err);
}
void Graph::saveDB(sqlite3* db)
{
    sqlite3_exec(db, "DELETE FROM buildings; DELETE FROM paths;", nullptr, nullptr, nullptr);

    // ── Save buildings ─────────────────────────────────
    Vertex* v = head;
    while (v)
    {
        string sql = "INSERT INTO buildings VALUES ('" + v->data + "');";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

        Edge* e = v->Edgelist;
        while (e)
        {
            string esql = "INSERT INTO paths VALUES ('" + v->data + "','" + e->vertices + "'," + to_string(e->weight) + ");";
            sqlite3_exec(db, esql.c_str(), nullptr, nullptr, nullptr);
            e = e->next;
        }
        v = v->next;
    }

    // ── Save HashMap (building info) ───────────────────
    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS building_info "
        "(name TEXT PRIMARY KEY, dept TEXT, hours TEXT);",
        nullptr, nullptr, nullptr);

    sqlite3_exec(db, "DELETE FROM building_info;", nullptr, nullptr, nullptr);

    for (auto& entry : buildingHours)
    {
        string sql = "INSERT INTO building_info VALUES ('"
                   + entry.first + "','"
                   + buildingDept[entry.first] + "','"
                   + entry.second + "');";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }

    cout << "Graph saved!\n";
}

void Graph::loadDB(sqlite3* db)
{
    head = nullptr;

    // ── Load buildings ─────────────────────────────────
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT name FROM buildings;", -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        string name = (const char*)sqlite3_column_text(stmt, 0);
        addVertex(name);
    }
    sqlite3_finalize(stmt);

    // ── Load paths ─────────────────────────────────────
    sqlite3_prepare_v2(db, "SELECT source, destination, weight FROM paths;", -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        string src  = (const char*)sqlite3_column_text(stmt, 0);
        string dest = (const char*)sqlite3_column_text(stmt, 1);
        int    w    = sqlite3_column_int(stmt, 2);
        AddEdge(src, dest, w);
    }
    sqlite3_finalize(stmt);

    // ── Load HashMap (building info) ───────────────────
    int rc = sqlite3_prepare_v2(db,
        "SELECT name, dept, hours FROM building_info;",
        -1, &stmt, nullptr);

    if (rc == SQLITE_OK)   // table might not exist yet
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            string name  = (const char*)sqlite3_column_text(stmt, 0);
            string dept  = (const char*)sqlite3_column_text(stmt, 1);
            string hours = (const char*)sqlite3_column_text(stmt, 2);
            buildingDept[name]  = dept;
            buildingHours[name] = hours;
        }
        sqlite3_finalize(stmt);
    }

    cout << "Graph loaded from database!\n";
}
