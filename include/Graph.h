#ifndef GRAPH_H
#define GRAPH_H
#include <iostream>
#include <queue>
#include <stack>
#include <map>
extern "C"
{
#include "sqlite3.h"
}

using namespace std;

class Edge
{
public:
    Edge *next;
    string vertices;
    int weight;

    Edge(string V, int w);
};

class Vertex
{
public:
    string data;
    Vertex *next;
    Edge *Edgelist;

    Vertex(string V);
};

class Graph
{
public:
    Vertex *head;

    Graph();

    void addVertex(string value);
    void AddEdge(string source, string Destination, int weight);

    void printPath(map<string, string> &parent, string dest);

    void DFShelper(string start);
    void DFS();

    void Dijkstra(string start, string destination);

    void BFS(string start);

    void Display();

    void Menu(sqlite3 *db); // ← pass db here
    // HashMap for building info
    map<string, string> buildingHours;
    map<string, string> buildingDept;

    void setBuildingInfo(string name, string hours, string dept);
    string getBuildingInfo(string name);

    // ─── SQLite ───────────────────
    void initDB(sqlite3 *db);
    void saveDB(sqlite3 *db);
    void loadDB(sqlite3 *db);
};

#endif