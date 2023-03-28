#!/usr/bin/env python3

import argparse

from sea_map import SeaMap
from visualization import visualize_map_to_image, visualize_map_to_screen


def main():
    parser = argparse.ArgumentParser(description='Visualize map')
    parser.add_argument('map', type=str, help='Path to map file')
    parser.add_argument('--output',
                        type=str,
                        required=False,
                        help='Path to output file. Leave empty to show on screen.')
    args = parser.parse_args()

    sea_map = SeaMap(args.map)
    if args.output:
        visualize_map_to_image(sea_map, args.output)
    else:
        visualize_map_to_screen(sea_map)


if __name__ == '__main__':
    main()
