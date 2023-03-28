import os

os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = 'hide'

import pygame

from sea_map import SeaMap

__all__ = ['visualize_map_to_image', 'visualize_map_to_screen']

# Initialize pygame
pygame.init()

# Define some colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)

# Define some constants
BORDER_COLOR = BLACK
BORDER_WIDTH = 2
TILE_SIZE = 64
INNER_SIZE = TILE_SIZE - BORDER_WIDTH * 2

# Load font
ARIAL_FONT_12 = pygame.font.SysFont('Arial', 12)
ARIAL_FONT_24 = pygame.font.SysFont('Arial', 24)

# Load the tile images
EMPTY_TILE = pygame.Surface((INNER_SIZE, INNER_SIZE))
EMPTY_TILE.fill(WHITE)

DANGER_TILE = pygame.Surface((INNER_SIZE, INNER_SIZE))
DANGER_TILE.fill(WHITE)
pygame.draw.line(DANGER_TILE, RED, (0, 0), (DANGER_TILE.get_width(), DANGER_TILE.get_height()), 2)
pygame.draw.line(DANGER_TILE, RED, (0, DANGER_TILE.get_width() - 1), (DANGER_TILE.get_height() - 1, 0), 2)

SHIELD_TEXT = ARIAL_FONT_24.render('S', True, WHITE)
SHIELD_TILE = pygame.Surface((INNER_SIZE, INNER_SIZE))
SHIELD_TILE.fill(BLUE)
SHIELD_TILE.blit(SHIELD_TEXT, (
    SHIELD_TILE.get_width() // 2 - SHIELD_TEXT.get_width() // 2,
    SHIELD_TILE.get_height() // 2 - SHIELD_TEXT.get_height() // 2,
))

FIRST_TEAM_TEXT = ARIAL_FONT_24.render('A', True, GREEN)
SECOND_TEAM_TEXT = ARIAL_FONT_24.render('B', True, GREEN)


def _draw_map(sea_map: SeaMap, surface: pygame.Surface, first_pos=None, second_pos=None, treasure_value=None):
    surface.fill(BORDER_COLOR)

    for row in range(sea_map.M):
        for col in range(sea_map.N):
            if sea_map.is_danger(row, col):
                surface.blit(DANGER_TILE, (col * TILE_SIZE + BORDER_WIDTH, row * TILE_SIZE + BORDER_WIDTH))
            elif sea_map.is_shield(row, col):
                surface.blit(SHIELD_TILE, (col * TILE_SIZE + BORDER_WIDTH, row * TILE_SIZE + BORDER_WIDTH))
            else:
                # Empty or gold
                surface.blit(EMPTY_TILE, (col * TILE_SIZE + BORDER_WIDTH, row * TILE_SIZE + BORDER_WIDTH))

                x = sea_map.get(row, col)
                if x > 0:
                    text = ARIAL_FONT_24.render(str(x), True, BLACK)
                    surface.blit(text, (col * TILE_SIZE + TILE_SIZE // 2 - text.get_width() // 2,
                                        row * TILE_SIZE + TILE_SIZE // 2 - text.get_height() // 2))

    if treasure_value is not None:
        text = ARIAL_FONT_24.render(str(treasure_value), True, RED)
        row = sea_map.M // 2
        col = sea_map.N // 2
        surface.blit(text, (col * TILE_SIZE + TILE_SIZE // 2 - text.get_width() // 2,
                            row * TILE_SIZE + TILE_SIZE // 2 - text.get_height() // 2))

    if first_pos is not None:
        surface.blit(
            FIRST_TEAM_TEXT,
            (first_pos[1] * TILE_SIZE + TILE_SIZE // 2 - FIRST_TEAM_TEXT.get_width() // 2,
             first_pos[0] * TILE_SIZE + BORDER_WIDTH // 2),
        )

    if second_pos is not None:
        surface.blit(
            SECOND_TEAM_TEXT,
            (second_pos[1] * TILE_SIZE + TILE_SIZE // 2 - SECOND_TEAM_TEXT.get_width() // 2,
             second_pos[0] * TILE_SIZE + BORDER_WIDTH // 2),
        )


def visualize_map_to_image(sea_map: SeaMap, filename: str, first_pos=None, second_pos=None, treasure_value=None):
    # Create the map surface
    map_surface = pygame.Surface((TILE_SIZE * sea_map.N, TILE_SIZE * sea_map.M))

    # Draw the map
    _draw_map(sea_map, map_surface, first_pos, second_pos, treasure_value)

    # Save the map to an image file
    pygame.image.save(map_surface, filename)


def visualize_map_to_screen(sea_map: SeaMap, first_pos=None, second_pos=None, treasure_value=None):
    # Create the display surface
    screen = pygame.display.set_mode((TILE_SIZE * sea_map.N, TILE_SIZE * sea_map.M))

    # Main loop
    running = True
    while running:
        # Handle events
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        # Draw the map
        _draw_map(sea_map, screen, first_pos, second_pos, treasure_value)

        # Update the display
        pygame.display.flip()
