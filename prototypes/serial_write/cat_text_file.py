#!/usr/bin/env python

from time import sleep
from sys import argv
from functools import partial

with open(argv[1],'r') as f:
    for c in iter(partial(f.read, 1), ''):
        print(c)
        sleep(0.01)
