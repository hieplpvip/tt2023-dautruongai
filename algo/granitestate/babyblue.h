#ifndef BABYBLUE_H
#define BABYBLUE_H

#include <bits/stdc++.h>

using namespace std;

#define MAXN 20
#define INF 100'000'000

#define MAX_DEPTH 10
#define KSEED 15
#define DANGER -1000
#define SHIELD 100    // change if too close to shield
#define TREASURE 100  // change if too close to treasure
#define KRAND 2
#define EZONE 3   // change if too close the enemy ship
#define BONUS 10  // change if eat too much low gold. If too high, the ship will be scare by enemy when it eating gold
#define RATE 5

#define sqr(a) ((a) * (a))
#define dist(a, b, c, d) (abs((a) - (c)) + abs((b) - (d)))
#define bval(a, b, c) max(a, min(b, c))
#define f(gold, dist) (sqr(gold) / sqrt(dist))

const int dx[] = {-1, 1, 0, 0};
const int dy[] = {0, 0, -1, 1};

typedef double score_t;

namespace BabyBlue {
  int main();
}

#endif  // BABYBLUE_H
