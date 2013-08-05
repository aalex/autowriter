#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Parses and contains the configuration.
Author: Alexandre Quessy
Date: July 2013
"""

from xml.dom import minidom
from twisted.python import log

def _get_text(nodelist):
    """
    Returns the text in an XML node.
    """
    rc = []
    for node in nodelist:
         if node.nodeType == node.TEXT_NODE:
             rc.append(node.data)
             return ''.join(rc)

class Configuration(object):
    """
    Contains the configuration options for style of the writer.
    Parses an XML config file, given as an argument to the constructor.
    In the form::

      <writer>
        <text>foo.txt</text>
        <font_directory>comicsans</font_directory>
        <num_lines>24</num_lines>
        <num_characters_per_line>70</num_characters_per_line>
        <line_height>400</line_height>
        <char_width>300</char_width>
        <topleft>4000</topleft> <!-- Y-axis coordinate for the first line to be drawn -->
      </writer>
    """
    def __init__(self, config_file):
        # Values:
        # List of text files to get inspired from
        self.text_file = ""
        self.font_directory = "fonts/hershey"
        # Length of the text to generate
        self.num_lines = 24
        self.num_characters_per_line = 70
        self.line_height = 400
        self.char_width = 300
        self.topleft = 4000

        # Populate them
        self._parse(config_file)

    def _parse(self, config_file):
        doc = minidom.parse(config_file)
        try:
            root = doc.getElementsByTagName("writer")[0]
        except IndexError, e:
            log.msg("Configuration._parse(%s): %s" % (config_file, str(e)))
            return None

        # TODO: load more than one text file
        try:
            self.text_file = _get_text(root.getElementsByTagName("text")[0].childNodes)
        except IndexError, e:
            log.msg("Configuration._parse(%s): %s" % (config_file, str(e)))
        try:
            self.font_directory = _get_text(root.getElementsByTagName("font_directory")[0].childNodes)
        except IndexError, e:
            log.msg(str(e))
        try:
            self.num_lines = int(_get_text(root.getElementsByTagName("num_lines")[0].childNodes))
        except IndexError, e:
            log.msg(str(e))
        try:
            self.num_characters_per_line = int(_get_text(root.getElementsByTagName("num_characters_per_line")[0].childNodes))
        except IndexError, e:
            log.msg(str(e))
        try:
            self.char_width = int(_get_text(root.getElementsByTagName("char_width")[0].childNodes))
        except IndexError, e:
            log.msg(str(e))
        try:
            self.line_height = int(_get_text(root.getElementsByTagName("line_height")[0].childNodes))
        except IndexError, e:
            log.msg(str(e))
        try:
            self.topleft = int(_get_text(root.getElementsByTagName("topleft")[0].childNodes))
        except IndexError, e:
            log.msg(str(e))

