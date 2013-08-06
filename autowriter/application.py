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
from twisted.internet.serialport import SerialPort
from twisted.python import log
from autowriter import configuration
from autowriter import webwriterpage
from autowriter import markovgenerator
from autowriter import textutils
from autowriter import hpgltext
from autowriter import plotter
import textwrap

class Application(object):
    """
    Main application.
    """
    def __init__(self, config_file, serial_port, baud_rate, web_port):
        self.config = None
        self._text_generator = None
        self._web_factory = None
        self._web_resource = None
        self._serial_port_manager = None
        self._spooler = None
        self._serial_port = serial_port
        self._baud_rate = baud_rate
        self._web_port = web_port

        self._parse_config_file(config_file)
        self._setup_plotter()
        self._setup_text_generator()
        self._start_web_server(web_port)
        self._last_generated_text = ""

    def _parse_config_file(self, config_file):
        log.msg("Parse config file %s" % (config_file))
        self.config = configuration.Configuration(config_file)
        log.msg("Done parsing config file %s" % (config_file))

    def _setup_plotter(self):
        self._serial_port
        self._baud_rate

        log.msg("Attempting to open %s at %dbps as a %s device" % (self._serial_port, self._baud_rate, plotter.PlotterProtocol.__name__))
        self._serial_port_manager = SerialPort(plotter.PlotterProtocol(), self._serial_port, reactor, baudrate=self._baud_rate)
        self._spooler = plotter.Spooler(self._serial_port_manager)

    def _setup_text_generator(self):
        log.msg("setup text generator")
        self._text_generator = markovgenerator.MarkovGenerator(self.config.text_file)

    def _start_web_server(self, web_port):
        log.msg("setup web server")
        self._web_resource = webwriterpage.WebWriterPage(self)
        self._web_factory = Site(self._web_resource)
        reactor.listenTCP(web_port, self._web_factory)

    def generate_text(self):
        log.msg("generate text")
        lines = self.config.num_lines
        chars = self.config.num_characters_per_line
        CHAR_PER_WORD = 4
        words = lines * chars / CHAR_PER_WORD

        text = self._text_generator.generate(words)
        cropped = textutils.crop_text(text, lines, chars)
        self._last_generated_text = cropped
        return cropped

    def plot_last_generated_text(self):
        if self._last_generated_text != "":
            hpgl = self.to_hpgl(self._last_generated_text)
            log.msg("PLOT A LOT OF HPGL NOW!")
            self._spooler.append(hpgl)
        else:
            log.msg("nothing to plot")

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
        topleft = self.config.topleft

        result = hpgltext.text_to_hpgl(text, font_dir, line_height, char_width, topleft)
        return result

