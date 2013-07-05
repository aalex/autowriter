#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Main business logic.
Author: Alexandre Quessy
Date: July 2013
"""
import os
import sys
from twisted.internet import reactor
from twisted.web.server import Site
from autowriter import configuration
from autowriter import webwriterpage
from autowriter import markovgenerator
from autowriter import textutils
from autowriter import hpgltext
import textwrap

class Application(object):
    """
    Main application.
    """
    def __init__(self, config_file, web_port=8080):
        self.config = None
        self._text_generator = None
        self._web_factory = None
        self._web_resource = None

        self._parse_config_file(config_file)
        self._setup_text_generator()
        self._start_web_server(web_port)

    def _parse_config_file(self, config_file):
        self.config = configuration.Configuration(config_file)

    def _setup_text_generator(self):
        self._text_generator = markovgenerator.MarkovGenerator(self.config.text_file)

    def _start_web_server(self, web_port):
        self._web_resource = webwriterpage.WebWriterPage(self)
        self._web_factory = Site(self._web_resource)
        reactor.listenTCP(web_port, self._web_factory)

    def generate_text(self):
        lines = self.config.num_lines
        chars = self.config.num_characters_per_line
        CHAR_PER_WORD = 4
        words = lines * chars / CHAR_PER_WORD

        text = self._text_generator.generate(words)
        return textutils.crop_text(text, lines, chars)

    def to_hpgl(self, text):
        """
        Converts some text to HPGL.
        The lines must have the desired length and be separated by newline.

        param text: lines of text
        return: hpgl to draw it.
        """
        # gather config
        line_height = self.config.line_height
        char_width = self.config.char_width
        font_dir = self.config.font_directory

        result = hpgltext.text_to_hpgl(text, font_dir, line_height, char_width)
        return result


