
import vidserver
import argparse
from vidserver.git import all_files


def main():
    print(all_files('/Users/hejunqiu/Documents/QtProjects/Vid3'))
    exit(0)
    parse = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parse.add_argument('-p', '--port', help='监听的端口号', type=int, default=9010)
    parse.add_argument('-P', '--path', help='监听的文件目录路径')
    parse.add_argument('-lp', '--log-path', help='日志输出路径', required=False)
    parse.add_argument('--git-dir', help='.git目录', default='.')
    parse.add_argument('--work-tree', help='工作目录，默认与path一致', default='.')

    args = parse.parse_args()
    print(args)
    vidserver.run_server(host='localhost', port=args.port, path=args.path, git_dir=args.git_dir,
                         work_tree=args.work_tree)


main()
