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
MARGIN_SIZE = 150
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


def _draw_map(
    sea_map: SeaMap,
    surface: pygame.Surface,
    header_str=None,
    first_state=None,
    second_state=None,
    treasure_value=None,
):
    surface.fill(BORDER_COLOR)
    surface.fill(WHITE, (0, 0, TILE_SIZE * sea_map.N, MARGIN_SIZE))

    for row in range(sea_map.M):
        for col in range(sea_map.N):
            if sea_map.is_danger(row, col):
                surface.blit(
                    DANGER_TILE,
                    (
                        col * TILE_SIZE + BORDER_WIDTH,
                        MARGIN_SIZE + row * TILE_SIZE + BORDER_WIDTH,
                    ),
                )
            elif sea_map.is_shield(row, col):
                surface.blit(
                    SHIELD_TILE,
                    (
                        col * TILE_SIZE + BORDER_WIDTH,
                        MARGIN_SIZE + row * TILE_SIZE + BORDER_WIDTH,
                    ),
                )
            else:
                # Empty or gold
                surface.blit(
                    EMPTY_TILE,
                    (
                        col * TILE_SIZE + BORDER_WIDTH,
                        MARGIN_SIZE + row * TILE_SIZE + BORDER_WIDTH,
                    ),
                )

                x = sea_map.get(row, col)
                if x > 0:
                    text = ARIAL_FONT_24.render(str(x), True, BLACK)
                    surface.blit(
                        text,
                        (
                            col * TILE_SIZE + TILE_SIZE // 2 - text.get_width() // 2,
                            MARGIN_SIZE + row * TILE_SIZE + TILE_SIZE // 2 - text.get_height() // 2,
                        ),
                    )

    if treasure_value is not None:
        text = ARIAL_FONT_24.render(str(treasure_value), True, RED)
        row = sea_map.M // 2
        col = sea_map.N // 2
        surface.blit(
            text,
            (
                col * TILE_SIZE + TILE_SIZE // 2 - text.get_width() // 2,
                MARGIN_SIZE + row * TILE_SIZE + TILE_SIZE // 2 - text.get_height() // 2,
            ),
        )

    header_next_y = BORDER_WIDTH
    if header_str is not None:
        text = ARIAL_FONT_24.render(header_str, True, RED)
        surface.blit(
            text,
            (BORDER_WIDTH, header_next_y),
        )
        header_next_y += text.get_height() + BORDER_WIDTH

    if first_state is not None:
        row, col = first_state['position']
        surface.blit(
            FIRST_TEAM_TEXT,
            (
                col * TILE_SIZE + TILE_SIZE // 2 - FIRST_TEAM_TEXT.get_width() // 2,
                MARGIN_SIZE + row * TILE_SIZE + BORDER_WIDTH // 2,
            ),
        )

        text = f'A: at ({row + 1}, {col + 1})'
        if first_state['eliminated']:
            text += ' eliminated,'
        else:
            text += ' alive,'
        if first_state['shield']:
            text += ' have shield,'
        else:
            text += ' no shield,'
        text += f' {first_state["gold"]} gold'

        text = ARIAL_FONT_24.render(text, True, RED)
        surface.blit(
            text,
            (BORDER_WIDTH, header_next_y),
        )
        header_next_y += text.get_height() + BORDER_WIDTH

    if second_state is not None:
        row, col = second_state['position']
        surface.blit(
            SECOND_TEAM_TEXT,
            (
                col * TILE_SIZE + TILE_SIZE // 2 - FIRST_TEAM_TEXT.get_width() // 2,
                MARGIN_SIZE + row * TILE_SIZE + BORDER_WIDTH // 2,
            ),
        )

        text = f'B: at ({row + 1}, {col + 1})'
        if second_state['eliminated']:
            text += ' eliminated,'
        else:
            text += ' alive,'
        if second_state['shield']:
            text += ' have shield,'
        else:
            text += ' no shield,'
        text += f' {second_state["gold"]} gold'

        text = ARIAL_FONT_24.render(text, True, RED)
        surface.blit(
            text,
            (BORDER_WIDTH, header_next_y),
        )
        header_next_y += text.get_height() + BORDER_WIDTH


def visualize_map_to_image(
    sea_map: SeaMap,
    filename: str,
    header_str=None,
    first_state=None,
    second_state=None,
    treasure_value=None,
):
    # Create the map surface
    map_surface = pygame.Surface((TILE_SIZE * sea_map.N, MARGIN_SIZE + TILE_SIZE * sea_map.M))

    # Draw the map
    _draw_map(
        sea_map,
        map_surface,
        header_str=header_str,
        first_state=first_state,
        second_state=second_state,
        treasure_value=treasure_value,
    )
    # Save the map to an image file
    pygame.image.save(map_surface, filename)


def visualize_map_to_screen(
    sea_map: SeaMap,
    header_str=None,
    first_state=None,
    second_state=None,
    treasure_value=None,
):
    # Create the display surface
    screen = pygame.display.set_mode((TILE_SIZE * sea_map.N, MARGIN_SIZE + TILE_SIZE * sea_map.M))

    # Main loop
    running = True
    while running:
        # Handle events
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        # Draw the map
        _draw_map(
            sea_map,
            screen,
            header_str=header_str,
            first_state=first_state,
            second_state=second_state,
            treasure_value=treasure_value,
        )

        # Update the display
        pygame.display.flip()
