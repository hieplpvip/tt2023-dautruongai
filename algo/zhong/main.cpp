#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

int M, N, K, Xi, Yi, Xj, Yj, S, B, Middle;

vector<vector<string>> read() {
  ifstream infile("MAP.INP");
  infile >> M >> N >> K >> Xi >> Yi >> Xj >> Yj >> S >> B;
  vector<vector<string>> a(M + 2, vector<string>(N + 2));
  for (int i = 1; i <= M; i++) {
    for (int j = 1; j <= N; j++) {
      infile >> a[i][j];
    }
  }
  infile.close();
  return a;
}

vector<vector<string>> a = read();

int getValueFromChar(string s) {
  if (s == "D") {
    return -1000000;
  } else if (s == "S") {
    return 1000000;
  } else if (s == "T") {
    return -1000000;
  } else {
    return stoi(s);
  }
}

int calculateManhattanDistance(int X1, int Y1, int X2, int Y2) {
  return abs(X1 - X2) + abs(Y1 - Y2);
}

long long GetEnemyGold() {
  long long ret = 0;
  for (int i = 1; i <= M; i++) {
    for (int j = 1; j <= N; j++) {
      if (a[i][j] != "S" && a[i][j] != "D" && (i != (M + 1) / 2 || j != (N + 1) / 2)) {
        ret += (long long)getValueFromChar(a[i][j]);
      }
    }
  }
  ret = 100 - ret - S;
  return ret;
}

// Function to generate all sequences of base-4
vector<string> generateBase4Sequences(int length) {
  vector<string> sequences;
  // Loop through all possible sequences of length "length"
  for (int i = 0; i < pow(4, length); i++) {
    string sequence = "";
    int num = i;
    // Convert the number to base-4 and append to the sequence string
    for (int j = 0; j < length; j++) {
      sequence += to_string(num % 4);
      num /= 4;
    }
    // Reverse the sequence string and add to the vector of sequences
    reverse(sequence.begin(), sequence.end());
    sequences.push_back(sequence);
  }
  return sequences;
}

// Function to check if we are going out of the map
bool isOutOfMap(int x, int y, int M, int N) {
  return !((1 <= x && x <= M) && (1 <= y && y <= N));
}

// Function to update x and y based on mov
void updateXY(int mov, int& x, int& y) {
  if (mov == 0) {
    x++;
  } else if (mov == 1) {
    y++;
  } else if (mov == 2) {
    x--;
  } else if (mov == 3) {
    y--;
  }
}

int getQuarter(int X, int Y) {
  int mid_x = (M + 1) / 2, mid_y = (N + 1) / 2;
  if (X <= mid_x && Y <= mid_y) {
    return 1;
  } else if (X <= mid_x && Y > mid_y) {
    return 2;
  } else if (X > mid_x && Y <= mid_y) {
    return 3;
  } else {
    return 4;
  }
}

// Function to calculate all the values in a rectangle from the top left point (X1, Y1) to the bottom right (X2, Y2) of the map
pair<int, int> calculateRectangleValue(int X1, int Y1, int X2, int Y2) {
  if (X1 < 1) X1 = 1;
  if (X1 > M) X1 = M;
  if (Y1 < 1) Y1 = 1;
  if (Y1 > N) Y1 = N;
  if (X2 < 1) X2 = 1;
  if (X2 > M) X2 = M;
  if (Y2 < 1) Y2 = 1;
  if (Y2 > N) Y2 = N;
  int sum = 0, available_cell = 0;
  for (int i = X1; i <= X2; i++) {
    for (int j = Y1; j <= Y2; j++) {
      int value = getValueFromChar(a[i][j]);
      if (0 <= value && value <= 100) {
        sum += value;
        available_cell++;
      }
    }
  }
  return make_pair(sum, available_cell);
}

// Function to get the top_left point and bottom_right point of a rectangle given a point (X, Y) and a side
pair<pair<int, int>, pair<int, int>> getRectangle(int X, int Y, int side) {
  if (side == 1) {
    return make_pair(make_pair(X, Y), make_pair(min(X + 3, M), min(Y + 3, N)));
  } else if (side == 2) {
    return make_pair(make_pair(X, max(0, Y - 3)), make_pair(min(X + 3, M), Y));
  } else if (side == 3) {
    return make_pair(make_pair(max(0, X - 3), Y), make_pair(X, min(Y + 3, N)));
  } else {
    return make_pair(make_pair(max(0, X - 3), max(0, Y - 3)), make_pair(X, Y));
  }
}

// Function to calculate the potential of a given position (X, Y)
int calculatePotential(int X, int Y) {
  int quarter = getQuarter(X, Y);
  pair<pair<int, int>, pair<int, int>> rect = getRectangle(X, Y, quarter);
  auto collection = calculateRectangleValue(rect.first.first, rect.first.second, rect.second.first, rect.second.second);
  return (collection.first == 0) ? 0 : ((K - min(7, K)) * collection.first / collection.second);
}

long long calculateGoldAndPotential(int X, int Y, string moves, int shield) {
  int count = 0, shield_able = shield, X_temp = X, Y_temp = Y;  // Declare a variable count 0
  int weight = 100;
  for (char c : moves) {  // For each character inside the string moves
    // Update X_temp and Y_temp based on the current character value
    updateXY(c - '0', X_temp, Y_temp);
    // Check if we are out of the map
    if (!isOutOfMap(X_temp, Y_temp, M, N)) {
      // Get the value of the current character
      int value = getValueFromChar(a[X_temp][Y_temp]);
      // If the value is greater than 1000, set shield_able to 1
      if (value > 1000) {
        shield_able = 1;
      }
      // If the value is between 0 and 10, add it to count
      else if (value > 0 && value < 100) {
        count += value * weight;
      }
      // If the value is less than 0
      else if (value < 0) {
        // If shield_able is not 1, return -1000000
        if (shield_able != 1) {
          return -1000000;
        }
      }
    } else {
      return -1000000;
    }
    weight -= 10;
  }
  return count + calculatePotential(X_temp, Y_temp) * weight;  // Return the final count
}

long long calculateGoldOnly(int X, int Y, string moves, int shield) {
  int count = 0, shield_able = shield, X_temp = X, Y_temp = Y;  // Declare a variable count 0
  int weight = 100;
  for (char c : moves) {  // For each character inside the string moves
    // Update X_temp and Y_temp based on the current character value
    updateXY(c - '0', X_temp, Y_temp);
    // Check if we are out of the map
    if (!isOutOfMap(X_temp, Y_temp, M, N)) {
      // Get the value of the current character
      int value = getValueFromChar(a[X_temp][Y_temp]);
      // If the value is greater than 1000, set shield_able to 1
      if (value > 10000) {
        shield_able = 1;
      }
      // If the value is between 0 and 10, add it to count
      else if (value > 0 && value < 100) {
        count += value * weight;
      }
      // If the value is less than 0
      else if (value < 0) {
        // If shield_able is not 1, return -1000000
        if (shield_able != 1) {
          return -1000000;
        }
      }
    } else {
      return -1000000;
    }
  }
  return count;  // Return the final count
}

// Function to choose initial start
pair<int, int> chooseInitialStart() {
  // Create an instance of results from generateBase4Sequences(7)
  vector<string> moves = generateBase4Sequences(min(7, K));
  int ans_x = -1, ans_y = -1;
  long long maxgold = -1;
  // Iterator through the map
  for (int i = 1; i <= M; i++) {
    for (int j = 1; j <= N; j++) {
      if (a[i][j] == "0") {
        long long gold_collected = 0;
        for (auto str : moves) {
          gold_collected = max(gold_collected, calculateGoldOnly(i, j, str, 0));
        }
        if (gold_collected > maxgold) {
          maxgold = gold_collected;
          ans_x = i;
          ans_y = j;
        }
      }
    }
  }
  return make_pair(ans_x, ans_y);
}

// Function to choose the next step to take
string chooseStep() {
  // Create an instance of results from generateBase4Sequences(7)
  vector<string> moves = generateBase4Sequences(min(7, K));
  string result = "";
  long long gold_max = -1;
  for (int k = 0; k < moves.size(); k++) {
    int Xi_temp = Xi;
    int Yi_temp = Yi;
    long long val = calculateGoldAndPotential(Xi_temp, Yi_temp, moves[k], B);

    if (val > gold_max) {
      result = moves[k];
      gold_max = val;
    }
  }
  if (result == "") {
    if (!isOutOfMap(Xi + 1, Yi, M, N) && getValueFromChar(a[Xi + 1][Yi]) >= 0) result = "0";
    if (!isOutOfMap(Xi, Yi + 1, M, N) && getValueFromChar(a[Xi][Yi + 1]) >= 0) result = "1";
    if (!isOutOfMap(Xi - 1, Yi, M, N) && getValueFromChar(a[Xi - 1][Yi]) >= 0) result = "2";
    if (!isOutOfMap(Xi, Yi - 1, M, N) && getValueFromChar(a[Xi][Yi - 1]) >= 0) result = "3";
  }
  return result;
}

int main() {
  if (Xi == 0) {
    auto first_start = chooseInitialStart();
    Xi = first_start.first;
    Yi = first_start.second;
  } else {
    long long enemy_gold = GetEnemyGold();
    if (enemy_gold > S) {
      int dist = calculateManhattanDistance(Xi, Yi, Xj, Yj);
      if (dist == 2) {
        a[Xj - 1][Yj] = "T";
        a[Xj][Yj - 1] = "T";
        a[Xj + 1][Yj] = "T";
        a[Xj][Yj + 1] = "T";
      } else if (dist == 1) {
        a[Xj][Yj] = "T";
      }
    }
    string path = chooseStep();
    updateXY(path[0] - '0', Xi, Yi);
  }
  ofstream outstream;
  outstream.open("MOVE.OUT");
  outstream << Xi << " " << Yi << "\n";
  outstream.close();
  return 0;
}
