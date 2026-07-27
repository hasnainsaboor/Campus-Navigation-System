#include "Graph.h"

int main()
{
    sqlite3* db;
    sqlite3_open("data/campus.db", &db);

    Graph g;
    g.initDB(db);
   

    g.Menu(db);

    g.saveDB(db);   // save on exit
    sqlite3_close(db);
    return 0;
}