#!/usr/bin/env python3

import sys
import glob

sys.path.append('../judge')

from sea_map import SeaMap
from visualization import visualize_map_to_image


def main():
    for filename in glob.glob('*/map_*.txt'):
        output = filename.replace('.txt', '.png')
        sea_map = SeaMap(filename)
        visualize_map_to_image(sea_map, output, header_str=filename)


if __name__ == '__main__':
    main()
