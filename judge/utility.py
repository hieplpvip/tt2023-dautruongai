from colorama import Fore, Style


def reset_color():
    print(Style.RESET_ALL, end='', flush=True)


def print_success(*args, **kwargs):
    print(Fore.GREEN, end='')
    print(*args, **kwargs)
    reset_color()


def print_info(*args, **kwargs):
    print(Fore.BLUE, end='')
    print(*args, **kwargs)
    reset_color()


def print_debug(*args, **kwargs):
    print(Fore.YELLOW, end='')
    print(*args, **kwargs)
    reset_color()


def print_error(*args, **kwargs):
    print(Fore.RED, end='')
    print(*args, **kwargs)
    reset_color()
