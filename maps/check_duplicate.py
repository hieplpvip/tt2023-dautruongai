#!/usr/bin/env python3

import sys
import glob
from collections import defaultdict

sys.path.append('../judge')

from sea_map import SeaMap
from utility import disable_all_logging


def main():
    disable_all_logging()

    value_to_names = defaultdict(list)
    for filename in glob.glob('*/map_*.txt'):
        sea_map = SeaMap(filename)
        value = tuple([sea_map.M, sea_map.N] + [x for row in sea_map.map for x in row])
        value_to_names[value].append(filename)

    for value, names in value_to_names.items():
        if len(names) > 1:
            print('Duplicate maps:')
            for name in names:
                print(name)
            print()


if __name__ == '__main__':
    main()
