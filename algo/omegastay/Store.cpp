#include "Store.h"
#include "Constants.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <queue>

State rootState;

namespace Store {
  int M, N, K, HALF_K;
  int currentTurn;
  GamePhaseEnum gamePhase;

  int dist[2][15][15][15][15];
  int numLegalMoves[2][15][15];
  bool isLegalMove[2][15][15][NUM_MOVES];

  State pastState;

  void load() {
    std::ifstream fin("STATE.OUT", std::ios::binary);
    fin.read((char*)&M, sizeof(M));
    fin.read((char*)&N, sizeof(N));
    fin.read((char*)&K, sizeof(K));
    fin.read((char*)&HALF_K, sizeof(HALF_K));
    fin.read((char*)&currentTurn, sizeof(currentTurn));
    fin.read((char*)&gamePhase, sizeof(gamePhase));
    fin.read((char*)&dist, sizeof(dist));
    fin.read((char*)&numLegalMoves, sizeof(numLegalMoves));
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
    fout.write((char*)&gamePhase, sizeof(gamePhase));
    fout.write((char*)&dist, sizeof(dist));
    fout.write((char*)&numLegalMoves, sizeof(numLegalMoves));
    fout.write((char*)&isLegalMove, sizeof(isLegalMove));
    fout.write((char*)&pastState, sizeof(State));
    fout.close();
  }

  void init() {
    // Calculate dist
    {
      std::queue<Position> q;

      REPL_ALL_CELL(x1, y1) {
        REPL_ALL_CELL(x2, y2) {
          Store::dist[0][x1][y1][x2][y2] = INF;

          // We can move to any cell with shield, hence Manhattan distance
          Store::dist[1][x1][y1][x2][y2] = abs(x1 - x2) + abs(y1 - y2);
        }
      }

#define dist(x, y) Store::dist[0][sx][sy][x][y]
      // No shield, use BFS
      REPL_ALL_CELL(sx, sy) {
        if (rootState.at[sx][sy] == DANGER_CELL) {
          continue;
        }

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
    }

    // Calculate numLegalMoves and isLegalMove
    {
      memset(Store::numLegalMoves, 0, sizeof(Store::numLegalMoves));
      memset(Store::isLegalMove, 0, sizeof(Store::isLegalMove));

      REPL_ALL_CELL(x, y) {
        for (int k = 0; k < NUM_MOVES; ++k) {
          int nx = x + dx[k];
          int ny = y + dy[k];

          if (isValidPos(nx, ny)) {
            // With shield
            ++Store::numLegalMoves[1][x][y];
            Store::isLegalMove[1][x][y][k] = true;

            if (rootState.at[nx][ny] != DANGER_CELL) {
              // No shield
              ++Store::numLegalMoves[0][x][y];
              Store::isLegalMove[0][x][y][k] = true;
            }
          }
        }
      }
    }

    // Set rootState's lastPos
    rootState.lastPos[0] = rootState.lastPos[1] = {-1, -1};

    // Initialize other fields of store
    // M, N, K, HALF_K is already set in main::readInput()
    Store::currentTurn = 1;
    Store::pastState = rootState;
    Store::gamePhase = EARLY_GAME;
  }

  void update() {
    assert(rootState.turnLeft < Store::K);
    assert(Store::currentTurn < Store::K);

    // Update opponent's shield and gold
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

    // Update rootState's lastPos
    rootState.lastPos[0] = Store::pastState.pos[0];
    rootState.lastPos[1] = Store::pastState.pos[1];

    // Set current game phase
    {
      int sum = rootState.gold[0] + rootState.gold[1];
      if (sum <= THRESHOLD_EARLY_GAME) {
        Store::gamePhase = GamePhaseEnum::EARLY_GAME;
      } else if (sum <= THRESHOLD_MID_GAME) {
        Store::gamePhase = GamePhaseEnum::MID_GAME;
      } else {
        Store::gamePhase = GamePhaseEnum::LATE_GAME;
      }
    }

    // Update other fields
    Store::currentTurn += 1;
    Store::pastState = rootState;
  }
}
