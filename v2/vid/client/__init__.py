

from client.monitor import start as start_main

__all__ = ['monitor']


def start(path: str):
    start_main(path=path)
