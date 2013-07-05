#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
HPGL Text translation and drawing utilities.
Author: Alexandre Quessy
Date: July 2013
"""
from __future__ import print_function
import os
import sys

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
    """
    Stores the infos for a HPGL command.
    Form: two-letter command name, plus optional arguments.
    """
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
    """
    Splits tokens in a string. Separator is ";" or newline character.
    """
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
    """
    Convert a HPGL text command to and instance of HPGLCommand.
    """
    command = token[0:2]
    args = []
    leftover = token[2:]
    for comma_separated in leftover.split(","):
        for arg in comma_separated.split(" "):
            args.append(arg)
    return HPGLCommand(command, args)

def translate_pa(pa_command, translate_x, translate_y):
    """
    Translates a HPGLCommand PA's arguments by some XY offset.
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

def draw_character(character, font_dir, offset_x, offset_y):
    """
    Draws a HPGL character.

    return: text of the translated HPGL file.
    param character: utf8 one-character string
    param font_dir: directory containing the HPGL letters
    param offset_x: int
    param offset_i: int
    """
    result = ""
    try:
        file_name = utf8_to_filename(font_dir, character)
        open_file = open(file_name, "rU")
        open_file.seek(0)
        hpgl = open_file.read()
        #print(hpgl)
        #print("TRANSLATED:")
        tokens = hpgl_split(hpgl)
        for token in tokens:
            command = hpgl_command_to_object(token)
            if command.get_command() == "PA":
                command = translate_pa(command, offset_x, offset_y)
                result += str(command)
            else:
                # XXX: weird we have to write this line twice. see 
                # two lines above
                result += str(command) 
    except RuntimeError, e:
        pass
        # print(e)
    return result

if __name__ == "__main__":
    try:
        letter = sys.argv[1] # "X"
        offset_x = int(sys.argv[2]) # 0
        offset_y = int(sys.argv[3]) # 0
        FONT_DIR = "../../fonts/hershey"
        result = draw_character(letter, FONT_DIR, offset_x, offset_y)
        print(result, "", "")
    except IndexError:
        print("Usage: ./to-hpgl.py [letter] [offset_x] [offset_y]")
        sys.exit(1)

