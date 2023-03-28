#!/usr/bin/env python3

import argparse
import os
import shutil

from match import Match
from utility import *


def main():
    parser = argparse.ArgumentParser(description='Run a match between two agents')
    parser.add_argument('map', type=str, help='Path to map file')
    parser.add_argument('exe0', type=str, help='Path to first agent')
    parser.add_argument('exe1', type=str, help='Path to second agent')
    parser.add_argument('--visualize', type=str, required=False, help='Path to visualization directory')
    args = parser.parse_args()

    map_filename = os.path.abspath(args.map)
    exe0 = os.path.abspath(args.exe0)
    exe1 = os.path.abspath(args.exe1)
    visualize_dir = args.visualize

    if visualize_dir:
        shutil.rmtree(visualize_dir, ignore_errors=True)
        os.makedirs(visualize_dir, exist_ok=True)

    match = Match(map_filename, exe0, exe1)
    while not match.finished:
        print('')
        match.next_turn(visualize_dir)
        match.print_state()

    print('')
    print_header('Match result:', match.status)


if __name__ == '__main__':
    main()
