import pygame
from visualization.constant import REGIONS, SEA
pygame.init()

AGENT = (198, 50, 135)
GOLD = (250, 211, 82)

MOUNTAIN = (0, 0, 0)
PRISON = (0, 0, 128)

SCAN = (255, 255, 255)


class layout:
    def __init__(self, row, col, width, height, region):
        self.row = row
        self.col = col
        self.x = col * width + width
        self.y = row * height + height
        self.width = width
        self.height = height
        self.region = None
        self.entity = None
        self.is_scan = False
        self.set_region(region)

    def set_region(self, region):  # decide what region of the grid
        entity = region[-1]
        if entity == 'M' or entity == 'P':
            self.region = int(region[:-1])
            self.entity = entity
        else:
            self.region = int(region)

    def get_pos(self):
        return (self.row, self.col)

    def is_barrier(self):
        if self.is_sea() or self.entity in ['M']:
            return True
        return False

    def is_mountain(self):
        if self.entity == 'M':
            return True
        return False

    def is_sea(self):
        if self.region == 0:
            return True
        return False

    def make_scan(self):
        self.is_scan = True

    def draw(self, win, THEME):
        region_color = REGIONS[self.region]
        if self.is_scan:
            region_color = SCAN
        pygame.draw.rect(win, region_color,
                         (self.x, self.y, self.width, self.height))

        entity_color = (0, 0, 0)
        if self.entity == 'T':
            entity_color = region_color  # GOLD
        elif self.entity == 'P':
            entity_color = PRISON
        elif self.entity == 'M':
            entity_color = MOUNTAIN

        text = THEME.render(self.entity, True, entity_color)
        center_rect = (self.x + self.width/2 - text.get_rect().width/2,
                       self.y + self.height/2 - text.get_rect().height/2)
        win.blit(text, center_rect)

    def __lt__(self, other):
        return False
