#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Main entry point of the application.
Author: Alexandre Quessy
Date: July 2013
"""
import os
import sys
import traceback
from optparse import OptionParser
from twisted.internet import reactor
from twisted.internet import defer
import autowriter
from twisted.python import log
from autowriter import application

DESCRIPTION = "generates writings."

def run():
    """
    Runs the application.
    """
    parser = OptionParser(usage="%prog [config file] [options]", version="%prog " + autowriter.__version__, description=DESCRIPTION)
    parser.add_option("-f", "--config-file", type="string",
                        help="Specifies the config file. You can also simply specify the config file as the first argument.")
    parser.add_option("-v", "--verbose", action="store_true", help="Makes the logging output verbose.")
    parser.add_option("-d", "--debug", action="store_true", help="Makes the logging output very verbose.")

    (options, args) = parser.parse_args()

    log.startLogging(sys.stdout)

    if options.config_file:
        config_file = options.config_file
    DEFAULT_CONFIG_FILE = os.path.expanduser("~/.autowriter")
    if len(args) == 1 and not options.config_file: 
        config_file = args[0] 
    else:
        config_file = DEFAULT_CONFIG_FILE

    error_message = None
    if not os.path.exists(config_file):
        error_message = "No such file: %s" % (config_file)
        print(error_message)
        sys.exit(1)

    app = application.Application(config_file)

    try:
        reactor.run()
    except KeyboardInterrupt:
        reactor.stop()

