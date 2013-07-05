#!/usr/bin/env python
from __future__ import print_function
import os

def utf8_to_number(character):
    return ord(unicode(character))

def utf8_to_filename(prefix, character):
    EXTENSION = ".hpgl"
    with_u = os.path.join(prefix, str(utf8_to_number(character)) + "u" + EXTENSION)
    normal = os.path.join(prefix, str(utf8_to_number(character)) + EXTENSION)
    if os.path.exists(with_u):
        return with_u
    elif os.path.exists(normal):
        return normal
    else:
        raise RuntimeError("File not found: %s" % (normal))

class HPGLCommand(object):
    def __init__(self, command, arguments):
        self._command = command
        self._arguments = arguments

    def get_command(self):
        return self._command

    def get_arguments(self):
        return self._arguments

    def set_arguments(self, arguments):
        self._arguments = arguments

    def __str__(self):
        result = self._command
        num_args = len(self._arguments)

        for i in xrange(num_args):
            result += self._arguments[i]
            if i == (num_args - 1):
                pass
            else:
                result += ","
        result += ";"
        return result

def hpgl_split(hpgl):
    result = []
    lines = hpgl.splitlines()
    for line in lines:
        if line == "":
            pass
        else:
            for token in line.split(";"):
                if token == "":
                    pass
                else:
                    result.append(token)
    return result

def hpgl_command_to_object(token):
    command = token[0:2]
    args = []
    leftover = token[2:]
    for comma_separated in leftover.split(","):
        for arg in comma_separated.split(" "):
            args.append(arg)
    return HPGLCommand(command, args)

def translate_pa(pa_command, translate_x, translate_y):
    """
    Translates a PA command's arguments by some XY offset.
    Does not currently support floats, just integers.
    """
    #result = ""
    # tokens = text.split(
    # TODO

    args = pa_command.get_arguments()
    result_args = []
    num_args = len(args)
    for i in xrange(num_args):
        value = int(args[i])
        modulo_two = i % 2
        if modulo_two == 0:
            value += translate_x
        else:
            value += translate_y
        result_args.append(str(value))
    pa_command.set_arguments(result_args)
    return pa_command

def run():
    letter = "X"
    prefix = "../../fonts/hershey"
    try:
        file_name = utf8_to_filename(prefix, letter)
        print(file_name)

        open_file = open(file_name, "rU")
        open_file.seek(0)
        hpgl = open_file.read()
        print(hpgl)

        print("TRANSLATED:")
        tokens = hpgl_split(hpgl)
        for token in tokens:
            command = hpgl_command_to_object(token)
            if command.get_command() == "PA":
                command = translate_pa(command, 10, 10)
            print(str(command), "", "")
    except RuntimeError, e:
        print(e)

if __name__ == "__main__":
    run()

