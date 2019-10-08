

import socket
import struct
from communication import request
from communication import Protocol
import zipfile


INITIALIZE = 0
_socket = None  # type: socket.socket
_query_id = INITIALIZE


def _get_query_id():
    global _query_id
    _query_id += 1
    return _query_id


def init_socket(**kwargs):
    global _socket
    if _socket:
        _socket.close()
    _socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    _socket.connect((kwargs['host'], kwargs['port']))


def send_request(req: request.Request):
    data = req.SerializeToString()
    length = struct.pack('!i', len(data))
    _socket.send(length + data)


def send_heartbeat():
    req = request.Request()
    req.id = _get_query_id()
    req.proto = Protocol.heart_beat
    send_request(req)


def on_modified(src_path: str, master: str):
    body = request.FileDiff()
    relative_path = src_path[len(master):]
    body.relative_path = relative_path



