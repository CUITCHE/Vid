#!/usr/bin/env python3


class IgnoreControl(object):
    def __init__(self, *args):
        self.ignore_patterns = args  # from .gitignore

    def is_ignore(self, ):
