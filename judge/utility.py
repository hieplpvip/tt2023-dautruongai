from colorama import Fore, Style

__all__ = [
    'enable_all_logging',
    'disable_all_logging',
    'enable_logging',
    'disable_logging',
    'enable_color',
    'disable_color',
    'reset_color',
    'print_header',
    'print_info',
    'print_debug',
    'print_important',
]

ENABLED_LEVEL = {
    'header': True,
    'info': True,
    'debug': True,
    'important': True,
}

ENABLED_COLOR = True


def enable_all_logging():
    for level in ENABLED_LEVEL:
        ENABLED_LEVEL[level] = True


def disable_all_logging():
    for level in ENABLED_LEVEL:
        ENABLED_LEVEL[level] = False


def enable_logging(level):
    assert level in ENABLED_LEVEL, 'Invalid logging level'
    ENABLED_LEVEL[level] = True


def disable_logging(level):
    assert level in ENABLED_LEVEL, 'Invalid logging level'
    ENABLED_LEVEL[level] = False


def enable_color():
    global ENABLED_COLOR
    ENABLED_COLOR = True


def disable_color():
    global ENABLED_COLOR
    ENABLED_COLOR = False


def reset_color():
    print(Style.RESET_ALL, end='', flush=True)


def print_header(*args, **kwargs):
    if not ENABLED_LEVEL['header']:
        return

    if not ENABLED_COLOR:
        print(*args, **kwargs)
        return

    print(Fore.GREEN, end='')
    print(*args, **kwargs)
    reset_color()


def print_info(*args, **kwargs):
    if not ENABLED_LEVEL['info']:
        return

    if not ENABLED_COLOR:
        print(*args, **kwargs)
        return

    print(Fore.BLUE, end='')
    print(*args, **kwargs)
    reset_color()


def print_debug(*args, **kwargs):
    if not ENABLED_LEVEL['debug']:
        return

    if not ENABLED_COLOR:
        print(*args, **kwargs)
        return

    print(Fore.YELLOW, end='')
    print(*args, **kwargs)
    reset_color()


def print_important(*args, **kwargs):
    if not ENABLED_LEVEL['important']:
        return

    if not ENABLED_COLOR:
        print(*args, **kwargs)
        return

    print(Fore.RED, end='')
    print(*args, **kwargs)
    reset_color()
