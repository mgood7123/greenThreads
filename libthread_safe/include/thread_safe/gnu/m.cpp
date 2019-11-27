#include <string>
#include <vector>
#include <iostream>
#include <queue>

using namespace std;

struct edge;
struct node {
    int id;
    double longitude;
    double latitude;
    double distance;
    string state;
    string name;
    vector<edge *> r;
    int prio;
};

struct edge {
    string name, type;
    double dist;
    node *x;
    node *y;

    void print() {
      cout << name << " " << type << " " << dist << " miles" << " from " << x->id << " to " << y->id
           << endl;
    }

    edge(node *x, node *y, double dist, string name) {
      this->x = x;
      this->y = y;
      this->dist = dist;
      this->name = name;
    }

    edge() {};
};

struct route {
    int place1;
    int place2;
    string rt;
    string vert;
    double lengt;

    route(int o, int d, double l, string r) {
      place1 = o;
      place2 = d;
      rt = r;
      lengt = l;
    }
};

struct compPair {
    bool operator()(node *a, node *b) {
      return a->prio > b->prio;
    }
};

void SPT(vector<node *> &g, int from, int to) {
  priority_queue < node * , vector < node * >, compPair > pq;
  g[from]->prio = 0;
  pq.push(g[from]);

  while (!pq.empty()) {
    node *place1 = pq.top();
    pq.pop();
    for (int i = 0; i < place1->r.size(); i++) {
      edge *rd = place1->r[i];
      node *place2 = rd->x;
      if (place2 == place1) {
        place2 = rd->y;
      }
      if (place2->prio == -2) {
        place2->prio = place1->prio + rd->dist;
        pq.push(place2);
      } else if (place2->prio > (place1->prio + rd->dist)) {
        place2->prio = place1->prio + rd->dist;
      }
    }
  }

  node *beg = g[from];
  node *end = g[to];

  if (g[from]->prio < 0 || g[to]->prio < 0) {
    cout << "Path does not exist";
    return;
  }
  node *lowp = end;
  vector < route * > p;
  while (beg != end) {
    double reallowp = end->prio;
    edge *rtt = end->r[0];
    for (int i = 0; i < end->r.size(); i++) {
      edge *r = end->r[i];
      node *town1 = r->x;
      if (town1 == end) {
        town1 = r->y;
      }
      if (town1->prio <= reallowp) {
        reallowp = town1->prio;
        lowp = town1;
        rtt = r;
      }
    }

    route *e = new route(end->id, lowp->id, rtt->dist, rtt->name);

    p.push_back(e);
    end = lowp;
  }


  for (int i = p.size() - 1; i >= 0; i--) {
    cout << "From intersection " << p[i]->place2 << "take " << p[i]->rt << " " << p[i]->lengt
         << " miles to intersection " << p[i]->place1 << endl;
  }

}

int main() {
  vector < node * > t = readTown();
  readRoad(t);
  SPT(t, 68, 69);
}