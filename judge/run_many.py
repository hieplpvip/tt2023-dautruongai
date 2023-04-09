#!/usr/bin/env python3

import argparse
import os
from multiprocessing import Pool

from beautifultable import BeautifulTable

from match import Match, MatchStatus
from utility import *


def print_result(exe0, exe1, maps, result):
    table = BeautifulTable(maxwidth=9999)
    maps += ['Total']
    for map_filename in maps:
        table.rows.append([
            result[map_filename]['first_win'],
            result[map_filename]['second_win'],
            result[map_filename]['draw'],
        ])
    table.rows.header = maps
    table.columns.header = [f'{exe0} wins', f'{exe1} wins', 'draw']
    print(table)


def single_thread(args):
    if args.no_color:
        disable_color()

    disable_all_logging()
    if not args.no_log:
        enable_logging('header')
        enable_logging('important')

    exe0 = os.path.abspath(args.exe0)
    exe1 = os.path.abspath(args.exe1)
    times = args.times
    with open(args.map_list, 'r', encoding='utf8') as f:
        maps = []
        for line in f.readlines():
            line = line.strip()
            if line and not line.startswith('#'):
                maps.append(line)

    result = {}
    result['Total'] = {
        'first_win': 0,
        'second_win': 0,
        'draw': 0,
    }

    for map_filename in maps:
        map_abs_path = os.path.abspath(os.path.join(os.path.dirname(args.map_list), map_filename))
        print_header(f'Running {exe0} VS {exe1} on {map_filename} {times} times')

        result[map_filename] = {
            'first_win': 0,
            'second_win': 0,
            'draw': 0,
        }

        for i in range(int(args.times)):
            print_header(f'{i + 1}-th run:')
            match = Match(map_abs_path, exe0, exe1)
            while not match.finished:
                match.next_turn()
                match.print_state()
            print_header('Result:', match.status)
            print_header('')

            if match.status == MatchStatus.FIRST_WIN:
                match_result = 'first_win'
            elif match.status == MatchStatus.SECOND_WIN:
                match_result = 'second_win'
            else:
                match_result = 'draw'

            result[map_filename][match_result] += 1
            result['Total'][match_result] += 1

    print_result(args.exe0, args.exe1, maps, result)


def run_job(map_filename, exe0, exe1):
    disable_color()
    disable_all_logging()

    match = Match(map_filename, exe0, exe1)
    while not match.finished:
        match.next_turn()

    if match.status == MatchStatus.FIRST_WIN:
        return 'first_win'
    elif match.status == MatchStatus.SECOND_WIN:
        return 'second_win'
    else:
        return 'draw'


def multi_threads(args):
    disable_color()
    disable_all_logging()

    exe0 = os.path.abspath(args.exe0)
    exe1 = os.path.abspath(args.exe1)
    times = args.times
    with open(args.map_list, 'r', encoding='utf8') as f:
        maps = []
        for line in f.readlines():
            line = line.strip()
            if line and not line.startswith('#'):
                maps.append(line)

    maps_abs_path = [
        os.path.abspath(os.path.join(os.path.dirname(args.map_list), map_filename)) for map_filename in maps
    ]

    jobs = list(zip(maps_abs_path * times, [exe0] * len(maps_abs_path) * times, [exe1] * len(maps_abs_path) * times))

    with Pool(args.num_threads) as p:
        job_results = p.starmap(run_job, jobs)

    result = {}
    result['Total'] = {
        'first_win': 0,
        'second_win': 0,
        'draw': 0,
    }

    for map_filename in maps:
        result[map_filename] = {
            'first_win': 0,
            'second_win': 0,
            'draw': 0,
        }
    for map_filename, job_result in zip(maps * times, job_results):
        result[map_filename][job_result] += 1
        result['Total'][job_result] += 1

    print_result(args.exe0, args.exe1, maps, result)


def main():
    parser = argparse.ArgumentParser(description='Run a match between two agents')
    parser.add_argument('map_list', type=str, help='Path to map list file')
    parser.add_argument('exe0', type=str, help='Path to first agent')
    parser.add_argument('exe1', type=str, help='Path to second agent')
    parser.add_argument('--times', type=int, default=1, help='Number of times to run for each map')
    parser.add_argument('--num-threads', type=int, default=1, help='Number of threads')
    parser.add_argument('--no-color', default=False, action='store_true', help='Disable color output')
    parser.add_argument('--no-log', default=False, action='store_true', help='Disable logging')
    args = parser.parse_args()

    assert args.num_threads > 0, 'Number of threads must be greater than 0'

    if args.num_threads == 1:
        single_thread(args)
    else:
        multi_threads(args)


if __name__ == '__main__':
    main()
