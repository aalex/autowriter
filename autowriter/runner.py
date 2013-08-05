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
    # Setup command-line options:
    parser = OptionParser(usage="%prog [config file] [options]", version="%prog " + autowriter.__version__, description=DESCRIPTION)
    parser.add_option("-f", "--config-file", type="string",
                        help="Specifies the config file. You can also simply specify the config file as the first argument.")
    parser.add_option("-v", "--verbose", action="store_true", help="Makes the logging output verbose.")
    parser.add_option("-d", "--debug", action="store_true", help="Makes the logging output very verbose.")
    parser.add_option("-b", "--baud-rate", type="int", help="Specifies the baud rate of the plotter. Default is 9600.")
    parser.add_option("-p", "--port", type="string", help="Serial port device. Default is /dev/ttyUSB0")

    (options, args) = parser.parse_args()

    # Load default values:
    log.startLogging(sys.stdout)
    log.msg("load default option values")
    DEFAULT_CONFIG_FILE = os.path.expanduser("~/.autowriter")
    DEFAULT_SERIAL_PORT = "/dev/ttyUSB0"
    DEFAULT_BAUD_RATE = 9600
    DEFAULT_WEB_PORT = 8080

    config_file = DEFAULT_CONFIG_FILE
    baud_rate = DEFAULT_BAUD_RATE
    serial_port = DEFAULT_SERIAL_PORT
    web_port = DEFAULT_WEB_PORT

    # Update with command-line options:
    log.msg("Update with command-line options")
    if options.config_file:
        config_file = options.config_file
    if len(args) == 1 and not options.config_file: 
        config_file = args[0] 
    else:
        config_file = DEFAULT_CONFIG_FILE

    # Check for config file:
    log.msg("Check for config file")
    error_message = None
    if not os.path.exists(config_file):
        error_message = "No such file: %s" % (config_file)
        print(error_message)
        sys.exit(1)

    # Run application:
    log.msg("Instanciate application")
    app = application.Application(config_file, serial_port, baud_rate, web_port)
    try:
        log.msg("Run the Twisted reactor")
        reactor.run()
    except KeyboardInterrupt:
        reactor.stop()

