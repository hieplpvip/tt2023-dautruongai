from enum import Enum

from sea_map import SeaMap
from sandbox import SandBox


class MatchStatus(Enum):
    UNDECIDED = 0
    FIRST_WIN = 1
    SECOND_WIN = 2
    DRAW = 2


class Match:
    """
    Referee a match between two teams
    """

    def __init__(self, map_filename, exe0, exe1):
        self.sea_map = SeaMap(map_filename)
        self.sandbox = [SandBox(exe0), SandBox(exe1)]

        # Game state
        self.current_turn = 1
        self.total_turn = self.sea_map.K
        self.position = [(-1, -1), (-1, -1)]
        self.eliminated = [False, False]
        self.have_shield = [False, False]
        self.gold = [0, 0]

    def status(self):
        if self.current_turn > self.total_turn or (self.eliminated[0] and self.eliminated[1]):
            if self.gold[0] > self.gold[1]:
                return MatchStatus.FIRST_WIN

            if self.gold[1] > self.gold[0]:
                return MatchStatus.SECOND_WIN

            return MatchStatus.DRAW

        if self.current_turn <= self.total_turn // 2:
            return MatchStatus.UNDECIDED

        if self.gold[0] - self.gold[1] > 20:
            return MatchStatus.FIRST_WIN

        if self.gold[1] - self.gold[0] > 20:
            return MatchStatus.SECOND_WIN

        return MatchStatus.UNDECIDED

    def next_turn(self):
        if self.status() != MatchStatus.UNDECIDED:
            print('Match is over')
            return False

        print(f'Turn {self.current_turn}/{self.total_turn}')

        if self.current_turn == 1:
            # First turn is for selecting start positions:
            # - Each team can choose any free cell to start.
            # - If a team chooses an invalid cell, that team is assigned a random free cell.
            # - If both teams choose the same free cell, retry at most 3 times.
            # - After 3 unsuccessful attempts, both teams are assigned two different random free cells.
            print('Selecting start positions')

            move = []
            for attempt in range(3):
                move.clear()
                for i in range(2):
                    self.sandbox[i].prepare(0, 0, 0, 0, 0, 0, self.total_turn, self.sea_map)
                    x, y = self.sandbox[i].run()
                    move.append((x - 1, y - 1))

                valid = [self.sea_map.is_valid_pos(x, y) and self.sea_map.is_free(x, y) for x, y in move]

                if valid[0] and valid[1]:
                    if move[0] != move[1]:
                        print(f'Attempt {attempt + 1}: Accept both.')
                        break

                    if attempt < 2:
                        print(f'Attempt {attempt + 1}: Both choose the same position. Retry.')
                        continue

                if valid[0] and not valid[1]:
                    print(f'Attempt {attempt + 1}: Accept team 1, randomize team 2.')
                    while True:
                        move[1] = self.sea_map.random_free_cell()
                        if move[0] != move[1]:
                            break
                    break

                if valid[1] and not valid[0]:
                    print(f'Attempt {attempt + 1}: Accept team 2, randomize team 1.')
                    while True:
                        move[0] = self.sea_map.random_free_cell()
                        if move[0] != move[1]:
                            break
                    break

                print(f'Attempt {attempt + 1}: Randomize both.')
                while True:
                    move[0] = self.sea_map.random_free_cell()
                    move[1] = self.sea_map.random_free_cell()
                    if move[0] != move[1]:
                        break

            self.position[0] = move[0]
            self.position[1] = move[1]
            self.current_turn += 1

            print(f'Team 1: {self.position[0][0] + 1} {self.position[0][1] + 1}')
            print(f'Team 2: {self.position[1][0] + 1} {self.position[1][1] + 1}')

            return True

        # Every turn since the second turn:
        # - Each team makes a move simultaneously
        # - If a team makes an invalid move (out of bound or not sharing sides with current cell),
        #   that team stays in the current cell.
        # - If both teams move to the same cell, both are eliminated.
        # - If teams swap positions, they are also eliminated.
        # - If a team moves to a danger cell without shield, that team is eliminated.
        # - An eliminated team stays in the current cell till the end of the game.
        # - If a live team moves to a cell with gold (or shield),
        #   they collect the gold (or shield) and the cell becomes free.
        # - The collected gold are not lost if the team is eliminated.

        move, valid = [], []
        for i in range(2):
            if self.eliminated[i]:
                move.append((-1, -1))
                valid.append(False)
                continue

            self.sandbox[i].prepare(
                self.position[i][0] + 1,
                self.position[i][1] + 1,
                self.position[i ^ 1][0] + 1,
                self.position[i ^ 1][1] + 1,
                1 if self.have_shield[i] else 0,
                self.gold[i],
                self.total_turn - self.current_turn + 1,
                self.sea_map,
            )
            x, y = self.sandbox[i].run()
            move.append((x - 1, y - 1))
            valid.append((x, y) in self.sea_map.get_neighbors(*self.position[i]))

        # New positions after this turn
        after_pos = [move[i] if valid[i] else self.position[i] for i in range(2)]

        # Eliminate if both teams move to the same cell or swap cells
        if after_pos[0] == after_pos[1] or (after_pos[0] == self.position[1] and after_pos[1] == self.position[0]):
            print('Teams move to same cell/swap cells. Eliminate both.')
            self.eliminated[0] = True
            self.eliminated[1] = True
            return True

        # valid only checks if the move is valid in terms of sharing sides with current cell
        # Eliminate if team moves to a danger cell without shield
        for i in range(2):
            if valid[i] and self.sea_map.is_danger(*after_pos[i]) and not self.have_shield[i]:
                print(f'Team {i + 1} moves to danger cell. Eliminate.')
                self.eliminated[i] = True

        # If team is still alive, let them collect gold/shield
        for i in range(2):
            if self.eliminated[i]:
                continue

            x, y = after_pos[i]
            if self.sea_map.is_shield(x, y):
                print(f'Team {i + 1} collects shield.')
                self.have_shield[i] = True
                self.sea_map.free(x, y)
            elif self.sea_map.is_gold(x, y):
                amount = self.sea_map.get(x, y)
                print(f'Team {i + 1} collects {amount} gold.')
                self.gold[i] += amount
                self.sea_map.free(x, y)

        return True
