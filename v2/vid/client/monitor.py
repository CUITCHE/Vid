# Author: hejunqiu

from watchdog.observers import Observer
from watchdog.events import *
from http import client
from communication import *
import time
import os
import ssl
import requests


file_map = {True: 'directory', False: 'file'}


def _check_dot_files(filepath: str):
    name = os.path.split(filepath)[1]  # 暂时只忽略这一个文件，后续可添加工具支持.gitignore文件
    if name == '.DS_Store':
        return True
    return False


class FileEventHandler(FileSystemEventHandler):
    def __init__(self, host: str, port: int, work_dir: str, certfile: str):
        super(FileSystemEventHandler, self).__init__()
        self._host = host
        self._port = port
        self._work_dir = work_dir
        self.cert_file = certfile
        # _ssl = ssl._create_unverified_context(certfile=certfile,
        #                                       purpose=ssl.Purpose.CLIENT_AUTH)
        # self._ssl = _ssl

    @property
    def path_offset(self):
        return len(self._work_dir)

    def on_moved(self, event: FileMovedEvent):
        print(f'{file_map[event.is_directory]} moved from {event.src_path} to {event.dest_path}')
        msg = MovedMsg()
        msg.src = self._get_relative_path(event.src_path)
        msg.dst = self._get_relative_path(event.dest_path)
        msg.is_dir = event.is_directory
        self._send('monitor/moved', msg.SerializeToString())

    def on_created(self, event: FileCreatedEvent):
        print(f'{file_map[event.is_directory]} created: {event.src_path}')
        if _check_dot_files(event.src_path):
            return
        if not os.path.exists(event.src_path):
            raise FileNotFoundError(f'{event.src_path}: Not Found!')
        msg = CreatedMsg()
        msg.path = self._get_relative_path(event.src_path)
        msg.is_dir = event.is_directory

        if not event.is_directory:
            with open(event.src_path, 'rb') as f:
                msg.content = f.read()

        self._send('monitor/created', msg.SerializeToString())

    def on_deleted(self, event: FileDeletedEvent):
        print(f'{file_map[event.is_directory]} deleted: {event.src_path}')
        if _check_dot_files(event.src_path):
            return
        msg = DeleteMsg()
        msg.path = self._get_relative_path(event.src_path)
        msg.is_dir = event.is_directory
        self._send('monitor/deleted', msg.SerializeToString())  # 注意有可能是dir删除

    def on_modified(self, event: FileModifiedEvent):
        print(f'{file_map[event.is_directory]} modified: {event.src_path}')
        if _check_dot_files(event.src_path):
            return
        if event.is_directory:
            return
        msg = ModifiedMsg()
        msg.path = self._get_relative_path(event.src_path)
        with open(event.src_path, 'rb') as f:
            msg.content = f.read()
        self._send('monitor/modified', msg.SerializeToString())

    def _send(self, path: str, body: bytes):
        # conn = client.HTTPSConnection(self._host, self._port)
        # conn.connect()
        # sock = self._ssl.wrap_socket(conn.sock)
        # conn.sock = sock
        # conn.request(method='POST', url=path, body=body)
        # print(conn.getresponse())
        # conn.close()
        response = requests.post(f'{self._host}:{self._port}/{path}', bytes(body), verify=self.cert_file)
        print(response, response.content)

    def _get_relative_path(self, path):
        return path[self.path_offset + 1:]


def start(host: str, port: int, work_dir: str, certfile: str):
    assert os.path.exists(work_dir), f'{work_dir} is not exists.'

    observer = Observer()
    if not host.startswith('https://'):
        host = 'https://' + host
    event_handler = FileEventHandler(host, port, work_dir, certfile)
    observer.schedule(event_handler, work_dir, True)
    print(f'Monitor dir: {work_dir}')
    try:
        observer.start()
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
