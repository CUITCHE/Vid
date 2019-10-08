#!/usr/bin/env python3
# Author: ihejunqiu@icloud.com


import socketserver
import struct
import random
import os
import communication as communication
from communication.filestatus import FileChangeType
import logging


def random_token(length):
    pool = 'abcdefghigklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
    return ''.join([random.choice(pool) for _ in range(length)])


password = ''  # random_token(8)
watchPath = ''  # 监控路径，由用户提供


class UnknownChangedFileTypeError(Exception):
    pass


class ProtocolLengthError(Exception):
    pass


class ProtocolReceiveError(Exception):
    pass


class TCPHandler(socketserver.BaseRequestHandler):
    def __init__(self, request, client_address, server):
        self.is_login = True
        self.methods = {communication.Protocol.token_login: 'token_login',
                        communication.Protocol.directory_verification: 'directory_verification',
                        communication.Protocol.file_diff: 'file_diff'}
        super().__init__(request, client_address, server)

    def handle(self):
        logger = logging.getLogger("TCPHandler.handle")
        while True:
            try:
                req = self.read_request()
                proto = req.proto
                method = self.methods.get(proto, '')
                if not method:
                    raise Exception(f"无法识别的协议号：{proto}")
                func = getattr(self, method, None)
                if func:
                    func(req)
            except Exception as e:
                logger.error("%s", e)
                break

    def read_request(self):
        length_buffer = self.request.recv(4)
        if not length_buffer:
            raise Exception("未能正确获取协议长度")
        length = struct.unpack('!i', length_buffer)
        buffer = self.request.recv(length[0])
        if length[0] != len(buffer):
            raise Exception(f"Excepted {length[0]} data, but received {len(buffer)}.")
        req = communication.request.Request()
        if not req.ParseFromString(buffer):
            raise Exception("未能从data中实例化Request")
        return req

    def send_response(self, res: communication.request.Request):
        data = res.SerializeToString()
        length = struct.pack('!i', len(data))
        self.request.send(length + data)

    def token_login(self, req: communication.request.Request):
        res = communication.response.Response()
        res.id = req.id
        token_login = communication.request.TokenLogin()
        try:
            if token_login.ParseFromString(req.body):
                if token_login.token == password:
                    res.code = 0
                    res.msg = 'success'
                    self.is_login = True
                    return
            self.is_login = False
            res.code = 403
            res.msg = '登入口令或数据包格式错误'
            return
        finally:
            self.send_response(res)

    def directory_verification(self, req: communication.request.Request):
        res = communication.response.Response()
        res.id = req.id
        res.msg = 'success'
        try:
            dv = communication.request.DirectoryVerification()

            if not dv.ParseFromString(req.body):
                res.code = 403
                res.msg = '数据包格式不对'
                return
            if os.path.split(watchPath)[1] != dv.root_name:
                res.code = 403
                res.msg = '监控的根目录名不一致'
                return
            all_files = list_all_files(watchPath)
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
                return
            hash_files = file_hash(all_files)
            differences = list()
            for key, value in hash_files.items():
                client_iter = dv.contents.get(key, None)
                if client_iter != value:
                    differences.append(f'[{key}] 两端文件hash值不一致')
            if len(differences):
                res.code = 5
                res.msg = '\n'.join(differences)
            return
        finally:
            self.send_response(res)

    def file_diff(self, req: communication.request.Request):
        logger = logging.getLogger("TCPHandler.file_diff")
        res = communication.response.Response()
        res.id = req.id
        res.msg = 'success'
        try:
            fdiff = communication.request.FileDiff()
            if not fdiff.ParseFromString(req.body):
                res.code = 403
                res.msg = '数据包格式不对'
                return
            path = watchPath
            if not path.endswith('/'):
                path += '/'
            path += fdiff.relative_path
            status = FileChangeType(fdiff.status)
            if status == FileChangeType.deleted:
                if not os.path.exists(path):
                    return
                try:
                    os.remove(path)
                except Exception as e:
                    logger.warning("%s", e)
                    res.code = 404
                    res.msg = f'{path}: remove failed.'
                    return
            elif status == FileChangeType.add:
                parent_dir = os.path.split(path)[0]
                if not os.path.exists(parent_dir):
                    os.makedirs(parent_dir, exist_ok=True)
                with open(path, 'w') as f:
                    f.write(fdiff.content)
            elif status == FileChangeType.modified:
                with open(path, 'w') as f:
                    f.write(fdiff.content)
            else:
                raise UnknownChangedFileTypeError()
            logger.info(f"更新文件: {path}, 操作={status}")
            return
        finally:
            self.send_response(res)


def run_server(**kwargs):
    log_path = None
    if kwargs['log_path']:
        log_path = kwargs['log_path']
    logging.basicConfig(level=logging.INFO,
                        format='[%(asctime)s %(name)s %(levelname)s] %(message)s',
                        filename=log_path,
                        filemode='a+')

    logger = logging.getLogger("run_server")

    host = kwargs['host']
    port = kwargs['port']

    global watchPath

    watchPath = os.path.abspath(kwargs['path'])

    if not watchPath:
        logger.error('监控目录不能为空')
        exit(-1)
    if not os.path.exists(watchPath):
        logger.error(f'{watchPath} not exists.')
        exit(-2)

    logger.info(f"Listen on {port}. (Token={password}, WatchPath={watchPath})")
    server = socketserver.TCPServer((host, port), TCPHandler)

    try:
        server.serve_forever()
    except Exception as e:
        logger.error("%s", e)
    finally:
        logger.info('\n\033[1;31mserver closed.\033[0m')
        server.shutdown()
        server.server_close()


def list_all_files(path):
    from server.git import all_files
    return all_files(path)


def file_hash(paths):
    import hashlib
    hashes = {}
    for path in paths:
        m = hashlib.sha256()
        with open(path, 'rb') as f:
            m.update(f.read())
            hashes[path[len(watchPath) + 1:]] = (m.hexdigest())
    return hashes





