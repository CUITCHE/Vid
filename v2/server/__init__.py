# encoding: utf-8

import sys


if sys.version_info < (3, 6):
    print('\033[22;31mError：请使用Python3.6或更高版本\033[0m')
    exit(49)

from server._http import start_server

__all__ = ['start_server']
