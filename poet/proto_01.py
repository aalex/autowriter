#!/usr/bin/env python
"""
One must do this first::

  import nltk
  nltk.download()

"""
#from __future__ import division
import nltk
#import re
#import pprint

FILE_NAME = "source_pg43038.txt"

def hello(length=100):
    raw = open(FILE_NAME, "rU").read()
    tokens = nltk.word_tokenize(raw)
    text = nltk.Text(tokens)
    result = text.generate(length)

if __name__ == "__main__":
    hello()

