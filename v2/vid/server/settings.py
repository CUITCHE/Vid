
import configparser


class Settings(object):
    def __init__(self, conf_path):
        self.cf = configparser.ConfigParser()
        self.cf.read(conf_path)

    @property
    def port(self):
        return self.cf.getint('server', 'port', fallback=9010)

    @property
    def watch_path(self):
        return self.cf.get('server', 'watchPath', fallback='/tmp')

    @property
    def log_path(self):
        return self.cf.get('server', 'logPath', fallback=None)
