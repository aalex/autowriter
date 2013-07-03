#!/usr/bin/env python
"""
Write "ping\n" to the serial port and wait for its output.
See tempi-snake/arduino/tempi_snake_arduino_firmware
"""

# The idea is to send HPGL
#
# Query its state:
# oa;

from twisted.python import log
from twisted.python import usage
from twisted.internet import reactor
from twisted.internet.serialport import SerialPort
from twisted.protocols import basic
import sys
import os

class StupidProtocol(basic.LineReceiver):
    delimiter = "\n"

    def lineReceived(self, line):
        line = line.strip("\n")
        if line == "pong":
            log.msg("Received pong!");
        elif line == "hello":
            log.msg("Received hello!");
        elif line.startswith == "error":
            log.msg("Received error: %s" % (line));
        else:
            log.msg("Received: \"%s\"" % (line))

class Options(usage.Options):
    optFlags = [
        ["verbose", "v", "Print many info"],
    ]
    optParameters = [
        ["outfile", "o", None, "Logfile [default: sys.stdout]"],
        ["baudrate", "b", None, "Serial baudrate [default: 9600]"],
        ["file", "f", None, "File name to read the HPGL from."],
        ["port", "p", '/dev/ttyACM0', "Serial Port device"],
    ]

if __name__ == '__main__':
    options = Options()
    baudrate = 9600
    filename = None
    try:
        options.parseOptions()
    except usage.UsageError, errortext:
        print "%s: %s" % (sys.argv[0], errortext)
        print "%s: Try --help for usage details." % (sys.argv[0])
        raise SystemExit, 1

    # Logging option
    logFile = options.opts["outfile"]
    if logFile is None:
        logFile = sys.stdout
    log.startLogging(logFile)

    # Verbose
    if options.opts['verbose']:
        print("TODO: be verbose")
    # Baud rate
    if options.opts["baudrate"]:
        baudrate = int(options.opts["baudrate"])
    if options.opts["file"]:
        filename = options.opts["file"]
    # Port
    port = options.opts["port"]

    # Open it
    log.msg("Attempting to open %s at %dbps as a %s device" % (port, baudrate, StupidProtocol.__name__))
    s = SerialPort(StupidProtocol(), options.opts['port'], reactor, baudrate=baudrate)

    def _send_file_contents(file_name, serial_port):
        log.msg("Write file contents from %s" % (file_name))
        with open(file_name, "rU") as f:
            contents = f.read()
            s.write(contents)

    if filename is None:
        log.msg("No file name provided.")
    else:
        if os.path.isfile(filename) and os.access(filename, os.R_OK):
            reactor.callLater(0.0, _send_file_contents, filename, s)
        else:
            log.msg("File does not exist or is not readable: %s" % (filename))

    reactor.run()

