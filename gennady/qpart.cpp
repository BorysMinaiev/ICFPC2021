/**
 *    author:  tourist
 *    created: 09.07.2021 17:21:07
**/
#undef _GLIBCXX_DEBUG

#include <random>
#include <vector>
#include <cassert>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "evaluator.h"

const int VERTEX_ID = 27;

using namespace std;

template <typename A, typename B>
string to_string(pair<A, B> p);

template <typename A, typename B, typename C>
string to_string(tuple<A, B, C> p);

template <typename A, typename B, typename C, typename D>
string to_string(tuple<A, B, C, D> p);

string to_string(const string& s) {
  return '"' + s + '"';
}

string to_string(const char* s) {
  return to_string((string) s);
}

string to_string(bool b) {
  return (b ? "true" : "false");
}

string to_string(vector<bool> v) {
  bool first = true;
  string res = "{";
  for (int i = 0; i < static_cast<int>(v.size()); i++) {
    if (!first) {
      res += ", ";
    }
    first = false;
    res += to_string(v[i]);
  }
  res += "}";
  return res;
}

template <size_t N>
string to_string(bitset<N> v) {
  string res = "";
  for (size_t i = 0; i < N; i++) {
    res += static_cast<char>('0' + v[i]);
  }
  return res;
}

template <typename A>
string to_string(A v) {
  bool first = true;
  string res = "{";
  for (const auto &x : v) {
    if (!first) {
      res += ", ";
    }
    first = false;
    res += to_string(x);
  }
  res += "}";
  return res;
}

template <typename A, typename B>
string to_string(pair<A, B> p) {
  return "(" + to_string(p.first) + ", " + to_string(p.second) + ")";
}

template <typename A, typename B, typename C>
string to_string(tuple<A, B, C> p) {
  return "(" + to_string(get<0>(p)) + ", " + to_string(get<1>(p)) + ", " + to_string(get<2>(p)) + ")";
}

template <typename A, typename B, typename C, typename D>
string to_string(tuple<A, B, C, D> p) {
  return "(" + to_string(get<0>(p)) + ", " + to_string(get<1>(p)) + ", " + to_string(get<2>(p)) + ", " + to_string(get<3>(p)) + ")";
}

void debug_out() { cerr << endl; }

template <typename Head, typename... Tail>
void debug_out(Head H, Tail... T) {
  cerr << " " << to_string(H);
  debug_out(T...);
}

#ifdef LOCAL
#define debug(...) cerr << "[" << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)
#else
#define debug(...) 42
#endif

const int EPS_COEF = 1000000;

int main(int argc, char** argv) {
  ios::sync_with_stdio(false);
  cin.tie(0);
  if (argc != 2) {
    cerr << "usage: sol [test-id]" << '\n';
    return 0;
  }
  int xid = atoi(argv[1]);
  if (to_string(xid) != argv[1]) {
    cerr << "test-id must be an integer" << '\n';
    return 0;
  }

  ifstream in("../inputs_conv/" + to_string(xid) + ".problem");
  if (!in.is_open()) {
    cerr << "input " << xid << ".problem doesn't exist (check relative path?)" << '\n';
    return 0;
  }
  vector<Point> poly;
  vector<Point> vertices;
  vector<pair<int, int>> edges;
  int np;
  in >> np;
  poly.resize(np);
  for (int i = 0; i < np; i++) {
    in >> poly[i].x >> poly[i].y;
    poly[i].id = i;
  }
  int ne;
  in >> ne;
  edges.resize(ne);
  for (int i = 0; i < ne; i++) {
    in >> edges[i].first >> edges[i].second;
  }
  int nv;
  in >> nv;
  vertices.resize(nv);
  for (int i = 0; i < nv; i++) {
    in >> vertices[i].x >> vertices[i].y;
    vertices[i].id = i;
  }
  int eps;
  in >> eps;
  in.close();

  Evaluator E(poly, vertices, edges, eps);
  int max_x = 0;
  int max_y = 0;
  for (auto& p : poly) {
    max_x = max(max_x, p.x);
    max_y = max(max_y, p.y);
  }
  vector<vector<bool>> has_edge(nv, vector<bool>(nv, false));
  vector<vector<int>> g(nv);
  for (auto& e : edges) {
    has_edge[e.first][e.second] = has_edge[e.second][e.first] = true;
    g[e.first].push_back(e.second);
    g[e.second].push_back(e.first);
  }

  vector<vector<double>> max_dist(nv, vector<double>(nv, 1e9));
  for (int i = 0; i < nv; i++) {
    max_dist[i][i] = 0;
  }
  for (int i = 0; i < nv; i++) {
    for (int j = 0; j < nv; j++) {
      if (has_edge[i][j]) {
        long long old_len = (vertices[i] - vertices[j]).abs2();
        // (new_len - old_len) * EPS_COEF <= eps * old_len
        // new_len <= eps * old_len / EPS_COEF + old_len
        double new_len = (double) old_len + (double) (eps * old_len) / (double) EPS_COEF;
        max_dist[i][j] = sqrt(new_len);
      }
    }
  }
  for (int k = 0; k < nv; k++) {
    for (int i = 0; i < nv; i++) {
      for (int j = 0; j < nv; j++) {
        max_dist[i][j] = min(max_dist[i][j], max_dist[i][k] + max_dist[k][j]);
      }
    }
  }

  vector<vector<long long>> ol(nv, vector<long long>(nv, 1e9));
  for (int i = 0; i < nv; i++)
    for (int j = 0; j < nv; j++)
      ol[i][j] = (vertices[i] - vertices[j]).abs2();

  vector<Point> inner;
  for (int x = 0; x <= max_x; x++) {
    for (int y = 0; y <= max_y; y++) {
      if (E.c.IsPointInside(Point(x, y))) {
        inner.emplace_back(x, y);
      }
    }
  }  

  vector<Point> v(nv, Point(-1, -1));
  auto best_v = v;
  bool found = false;

  ifstream in_ans("../outputs_romka/" + to_string(xid) + ".ans");
  int nv_test;
  in_ans >> nv_test;
  assert(nv == nv_test);
  for (int i = 0; i < nv; i++) {
    in_ans >> v[i].x >> v[i].y;
  }
  in_ans.close();
  int best_score = E.eval(v);
  auto ans_v = v;
  debug(nv, np, eps);

  for (int mv = 0; mv < nv; mv++) {
  // for (int me = 0; me < ne; me++) {
    v = ans_v;
    // vector<int> test = g[edges[me].first];
    // for (int qv : g[edges[me].second]) test.push_back(qv);
    // sort(test.begin(), test.end());
    // test.resize(unique(test.begin(), test.end()) - test.begin());
    vector<int> test = g[mv];
    test.push_back(mv);

    vector<int> t2;
    for (int qv : test) if (qv != VERTEX_ID) t2.push_back(qv);
    test = t2;

    debug(test);
    for (int vd : test) v[vd] = Point(-1, -1);

    vector<bool> taken(nv, false);
    int tkc = 0;
    for (int i = 0; i < nv; i++)
      if (v[i].x != -1) { taken[i] = true; tkc++; }

    vector<vector<Point>> oknp(nv);
    long long prod = 1;
    const size_t KO = 100 + rand() % 500;
    for (int i = 0; i < nv; i++) {
      if (taken[i]) continue;
      for (const auto& tp : inner) {
        bool ok = true;
        for (size_t jj = 0; jj < g[i].size(); jj++) {
          int j = g[i][jj];
          if (v[j].x == -1) continue;
          if (!E.c.IsSegmentInside(tp, v[j])) {
            ok = false;
            break;
          }
          int new_len = (tp - v[j]).abs2();
          int old_len = (vertices[i] - vertices[j]).abs2();
          long long num = abs(new_len - old_len);
          long long den = old_len;
          if (num * EPS_COEF > eps * den) {
            ok = false;
            break;
          }
        }
        for (int j = 0; j < nv && ok; j++) {
          if (!taken[j]) continue;
          auto dist = (tp - v[j]).abs2();
          if (dist > max_dist[i][j] * max_dist[i][j] + 1e-9) {
            ok = false;
            break;
          }
        }      
        if (ok) oknp[i].push_back(tp);
      }
      if (oknp[i].size() > KO) {
        sort(oknp[i].begin(), oknp[i].end(), [&v, &i](const Point& a, const Point& b) { return (a-v[i]).abs2() < (b-v[i]).abs2(); });
        oknp[i].resize(KO);
      }
      cerr << i << " - " << oknp[i].size() << endl;
      
      prod = prod * sqrt(oknp[i].size());
    }

    long long difficulty = prod * nv;
    if (difficulty > 2e6) {
      cerr << ">>> skip this test, difficulty = " << difficulty << " <<<\n";
      continue;
    } else {
      cerr << "difficulty = " << difficulty << endl;
    }

    vector<int> order(nv);
    mt19937 rng(60);
    iota(order.begin(), order.end(), 0);
    shuffle(order.begin(), order.end(), rng);
    sort(order.begin(), order.end(), [&](int i, int j) { return taken[i] > taken[j]; });
    if (nv <= 10) {
      iota(order.begin(), order.end(), 0);
      auto best_order = order;
      auto best_seq = vector<int>(nv, 0);
      do {
        vector<int> seq(nv, 0);
        for (int i = 0; i < nv; i++) {
          for (int j = 0; j < i; j++) {
            if (has_edge[order[i]][order[j]]) {
              seq[i] += 1;
            }
          }
        }
        if (seq > best_seq) {
          best_seq = seq;
          best_order = order;
        }
      } while (next_permutation(order.begin(), order.end()));
      // debug(best_order);
      // debug(best_seq);
      order = best_order;
    } else {
      auto best_order = order;
      auto best_seq = vector<int>(nv, 0);
      for (auto iter = 0; iter < (int)1e3 * nv / (nv - tkc); iter++) {
        // shuffle(order.begin(), order.end(), rng);
        int qi = iter % order.size();
        if (taken[order[qi]]) continue;
        int qj = rand() % order.size();
        if (taken[order[qj]]) continue;
        if (qi == qj) continue;
        swap(order[qi], order[qj]);
        vector<int> seq(nv, 0);
        for (int i = 0; i < nv; i++) {
          for (int j = 0; j < i; j++) {
            if (has_edge[order[i]][order[j]]) {
              seq[i] += 1;
            }
          }
        }
        if (seq > best_seq) {
          best_seq = seq;
          best_order = order;
        } else {
          swap(order[qi], order[qj]);
        }
      }
      // debug(best_order);
      // debug(best_seq);
      order = best_order;
    }

    function<void(int)> Dfs = [&](int ii) {
      if (found) return;
      // cerr << ii << " of " << nv << endl;
      if (ii == nv) {
        int score = (int) E.eval(v);
        if (score != -1) {
          if (score < best_score) {
            best_score = score;
            best_v = v;
            debug(best_v);
            debug(score);
            ofstream out("../outputs_romka/" + to_string(xid) + ".ans");
            out << best_v.size() << '\n';
            for (auto& p : best_v) {
              out << p.x << " " << p.y << '\n';
            }
            out.close();
          }
        }
        return;
      }
      int i = order[ii];
      if (v[i].x != -1) {
        Dfs(ii + 1);
        return;
      }
      for (size_t qq = 0; qq < oknp[i].size(); qq++) {
        const Point& pc = oknp[i][qq];
        v[i] = pc;
        bool ok = true;      
        for (size_t jj = 0; jj < g[i].size(); jj++) {
          int j = g[i][jj];
          if (v[j].x == -1) continue;
          if (!E.c.IsSegmentInside(v[i], v[j])) {
            ok = false;
            break;
          }
          int new_len = (v[i] - v[j]).abs2();
          int old_len = ol[i][j];
          long long num = abs(new_len - old_len);
          if (num * EPS_COEF > eps * old_len) {
            ok = false;
            break;
          }
        }

        for (int jj = 0; jj < nv && ok; jj++) {
          if (ii == jj || v[order[jj]].x == -1) {
            continue;
          }
          
          int j = order[jj];
          auto dist = (v[i] - v[j]).abs2();
          if (dist > max_dist[i][j] * max_dist[i][j] + 1e-9) {
            ok = false;
            break;
          }
        }
        if (ok) {
          Dfs(ii + 1);
        }
        v[i] = Point(-1, -1);
      }
    };

    Dfs(0);
    cout << "done for " << mv << " of " << nv << ", best_score = " << best_score << endl;
    debug(xid);
    // cout << "done for " << me << " of " << ne << ", best_score = " << best_score << endl;
  }

  cout << "totally done\n";
  return 0;
}
