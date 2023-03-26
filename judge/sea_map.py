import random
from collections import deque

from utility import *


class SeaMap:
    """
    Load map from file and manage it
    """

    def __init__(self, filename):
        with open(filename, 'r', encoding='utf8') as f:
            tmp = list(map(int, f.readline().strip().split(' ')))
            assert (len(tmp) == 3), 'Map must start with M N K'

            # Read M, N, and K
            self.M = tmp[0]
            self.N = tmp[1]
            self.K = tmp[2]
            assert self.M <= 15, 'M must be at most 15'
            assert self.M % 2 == 1, 'M must be odd'
            assert self.N <= 15, 'N must be at most 15'
            assert self.N % 2 == 1, 'N must be odd'
            assert self.K <= 100, 'K must be at most 100'
            assert self.K % 2 == 0, 'K must be even'

            # Read the map
            self.map = []
            for _ in range(self.M):
                row = f.readline().strip().split(' ')
                assert (len(row) == self.N), 'Row must have N cells'
                self.map.append(row)

            # Check if each cell is either 'D' or 'S' or an integer in [1, 5]
            count_danger = 0
            count_shield = 0
            count_free = 0
            total_gold = 0
            for i in range(self.M):
                for j in range(self.N):
                    cell = self.map[i][j]
                    if cell == 'D':
                        count_danger += 1
                    elif cell == 'S':
                        count_shield += 1
                    elif cell == '0':
                        count_free += 1
                        self.map[i][j] = 0
                    else:
                        assert cell in ['1', '2', '3', '4', '5'], 'Invalid gold value'
                        self.map[i][j] = int(cell)
                        total_gold += int(cell)

            assert count_shield == 2, 'Map must have exactly 2 shields'
            assert count_free >= 2, 'Map must have at least 2 free cell for starting positions'
            assert total_gold == 100, 'Sum of gold must be 100'

            # Center of map must be free (so treasure can be placed there)
            assert self.map[self.M // 2][self.N // 2] == 0, 'Center of map must be free'

            # Find any not-danger cell
            start = None
            for i in range(self.M):
                for j in range(self.N):
                    if self.map[i][j] != 'D':
                        start = (i, j)
                        break

                if start is not None:
                    break
            assert start is not None

            # Check if all cells that are not 'D' are connected
            visited = set()
            q = deque()
            q.append(start)
            visited.add(start)
            while len(q) > 0:
                x, y = q.popleft()
                for u, v in self.get_neighbors(x, y):
                    if (u, v) not in visited and self.map[u][v] != 'D':
                        visited.add((u, v))
                        q.append((u, v))
            assert len(visited) == self.M * self.N - count_danger, 'Map must be connected'

            # Check if map is symmetric
            horizontal_symmetric = True
            vertical_symmetric = True
            for i in range(self.M):
                for j in range(self.N):
                    if self.map[i][j] != self.map[self.M - i - 1][j]:
                        horizontal_symmetric = False
                    if self.map[i][j] != self.map[i][self.N - j - 1]:
                        vertical_symmetric = False

            if not horizontal_symmetric and not vertical_symmetric:
                assert self.M == self.N, 'Map must be symmetric'
                main_diagonal_symmetric = True
                anti_diagonal_symmetric = True
                for i in range(self.M):
                    for j in range(self.N):
                        if self.map[i][j] != self.map[j][i]:
                            main_diagonal_symmetric = False
                        if self.map[i][j] != self.map[self.M - j - 1][self.N - i - 1]:
                            anti_diagonal_symmetric = False
                assert main_diagonal_symmetric or anti_diagonal_symmetric, 'Map must be symmetric'

            # Print map information
            print_header('Map loaded successfully')
            print_info('Number of rows =', self.M)
            print_info('Number of columns =', self.N)
            print_info('Number of turns =', self.K)
            print_info('Number of danger cells =', count_danger)
            print_info('Number of free cells =', count_free)
            print_info('Total gold =', total_gold)

    def __iter__(self):
        return iter(self.map)

    def get_neighbors(self, x: int, y: int):
        neighbors = []
        if x > 0:
            neighbors.append((x - 1, y))
        if x < self.M - 1:
            neighbors.append((x + 1, y))
        if y > 0:
            neighbors.append((x, y - 1))
        if y < self.N - 1:
            neighbors.append((x, y + 1))
        return neighbors

    def get(self, x: int, y: int):
        return self.map[x][y]

    def free(self, x: int, y: int):
        assert self.map[x][y] != 'D', 'Cannot free a danger cell'
        self.map[x][y] = 0

    def set_treasure(self, value: int):
        x = self.M // 2
        y = self.N // 2
        assert self.map[x][y] == 0, 'Center of map is not free'
        self.map[x][y] = value

    def random_free_cell(self):
        candidates = []
        for i in range(self.M):
            for j in range(self.N):
                if self.map[i][j] == 0:
                    candidates.append((i, j))
        assert len(candidates) > 0, 'No free cell'
        return random.choice(candidates)

    def is_valid_pos(self, x: int, y: int):
        return 0 <= x < self.M and 0 <= y < self.N

    def is_free(self, x: int, y: int):
        return self.map[x][y] == 0

    def is_danger(self, x: int, y: int):
        return self.map[x][y] == 'D'

    def is_shield(self, x: int, y: int):
        return self.map[x][y] == 'S'

    def is_gold(self, x: int, y: int):
        return self.map[x][y] != 'D' and self.map[x][y] != 'S' and self.map[x][y] != 0
