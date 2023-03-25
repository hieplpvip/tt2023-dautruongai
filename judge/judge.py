from pathlib import Path

from match import Match

if __name__ == '__main__':
    match = Match('../map/sample.txt', Path('../algo/hiep1').absolute(), Path('../algo/hiep2').absolute())
    print(match.next_turn())
