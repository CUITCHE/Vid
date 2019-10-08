# Author: hejunqiu

from watchdog.observers import Observer
from watchdog.events import *
import time


def noop(*args, **kwargs):
    _, _ = args, kwargs


ON_MOVED = noop
ON_CREATED = noop
ON_DELETED = noop
ON_MODIFIED = noop

file_map = {True: 'directory', False: 'file'}


class FileEventHandler(FileSystemEventHandler):
    def __init__(self):
        FileSystemEventHandler.__init__(self)

    def on_moved(self, event: FileMovedEvent):
        print(f'{file_map[event.is_directory]} moved from {event.src_path} to {event.dest_path}')
        ON_MOVED(src=event.src_path, dest=event.dest_path)

    def on_created(self, event: FileCreatedEvent):
        print(f'{file_map[event.is_directory]} created: {event.src_path}')
        ON_CREATED(src=event.src_path)

    def on_deleted(self, event: FileDeletedEvent):
        print(f'{file_map[event.is_directory]} deleted: {event.src_path}')
        ON_DELETED(src=event.src_path)

    def on_modified(self, event: FileModifiedEvent):
        print(f'{file_map[event.is_directory]} modified: {event.src_path}')
        ON_MODIFIED(src=event.src_path)


def start(path: str):
    observer = Observer()
    event_handler = FileEventHandler()
    observer.schedule(event_handler, path, True)
    observer.start()
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
