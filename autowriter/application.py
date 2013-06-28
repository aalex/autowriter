#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Main business logic.
"""
import os
import sys
from twisted.internet import reactor
from twisted.web.server import Site
from autowriter import configuration
from autowriter import webwriterpage
from autowriter import markovgenerator
from autowriter import textutils
import textwrap

class Application(object):
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
        self._text_generator = markovgenerator.MarkovGenerator(self.config.text_files[0])

    def _start_web_server(self, web_port):
        self._web_resource = webwriterpage.WebWriterPage(self)
        self._web_factory = Site(self._web_resource)
        reactor.listenTCP(web_port, self._web_factory)

    def generate_text(self):
        text = self._text_generator.generate(self.config.length)
        return textutils.crop_text(text, self.config.length)

