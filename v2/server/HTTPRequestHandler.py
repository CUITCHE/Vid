# usr/bin/env python3
# python version 3.6


from http.server import BaseHTTPRequestHandler
import json
import zlib


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
            if self.is_gzip:
                post_data = zlib.decompress(post_data)
            data = json.loads(post_data, encoding='utf-8')
            if self.path == 'diff':
                self.diff(data)
        except Exception as e:
            self.send_error(500)

    def diff(self, request_data: dict):
        """
        path=diff
        request body:
            {
                'status': int, # 文件的状态：new, moved, deleted, modified
                'path': str, # 相对路径
                'content': str,  # 文件内容，有可能会被BASE64编码
                'fileType': str, # 文件类型。'text', 'binary'
            }
        :param request_data:
        :return:
        """



