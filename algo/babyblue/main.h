#ifndef MAIN_H
#define MAIN_H

#include <bits/stdc++.h>

using namespace std;

#define MAXN 20
#define INF 100'000'000

#define MAX_DEPTH 4
#define KSEED 15
#define DANGER -1000
#define SHIELD 5      // change if too close to shield
#define TREASURE 100  // change if too close to treasure
#define KRAND 3
#define EZONE 3        // change if too close the enemy ship
#define BONUS 100'000  // change if eat too much low gold

#define sqr(a) ((a) * (a))
#define dist(a, b, c, d) (abs((a) - (c)) + abs((b) - (d)))
#define bval(a, b, c) max(a, min(b, c))

const int dx[] = {-1, 1, 0, 0};
const int dy[] = {0, 0, -1, 1};

typedef double score_t;

#endif  // MAIN_H
