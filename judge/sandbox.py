import resource
import shutil
import subprocess
import tempfile
from pathlib import Path

from sea_map import SeaMap
from utility import *

TIME_LIMIT = 2
MEMORY_LIMIT = 512 * 1024 * 1024  # 512 MB


def limit_memory():
    try:
        resource.setrlimit(resource.RLIMIT_AS, (MEMORY_LIMIT, MEMORY_LIMIT))
    except ValueError as e:
        print_error(e)
        pass


class SandBox:
    """
    Prepare a sandbox for executing agent.

    A temporary directory is created upon initialization.
    This directory is used for all runs.
    After each run, all files except STATE.OUT are deleted.
    The content of MOVE.OUT is returned to Match.

    This sandbox also limits the memory and time usage of the agent.
    However, it is the only limitation in place, which means the agent
    can still do anything it wants to the system (including creating files
    outside working directory). Since we only run our own agents, this
    should not be a problem.
    """

    def __init__(self, exe):
        self._exe = exe
        self._working_dir = tempfile.TemporaryDirectory()
        self._prepared = False

    def __del__(self):
        self._working_dir.cleanup()

    def prepare(
        self,
        self_x: int,
        self_y: int,
        other_x: int,
        other_y: int,
        shield: int,
        gold: int,
        remaining_turn: int,
        sea_map: SeaMap,
    ):
        """
        Prepare directory for running agent:
        - Delete all files except STATE.OUT
        - Create MAP.INP
        """
        for file in Path(self._working_dir.name).iterdir():
            if file.name != 'STATE.OUT':
                if file.is_file():
                    file.unlink()
                else:
                    shutil.rmtree(file)

        with open(Path(self._working_dir.name) / 'MAP.INP', 'w', encoding='utf8') as f:
            f.write(f'{sea_map.M} {sea_map.N} {remaining_turn}\n')
            f.write(f'{self_x} {self_y} {other_x} {other_y}\n')
            f.write(f'{shield} {gold}\n')
            for row in sea_map:
                f.write(' '.join(map(str, row)) + '\n')

        self._prepared = True

    def run(self):
        assert self._prepared, 'Must call prepare before running'
        self._prepared = False

        try:
            subprocess.run(
                [self._exe],
                cwd=self._working_dir.name,
                timeout=TIME_LIMIT,
                preexec_fn=limit_memory,
                check=True,
            )

            output_file = Path(self._working_dir.name) / 'MOVE.OUT'
            if not output_file.exists():
                return -1, -1

            with open(output_file, 'r', encoding='utf8') as f:
                tmp = list(map(int, f.readline().strip().split(' ')))
                if len(tmp) != 2:
                    return -1, -1

            return (tmp[0], tmp[1])
        except subprocess.SubprocessError as e:
            print_error(self._exe, 'failed:', e)
            return -1, -1
