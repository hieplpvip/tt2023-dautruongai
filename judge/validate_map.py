#!/usr/bin/env python3

import argparse

from sea_map import SeaMap
from utility import *


def main():
    parser = argparse.ArgumentParser(description='Validate map')
    parser.add_argument('map', type=str, help='Path to map file')
    args = parser.parse_args()
    SeaMap(args.map)


if __name__ == '__main__':
    main()
