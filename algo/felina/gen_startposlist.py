#!/usr/bin/env python3

import os


def main():
    with open('startpos.txt', 'r', encoding='utf8') as f:
        rows = f.readlines()

    rows = [row for row in rows if not row.startswith('#')]

    MAPLIST_LEN = len(rows)
    MAP_M = []
    MAP_N = []
    MAPS = []
    START_POSITIONS = []

    for row in rows:
        tmp = row.strip().split()
        filename = tmp[0]
        pos = tmp[1:]
        assert len(pos) % 2 == 0

        pos = list(map(int, pos))
        pos = [(pos[i], pos[i + 1]) for i in range(0, len(pos), 2)]

        filename = os.path.abspath(os.path.join('../../maps', filename))
        print(filename, pos)

        with open(filename, 'r', encoding='utf8') as f:
            M, N, _ = list(map(int, f.readline().strip().split(' ')))

            MAP_M.append(M)
            MAP_N.append(N)
            START_POSITIONS.append(pos)

            s = []
            for _ in range(M):
                row = f.readline().strip().split(' ')
                s.extend(row)
            MAPS.append('{' + ', '.join(map(lambda x: f"'{x}'", s)) + '}')

    output = \
"""#ifndef START_POS_H
#define START_POS_H

#include "Constants.h"
#include "State.h"
#include <vector>

namespace StartPosList {
"""
    output += f'  constexpr int MAPLIST_LEN = {MAPLIST_LEN};\n\n'
    output += f'  constexpr int MAP_M[MAPLIST_LEN] = {{{", ".join(map(str, MAP_M))}}};\n\n'
    output += f'  constexpr int MAP_N[MAPLIST_LEN] = {{{", ".join(map(str, MAP_N))}}};\n\n'
    output += '  const std::vector<char> MAPS[MAPLIST_LEN] = {\n'
    for m in MAPS:
        output += f'      {m},\n'
    output += '  };\n\n'
    output += '  // 0-indexed\n'
    output += '  const std::vector<Position> START_POSITIONS[MAPLIST_LEN] = {\n'
    for pos in START_POSITIONS:
        s = ', '.join(map(lambda x: f'{{{x[0] - 1}, {x[1] - 1}}}', pos))
        output += f'      {{{s}}},\n'
    output += '  };\n'
    output += '}\n\n'
    output += '#endif\n'

    with open('StartPosList.h', 'w', encoding='utf8') as f:
        f.write(output)


if __name__ == '__main__':
    main()
