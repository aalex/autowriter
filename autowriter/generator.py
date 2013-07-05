#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
The TextGenerator abstract class.
Author: Alexandre Quessy
Date: July 2013
"""

class TextGenerator(object):
    """
    Abstract class for text generators.
    """
    def __init__(self, text_file_name):
        pass

    def generate(self, number_of_words):
        raise RuntimeError("Must be overrided in child class.")
        
