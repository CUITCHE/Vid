
import vidserver
import argparse


def main():
    parse = argparse.ArgumentParser()
    parse.add_argument('-p', '--port', help='监听的端口号', type=int)
    parse.add_argument('-P', '--path', help='监听的文件目录路径')
    parse.add_argument('-lp', '--log_path', help='日志输出路径', required=False)
    args = parse.parse_args()
    print(args)
    vidserver.run_server(host='localhost', port=args.port, path=args.path)


main()
