
from pygit2 import *
import sys


def all_files(root_path: str):
    if not root_path:
        sys.stderr.write('root path cannot be empty.')
        exit(7)
    if not root_path.endswith('/'):
        root_path += '/'
    repo = init_repository(root_path)
    tree = repo.revparse_single("HEAD^{tree}")
    files = list()
    _list_tree(tree, repo, root_path, files)
    return files


def _list_tree(tree, repo, name: str, files: list):
    for f in tree:
        if f.type == 'tree':
            child_tree = repo.git_object_lookup_prefix(f.oid)
            _list_tree(child_tree, repo, f'{name}{f.name}/', files)
        else:
            files.append(f'{name}{f.name}')



