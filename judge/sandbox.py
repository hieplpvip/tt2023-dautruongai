import os
import platform
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
    if platform.system() != 'Darwin':
        # setrlimit does not work on macos
        resource.setrlimit(resource.RLIMIT_AS, (MEMORY_LIMIT, MEMORY_LIMIT))


class TrackedPopen(subprocess.Popen):
    """
    Modified subprocess.Popen to track resource usage
    """

    def _try_wait(self, wait_flags):
        # Modified to use wait4 instead of waitpid
        try:
            (pid, sts, rusage) = os.wait4(self.pid, wait_flags)
            self.rusage = rusage
        except ChildProcessError:
            pid = self.pid
            sts = 0
        return (pid, sts)


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
            if os.path.basename(file.name) != 'STATE.OUT':
                if file.is_file():
                    file.unlink()
                else:
                    shutil.rmtree(file)

        with open(Path(self._working_dir.name) / 'MAP.INP', 'w', encoding='utf8') as f:
            f.write(f'{sea_map.M} {sea_map.N} {remaining_turn}\n')
            f.write(f'{self_x} {self_y} {other_x} {other_y}\n')
            f.write(f'{gold} {shield}\n')
            for row in sea_map:
                f.write(' '.join(map(str, row)) + '\n')

        self._prepared = True

    def run(self):
        """
        Run agent and return move (1-indexed)
        """

        assert self._prepared, 'Must call prepare before running'
        self._prepared = False

        print_debug('Running', self._exe, 'in', self._working_dir.name)
        try:
            p = TrackedPopen(
                [self._exe],
                cwd=self._working_dir.name,
                preexec_fn=limit_memory,
            )
            p.communicate(timeout=TIME_LIMIT)

            time_usage = round(p.rusage.ru_utime + p.rusage.ru_stime, 5)
            memory_usage = p.rusage.ru_maxrss
            if platform.system() == 'Darwin':
                # macos reports memory usage in bytes
                # setrlimit does not work on macos, so we check for MLE here
                if memory_usage > MEMORY_LIMIT:
                    raise Exception('Exceeded memory limit')

                # convert to KB
                memory_usage //= 1024

            print_debug('Time:', time_usage, 's')
            print_debug('Memory:', memory_usage, 'KB')

            if p.returncode:
                print_important(self._exe, 'exited with non-zero code', p.returncode)
        except Exception as e:
            print_important(self._exe, 'run failed:', e)

        output_file = Path(self._working_dir.name) / 'MOVE.OUT'
        if not output_file.exists():
            print_important(self._exe, 'did not produce output file')
            return 0, 0

        try:
            with open(output_file, 'r', encoding='utf8') as f:
                tmp = list(map(int, f.readline().strip().split(' ')))
                if len(tmp) != 2:
                    return 0, 0

            return (tmp[0], tmp[1])
        except Exception as e:
            print_important(self._exe, 'produced invalid output file:', e)
            return 0, 0
