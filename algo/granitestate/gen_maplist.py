#!/usr/bin/env python3

import os


def main():
    with open('maplist.txt', 'r', encoding='utf8') as f:
        rows = f.readlines()

    MAPLIST_LEN = len(rows)
    MAP_M = []
    MAP_N = []
    MAPS = []
    ALGOS = []

    for row in rows:
        filename, algo = row.strip().split()
        filename = os.path.abspath(os.path.join('../../maps', filename))
        # print(filename, algo)

        with open(filename, 'r', encoding='utf8') as f:
            M, N, _ = list(map(int, f.readline().strip().split(' ')))

            MAP_M.append(M)
            MAP_N.append(N)
            ALGOS.append(algo)

            s = []
            for _ in range(M):
                row = f.readline().strip().split(' ')
                s.extend(row)
            MAPS.append('{' + ', '.join(map(lambda x: f"'{x}'", s)) + '}')

    output = \
"""#ifndef MAP_LIST_H
#define MAP_LIST_H

#include "constant.h"
#include <vector>

"""
    output += f'constexpr int MAPLIST_LEN = {MAPLIST_LEN};\n\n'
    output += f'constexpr int MAP_M[MAPLIST_LEN] = {{{", ".join(map(str, MAP_M))}}};\n\n'
    output += f'constexpr int MAP_N[MAPLIST_LEN] = {{{", ".join(map(str, MAP_N))}}};\n\n'
    output += 'const std::vector<char> MAPS[MAPLIST_LEN] = {\n'
    for m in MAPS:
        output += f'    {m},\n'
    output += '};\n\n'
    output += 'const int ALGOS[MAPLIST_LEN] = {\n'
    for a in ALGOS:
        output += f'    {a},\n'
    output += '};\n\n'
    output += '#endif\n'

    with open('maplist.h', 'w', encoding='utf8') as f:
        f.write(output)


if __name__ == '__main__':
    main()
