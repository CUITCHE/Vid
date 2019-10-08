
import vidserver
from vidserver.settings import Settings
import fire


def main(conf_path):
    setting = Settings(conf_path)

    vidserver.run_server(host='localhost', port=setting.port, path=setting.watch_path, log_path=setting.log_path)


fire.Fire(main)
