#!/usr/bin/env python3

import argparse
import os

from beautifultable import BeautifulTable

from match import Match, MatchStatus
from utility import *


def main():
    parser = argparse.ArgumentParser(description='Run a match between two agents')
    parser.add_argument('map_list', type=str, help='Path to map list file')
    parser.add_argument('exe0', type=str, help='Path to first agent')
    parser.add_argument('exe1', type=str, help='Path to second agent')
    parser.add_argument('--times', type=int, default=1, help='Number of times to run for each map')
    parser.add_argument('--no-color', default=False, action='store_true', help='Disable color output')
    args = parser.parse_args()

    if args.no_color:
        disable_color()

    disable_all_logging()
    enable_logging('header')
    enable_logging('important')

    exe0 = os.path.abspath(args.exe0)
    exe1 = os.path.abspath(args.exe1)
    times = args.times
    with open(args.map_list, 'r', encoding='utf8') as f:
        maps = list(map(str.strip, f.readlines()))

    result = {}

    for map_filename in maps:
        map_abs_path = os.path.join(os.path.dirname(args.map_list), map_filename)
        print_header(f'Running {exe0} VS {exe1} on {map_filename} {times} times')

        result[map_filename] = {
            args.exe0: 0,
            args.exe1: 0,
            'draw': 0,
        }

        for i in range(int(args.times)):
            print_header(f'{i + 1}-th run:')
            match = Match(map_abs_path, exe0, exe1)
            while not match.finished:
                match.next_turn()
                match.print_state()
            print_header('Result:', match.status)
            print('')

            if match.status == MatchStatus.FIRST_WIN:
                result[map_filename][args.exe0] += 1
            elif match.status == MatchStatus.SECOND_WIN:
                result[map_filename][args.exe1] += 1
            elif match.status == MatchStatus.DRAW:
                result[map_filename]['draw'] += 1

    table = BeautifulTable()
    for map_filename in maps:
        table.rows.append([
            result[map_filename][args.exe0],
            result[map_filename][args.exe1],
            result[map_filename]['draw'],
        ])
    table.rows.header = maps
    table.columns.header = [f'{args.exe0} wins', f'{args.exe1} wins', 'draw']
    print(table)


if __name__ == '__main__':
    main()
