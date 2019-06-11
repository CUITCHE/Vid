#!/usr/bin/env python3
# Author: ihejunqiu@icloud.com


import socketserver
import struct
import random
import os
import re
import vidserver.communication as communication
from enum import Enum


class FileChangeType(Enum):
    modified = 1
    add = 2
    removed = 3


def random_token(length):
    pool = 'abcdefghigklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
    return ''.join([random.choice(pool) for _ in range(length)])


password = ''  # random_token(8)
watchPath = ''
fileNameFilter = re.compile('.*(.cpp|.h|.swift|.cc|.hpp|.xml|.java|.js|.vue|.c)$')


class UnknownChangedFileTypeError(Exception):
    pass


class TCPHandler(socketserver.BaseRequestHandler):
    def __init__(self, request, client_address, server):
        self.is_login = True
        self.methods = {communication.Protocol.token_login: 'token_login',
                        communication.Protocol.directory_verification: 'directory_verification',
                        communication.Protocol.file_diff: 'file_diff'}
        super().__init__(request, client_address, server)

    def handle(self):

        try:
            while True:
                req = self.read_request()
                if not req:
                    print("非法数据格式或链接已断开")
                    break
                proto = req.proto
                method = self.methods.get(proto, '')
                if not method:
                    print(f"无法识别的协议号：{proto}")
                    break
                func = getattr(self, method, None)
                if func:
                    func(req)

        except Exception as e:
            print(e)

    def read_request(self):
        length_buffer = self.request.recv(4)
        if not length_buffer:
            return None
        length = struct.unpack('!i', length_buffer)
        buffer = self.request.recv(length[0])
        if length[0] != len(buffer):
            return None
        req = communication.request.Request()
        if not req.ParseFromString(buffer):
            return None
        return req

    def send_response(self, res: communication.request.Request):
        data = res.SerializeToString()
        length = struct.pack('!i', len(data))
        self.request.send(length + data)

    def token_login(self, req: communication.request.Request):
        res = communication.response.Response()
        res.id = req.id
        token_login = communication.request.TokenLogin()
        while True:
            if token_login.ParseFromString(req.body):
                if token_login.token == password:
                    res.code = 0
                    res.msg = 'success'
                    self.is_login = True
                    break
            self.is_login = False
            res.code = 403
            res.msg = '登入口令或数据包格式错误'
            break

        self.send_response(res)

    def directory_verification(self, req: communication.request.Request):
        res = communication.response.Response()
        res.id = req.id
        res.msg = 'success'
        while True:
            dv = communication.request.DirectoryVerification()

            if not dv.ParseFromString(req.body):
                res.code = 403
                res.msg = '数据包格式不对'
                break
            if os.path.split(watchPath)[1] != dv.root_name:
                res.code = 403
                res.msg = '监控的根目录名不一致'
                break
            all_files = list()
            list_all_files(watchPath, all_files)
            relative_all_files = list()
            for path in all_files:
                relative_all_files.append(path[len(watchPath) + 1:])
            server = set(relative_all_files)
            client = set(dv.contents.keys())
            msg = ''
            for path in (server - client):
                msg += f'{path}: 远端存在的文件，本端不存在\n'
            for path in (client - server):
                msg += f'{path}: 本端存在的文件，远端不存在\n'
            if msg:
                res.code = 4
                res.msg = msg
                break
            hash_files = file_hash(all_files)
            differences = list()
            for key, value in hash_files.items():
                client_iter = dv.contents.get(key, None)
                if client_iter != value:
                    differences.append(f'[{key}] 两端文件hash值不一致')
            if len(differences):
                res.code = 5
                res.msg = '\n'.join(differences)
            break

        self.send_response(res)

    def file_diff(self, req: communication.request.Request):
        print("diff")
        res = communication.response.Response()
        res.id = req.id
        res.msg = 'success'
        while True:
            fdiff = communication.request.FileDiff()
            if not fdiff.ParseFromString(req.body):
                res.code = 403
                res.msg = '数据包格式不对'
                break
            path = watchPath
            if not path.endswith('/'):
                path += '/'
            path += fdiff.relative_path
            status = FileChangeType(fdiff.status)
            if status == FileChangeType.removed:
                if not os.path.exists(path):
                    break
                try:
                    os.remove(path)
                except Exception as e:
                    print(e)
                    res.code = 404
                    res.msg = f'{path}: remove failed.'
                    break
            elif status == FileChangeType.add:
                print(f'raw path: {path}')
                parent_dir = os.path.split(path)[0]
                print(f'parent dir: {parent_dir}')
                if not os.path.exists(parent_dir):
                    os.makedirs(parent_dir, exist_ok=True)
                with open(path, 'w') as f:
                    f.write(fdiff.content)
            elif status == FileChangeType.modified:
                with open(path, 'w') as f:
                    f.write(fdiff.content)
            else:
                raise UnknownChangedFileTypeError()
            break
        self.send_response(res)


def run_server(**kwargs):
    host = kwargs['host']
    if not host:
        host = 'localhost'
    port = kwargs['port']
    if not port:
        port = 9000

    global watchPath
    watchPath = kwargs['path']
    if not watchPath:
        print('监控目录不能为空')
        exit(-1)
    if not os.path.exists(watchPath):
        print(f'\033[1;31m{watchPath} 不存在\033[0m')
        exit(-1)
    print(f"Listen on {port}. (Token={password}, WatchPath={watchPath})")
    server = socketserver.TCPServer((host, port), TCPHandler)
    try:
        server.serve_forever()
    except Exception as e:
        print(e)
    finally:
        print('\n\033[1;31mserver closed.\033[0m')
        server.shutdown()
        server.server_close()


def list_all_files(path, all_files: list):
    files = os.listdir(path)
    for file in files:
        new_path = os.path.join(path, file)
        if os.path.isfile(new_path):
            if fileNameFilter.match(new_path):
                all_files.append(new_path)
        elif os.path.isdir(new_path):
            list_all_files(new_path, all_files)


def file_hash(paths):
    import hashlib
    hashes = {}
    for path in paths:
        m = hashlib.sha256()
        with open(path, 'rb') as f:
            m.update(f.read())
            hashes[path[len(watchPath) + 1:]] = (m.hexdigest())
    return hashes
