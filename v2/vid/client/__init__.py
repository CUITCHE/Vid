# encoding: utf-8

import sys


if sys.version_info < (3, 6):
    print('\033[22;31mError：请使用Python3.6或更高版本\033[0m')
    exit(49)

from client.monitor import start as start_main

__all__ = ['start']


def start(host: str, port: int, work_dir: str, certfile: str = None):
    start_main(host, port, work_dir, certfile)
