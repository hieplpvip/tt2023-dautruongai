#include "Store.h"
#include <cstring>
#include <fstream>
#include <queue>

namespace Store {
  int M, N, K, HALF_K;
  int currentTurn;

  int dist[2][15][15][15][15];

  bool isLegalMove[2][15][15][NUM_MOVES];

  State pastState;

  void load() {
    std::ifstream fin("STATE.OUT", std::ios::binary);
    fin.read((char*)&M, sizeof(M));
    fin.read((char*)&N, sizeof(N));
    fin.read((char*)&K, sizeof(K));
    fin.read((char*)&HALF_K, sizeof(HALF_K));
    fin.read((char*)&currentTurn, sizeof(currentTurn));

    fin.read((char*)&dist, sizeof(dist));

    fin.read((char*)&isLegalMove, sizeof(isLegalMove));

    fin.read((char*)&pastState, sizeof(State));
    fin.close();
  }

  void save() {
    std::ofstream fout("STATE.OUT", std::ios::binary);
    fout.write((char*)&M, sizeof(M));
    fout.write((char*)&N, sizeof(N));
    fout.write((char*)&K, sizeof(K));
    fout.write((char*)&HALF_K, sizeof(HALF_K));
    fout.write((char*)&currentTurn, sizeof(currentTurn));

    fout.write((char*)&dist, sizeof(dist));

    fout.write((char*)&isLegalMove, sizeof(isLegalMove));

    fout.write((char*)&pastState, sizeof(State));
    fout.close();
  }

  void update() {
    // Update opponent's shield/gold
    {
      rootState.eliminated[0] = Store::pastState.eliminated[0];
      rootState.eliminated[1] = Store::pastState.eliminated[1];
      rootState.gold[1] = Store::pastState.gold[1];
      rootState.hasShield[1] = Store::pastState.hasShield[1];

      auto [x, y] = rootState.pos[1];
      int prevCell = Store::pastState.at[x][y];
      if (prevCell == SHIELD_CELL) {
        rootState.hasShield[1] = true;
      } else if (prevCell == DANGER_CELL) {
        if (!rootState.hasShield[1]) {
          rootState.eliminated[1] = true;
        }
      } else if (prevCell != EMPTY_CELL) {
        rootState.gold[1] += prevCell;
      }
    }

    // Update score, gold, lastPos
    {
      rootState.score[0] = rootState.gold[0];
      rootState.score[1] = rootState.gold[1];

      rootState.lastPos[0] = Store::pastState.pos[0];
      rootState.lastPos[1] = Store::pastState.pos[1];
    }

    // Set current phase
    {
      int sum = Store::pastState.gold[0] + Store::pastState.gold[1];
      if (sum <= THRESHOLD_EARLY_GAME) {
        rootState.phase = GamePhaseEnum::EARLY_GAME;
      } else if (sum <= THRESHOLD_MID_GAME) {
        rootState.phase = GamePhaseEnum::MID_GAME;
      } else {
        rootState.phase = GamePhaseEnum::LATE_GAME;
      }
    }

    // Update store with new data
    Store::currentTurn += 1;
    Store::pastState = rootState;
  }

  void init() {
    // Init opponent's shield/gold
    {
      rootState.eliminated[0] = false;
      rootState.eliminated[1] = false;
      rootState.gold[1] = 0;
      rootState.hasShield[1] = false;
    }

    // Init score
    {
      rootState.score[0] = 0;
      rootState.score[1] = 0;
    }

    // Game phase
    {
      rootState.phase = EARLY_GAME;
    }

    // Calculate distNoShield/distWithShield using BFS
    {
      REPL_ALL_CELL(x1, y1) {
        REPL_ALL_CELL(x2, y2) {
          Store::dist[0][x1][y1][x2][y2] = Store::dist[1][x1][y1][x2][y2] = INF;
        }
      }

#define dist(x, y) Store::dist[0][sx][sy][x][y]
      std::queue<Position> q;
      REPL_ALL_CELL(sx, sy) {
        if (rootState.at[sx][sy] == DANGER_CELL) continue;

        dist(sx, sy) = 0;
        q.emplace(sx, sy);
        while (!q.empty()) {
          auto [x, y] = q.front();
          q.pop();
          for (int k = 0; k < NUM_MOVES; ++k) {
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (isValidPos(nx, ny) && rootState.at[nx][ny] != DANGER_CELL && dist(nx, ny) == INF) {
              dist(nx, ny) = dist(x, y) + 1;
              q.emplace(nx, ny);
            }
          }
        }
      }
#undef dist

#define dist(x, y) Store::dist[1][sx][sy][x][y]
      REPL_ALL_CELL(sx, sy) {
        dist(sx, sy) = 0;
        q.emplace(sx, sy);
        while (!q.empty()) {
          auto [x, y] = q.front();
          q.pop();
          for (int k = 0; k < NUM_MOVES; ++k) {
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (isValidPos(nx, ny) && dist(nx, ny) == INF) {
              dist(nx, ny) = dist(x, y) + 1;
              q.emplace(nx, ny);
            }
          }
        }
      }
#undef dist
    }

    // Calculate numLegalMovesNoShield/numLegalMovesWithShield/isLegalMoveNoShield/isLegalMoveWithShield
    {
      memset(Store::isLegalMove, 0, sizeof(Store::isLegalMove));

      REPL_ALL_CELL(x, y) {
        for (int k = 0; k < NUM_MOVES; ++k) {
          int nx = x + dx[k];
          int ny = y + dy[k];

          if (isValidPos(nx, ny)) {
            Store::isLegalMove[1][x][y][k] = true;

            if (rootState.at[nx][ny] != DANGER_CELL) {
              Store::isLegalMove[0][x][y][k] = true;
            }
          }
        }
      }
    }

    // Initialize other fields of store
    // M, N, K, HALF_K is already set in main::readInput()
    Store::currentTurn = 1;
    Store::pastState = rootState;
  }
}
