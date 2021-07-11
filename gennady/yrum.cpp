/**
 *    author:  tourist
 *    created: 09.07.2021 17:21:07
**/
#undef _GLIBCXX_DEBUG

#include <chrono>
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
  for (auto& e : edges) {
    has_edge[e.first][e.second] = has_edge[e.second][e.first] = true;
  }

  for (int very_outer = 0; very_outer < 100; very_outer++) {

  vector<Point> v(nv, Point(-1, -1));
  ifstream in_ans("../outputs/" + to_string(xid) + ".ans");
  int nv_test;
  in_ans >> nv_test;
  assert(nv == nv_test);
  for (int i = 0; i < nv; i++) {
    in_ans >> v[i].x >> v[i].y;
  }
  in_ans.close();

  auto best_score = E.eval(v);
  debug(best_score);
//  cout << E.error_msg << '\n';

  auto best_v = v;

          for (int i = 0; i < nv; i++) {
            for (int j = i + 1; j < nv; j++) {
              if (has_edge[i][j]) {
                int now = (v[i] - v[j]).abs2();
                int old = (vertices[j] - vertices[i]).abs2();
                double ratio = 1.0 * now / old;
                if (abs(ratio - 1) > 1.0 * eps / EPS_COEF + 1e-9) {
                  debug(i, j, ratio, eps);
                  return 0;
                }
              }
            }
          }

  vector<vector<int>> nei(nv);
  for (auto& e : edges) {
    int i = e.first;
    int j = e.second;
    nei[i].push_back(j);
    nei[j].push_back(i);
  }
  mt19937 rng((unsigned int) chrono::steady_clock::now().time_since_epoch().count());
  double init_temp = 100000;
  double final_temp = 0.1;
//  double init_temp = 0.1;
//  double final_temp = 0.0001;
  const int ITERS = 2000;
  for (int outer = 0; outer < ITERS; outer++) {
    double temp = init_temp * pow(final_temp / init_temp, outer * 1.0 / ITERS);
    for (int rep = 0; rep < nv; rep++) {
      int i = rng() % nv;
      int dx = (int) (rng() % 3) - 1;
      int dy = (int) (rng() % 3) - 1;
      if (dx == 0 && dy == 0) continue;
/*      if (rng() % 2 == 0) {
        dx = (int) (rng() % 101) - 50;
        dy = (int) (rng() % 101) - 50;
      }*/
      auto new_vi = v[i] + Point(dx, dy);
      if (E.c.IsPointInside(new_vi)) {
        vector<bool> moved(nv, false);
        vector<int> que(1, i);
        moved[i] = true;
        bool fail = false;
        for (int b = 0; b < (int) que.size(); b++) {
          for (int j : nei[que[b]]) {
            if (has_edge[que[b]][j]) {
              if (!moved[j]) {
                bool bad_len = false;
                Point new_vq = v[que[b]] + Point(dx, dy);
                int now = (new_vq - v[j]).abs2();
                int old = (vertices[j] - vertices[que[b]]).abs2();
                double ratio = 1.0 * now / old;
                if (abs(ratio - 1) > 1.0 * eps / EPS_COEF + 1e-9) {
                  bad_len = true;
                }
                if (bad_len || !E.c.IsSegmentInside(v[que[b]] + Point(dx, dy), v[j])) {
                  if (!E.c.IsPointInside(v[j] + Point(dx, dy))) {
                    fail = true;
                    break;
                  }
                  que.push_back(j);
                  moved[j] = true;
                }
              } else {
                bool bad_len = false;
                Point new_vq = v[que[b]] + Point(dx, dy);
                Point new_vj = v[j] + Point(dx, dy);
                int now = (new_vq - new_vj).abs2();
                int old = (vertices[j] - vertices[que[b]]).abs2();
                double ratio = 1.0 * now / old;
                if (abs(ratio - 1) > 1.0 * eps / EPS_COEF + 1e-9) {
                  bad_len = true;
                }
                if (bad_len || !E.c.IsSegmentInside(v[que[b]] + Point(dx, dy), v[j] + Point(dx, dy))) {
                  fail = true;
                  break;
                }
              }
            }
          }
          if (fail) {
            break;
          }
        }
        if (fail) {
          continue;
        }
        double func = 0;
        for (auto& e : edges) {
          int i = e.first;
          int j = e.second;
          Point new_vi = v[i] + (moved[i] ? Point(dx, dy) : Point(0, 0));
          Point new_vj = v[j] + (moved[j] ? Point(dx, dy) : Point(0, 0));
          int old_now = (v[j] - v[i]).abs2();
          int now = (new_vj - new_vi).abs2();
          int old = (vertices[j] - vertices[i]).abs2();
          double old_ratio = 1.0 * old_now / old;
          double ratio = 1.0 * now / old;
          if (ratio > 1 + 1.0 * eps / EPS_COEF + 1e-9) {
            debug(i, j, que, moved, ratio, eps, old_now, now, old);
            assert(false);
//                ok = false;
//                break;
          }
          if (ratio < 1 - 1.0 * eps / EPS_COEF - 1e-9) {
            debug(i, j, que, moved, ratio, eps, old_now, now, old);
            assert(false);
//                ok = false;
//                break;
          }
/*              auto z = abs(ratio - 1) / (1.0 * eps / EPS_COEF);
              auto old_z = abs(old_ratio - 1) / (1.0 * eps / EPS_COEF);
              func += 0.00 * (pow(z, 2) - pow(old_z, 2)) * (ITERS - outer) / ITERS;*/
        }
/*        if (!ok) {
          continue;
        }*/
        for (int ii = 0; ii < np; ii++) {
          int mn = (int) 1e9;
          for (int j = 0; j < nv; j++) {
            mn = min(mn, (poly[ii] - v[j]).abs2());
          }
          func -= mn;
        }
        auto old_v = v;
        for (int i : que) {
          v[i] = v[i] + Point(dx, dy);
        }
        for (int ii = 0; ii < np; ii++) {
          int mn = (int) 1e9;
          for (int j = 0; j < nv; j++) {
            mn = min(mn, (poly[ii] - v[j]).abs2());
          }
          func += mn;
        }
  //            debug(func);
        if (func <= 0 || (func > 0 && 1.0 * rng() / (1LL << 32) < exp(-func / temp))) {
          // ok!
/*          debug("hi", que);
          for (int i = 0; i < nv; i++) {
            for (int j = i + 1; j < nv; j++) {
              if (has_edge[i][j]) {
                int now = (v[i] - v[j]).abs2();
                int old = (vertices[j] - vertices[i]).abs2();
                double ratio = 1.0 * now / old;
                if (abs(ratio - 1) > 1.0 * eps / EPS_COEF + 1e-9) {
                  debug(i, j, que);
                  return 0;
                }
              }
            }
          }*/
        } else {
          v = old_v;
        }
      }
    }
    auto score = E.eval(v);
    if (score < best_score) {
      best_score = score;
      best_v = v;
    }
    if (outer % 1000 == 0)
      debug(temp, score, best_score);
//    cout << E.error_msg << '\n';
  }
  debug(E.eval(v), best_score);

  ofstream out("../outputs/" + to_string(xid) + ".ans");
  out << best_v.size() << '\n';
  for (auto& p : best_v) {
    out << p.x << " " << p.y << '\n';
  }
  out.close();

  }
  return 0;
}
