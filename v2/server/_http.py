# usr/bin/env python3
# python version 3.6

from http import server
import ssl
import logging
from server import HTTPRequestHandler


httpd = None  # type: HTTPS


class HTTPS(object):
    def __init__(self, port, work_dir):
        self.work_dir = work_dir
        self._ssl = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        self._ssl.load_cert_chain('', '')
        self.httpd = server.HTTPServer(('localhost', port), HTTPRequestHandler)
        self.httpd.socket = self._ssl.wrap_socket(self.httpd.socket, server_side=True)

    def run(self):
        logging.info('Starting httpd...\n')
        try:
            self.httpd.serve_forever()
        except KeyboardInterrupt:
            pass
        self.httpd.server_close()
        logging.info('Stopping httpd...\n')


def start_server(port, work_dir):
    global httpd

    httpd = HTTPS(port, work_dir)
    httpd.run()
