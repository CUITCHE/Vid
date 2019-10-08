

from enum import Enum


class FileChangeType(Enum):
    modified = 1
    add = 2
    deleted = 3
    moved = 4
