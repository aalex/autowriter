#!/usr/bin/env python
"""
Sends/receives to/from a plotter over USB.
"""

from twisted.python import log
from twisted.python import usage
from twisted.internet import reactor
from twisted.internet import defer
from twisted.internet.serialport import SerialPort
from twisted.protocols import basic
import sys
import os

class PlotterProtocol(basic.LineReceiver):
    """
    Simple protocol to send and receive ASCII to/from a plotter.
    """
    delimiter = ";"
    bufferSize = 1 # XXX

    def lineReceived(self, line):
        log.msg("Received: \"%s\"" % (line))

def slowly_write(serial_port, text):
    def _later(new_text):
        c = new_text[0:1]
        log.msg("writing %s" % (c))
        serial_port.write(c)
        new_text = new_text[1:]
        if len(new_text) == 0:
            log.msg("done writing")
        else:
            reactor.callLater(0.01, _later, new_text)
    _later(text)
    
class Options(usage.Options):
    """
    Command line options for this program
    """
    optFlags = [
        ["verbose", "v", "Print many info"],
    ]
    optParameters = [
        ["outfile", "o", None, "Logfile [default: sys.stdout]"],
        ["baudrate", "b", None, "Serial baudrate [default: 9600]"],
        ["file", "f", None, "File name to read the HPGL from."],
        ["port", "p", '/dev/ttyACM0', "Serial Port device"],
    ]

def query_output_actual_point(serial_port):
    """
    Sends OA;
    """
    log.msg("Write OA;")
    s.write("OA;")
    return defer.succeed(None)

def send_file_contents(serial_port, file_name):
    """
    Sends the contents of a file.
    """
    if os.path.isfile(file_name):
        if os.access(file_name, os.R_OK):
            log.msg("Write file contents from %s" % (file_name))
            with open(file_name, "rU") as f:
                contents = f.read()
                slowly_write(s, contents)
                # s.write(contents)
                return defer.succeed(None)
        else:
            log.msg("File is not readable: %s" % (file_name))
            return defer.fail(RuntimeError("File is not readable"))
    else:
        log.msg("File does not exist: %s" % (file_name))
        return defer.fail(RuntimeError("File does not exist"))

if __name__ == '__main__':
    options = Options()
    # defaults
    baudrate = 9600
    filename = None
    port = None
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
    log.msg("Attempting to open %s at %dbps as a %s device" % (port, baudrate, PlotterProtocol.__name__))
    s = SerialPort(PlotterProtocol(), port, reactor, baudrate=baudrate)

    if filename is None:
        log.msg("No file name provided.")
    else:
        if os.path.isfile(filename) and os.access(filename, os.R_OK):
            reactor.callLater(0.0, send_file_contents, s, filename)
        else:
            log.msg("File does not exist or is not readable: %s" % (filename))

    #reactor.callLater(0.0, query_output_actual_point, s)
    reactor.run()

