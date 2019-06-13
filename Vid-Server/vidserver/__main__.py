
import vidserver
import argparse


def main():
    parse = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parse.add_argument('-p', '--port', help='监听的端口号', type=int, default=9010)
    parse.add_argument('-P', '--path', help='监听的文件目录路径')
    parse.add_argument('-lp', '--log_path', help='日志输出路径', required=False)
    parse.add_argument('-f', '--filter', help='需要过滤哪些文件，例如：--filter c cpp h，只会分析后缀名为c、cpp、h的文件',
                       nargs='+', default=['c', 'cc', 'cpp', 'h', 'hpp', 'java', 'swift', 'py', 'xml', 'js', 'vue'])
    args = parse.parse_args()
    print(args)
    vidserver.run_server(host='localhost', port=args.port, path=args.path, filter=args.filter)


main()
