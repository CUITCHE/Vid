# usr/bin/env python3
# python version 3.6

from http import server
import ssl
import os
from http.server import BaseHTTPRequestHandler
from communication import *
import shutil


httpd = None  # type: HTTPS


class HTTPS(object):
    def __init__(self, port: int, work_dir: str, certfile: str, keyfile: str):
        self.work_dir = work_dir
        self._ssl = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        # openssl req -new -x509 -days 3650 -nodes -out mycertfile.pem -keyout mykeyfile.pem
        self._ssl.load_cert_chain(certfile, keyfile)
        self.httpd = server.HTTPServer(('localhost', port), HTTPRequest)
        self.httpd.socket = self._ssl.wrap_socket(self.httpd.socket, server_side=True)

    def run(self):
        print('Starting httpd...')
        try:
            self.httpd.serve_forever()
        except KeyboardInterrupt:
            pass
        self.httpd.server_close()
        print('Stopping httpd...')


def start_server(port: int, work_dir: str, certfile: str, keyfile: str):
    assert os.path.exists(work_dir), f'{work_dir} is not exists.'
    global httpd

    httpd = HTTPS(port, work_dir, certfile, keyfile)
    httpd.run()


class FilePathError(Exception):
    pass


def _complete_path(path: str):
    if '..' in path or path.startswith('~'):
        raise FilePathError(f'Relative-path is forbidden, path={path}')
    return f'{httpd.work_dir}/{path}'


def _check_path(filepath: str):
    if not os.path.exists(filepath):
        parent_dir = os.path.split(filepath)[0]
        os.makedirs(parent_dir, exist_ok=True)


class HTTPRequest(BaseHTTPRequestHandler):
    @property
    def is_gzip(self):
        h = self.headers['Content-Encoding']
        if h:
            return 'gzip' in h
        return False

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])  # Gets the size of data
        post_data = self.rfile.read(content_length)  # Gets the data itself
        try:
            if self.path == '/monitor/created':
                msg = CreatedMsg()
                method = getattr(self, 'on_created')
            elif self.path == '/monitor/modified':
                msg = ModifiedMsg()
                method = getattr(self, 'on_modified')
            elif self.path == '/monitor/moved':
                msg = MovedMsg()
                method = getattr(self, 'on_moved')
            elif self.path == '/monitor/deleted':
                msg = DeleteMsg()
                method = getattr(self, 'on_deleted')
            else:
                msg = None
                method = None
            if msg:
                msg.ParseFromString(post_data)
                method(msg)
            else:
                self.send_error(404, f'Unknown request path: {self.path}')
            self.send_header('Content-type', 'application/json')
            self.end_headers()
        except Exception as e:
            print(e)
            self.send_error(500, explain=str(e))

    def on_created(self, msg):
        path = _complete_path(msg.path)
        if msg.is_dir:
            os.makedirs(path, exist_ok=True)
        else:
            _check_path(path)
            with open(path, 'wb') as f:
                f.write(msg.content)
        self.send_response(200)

    def on_modified(self, msg):
        path = _complete_path(msg.path)
        _check_path(path)
        with open(path, 'wb') as f:
            f.write(msg.content)
        self.send_response(200)

    def on_moved(self, msg):
        src = _complete_path(msg.src)
        if not os.path.exists(src):
            self.send_response(200)
            return  # 可能之前就已经移动了
        if msg.is_dir:
            os.removedirs(src)
            self.send_response(200)
            return
        dst = _complete_path(msg.dst)

        with open(src, 'rb') as f:
            content = f.read()
        _check_path(dst)
        with open(dst, 'wb') as f:
            f.write(content)
        os.remove(src)
        self.send_response(200)

    def on_deleted(self, msg):
        path = _complete_path(msg.path)
        if os.path.exists(path):
            if os.path.isdir(path):
                shutil.rmtree(path)
            else:
                os.remove(path)
        self.send_response(200)

