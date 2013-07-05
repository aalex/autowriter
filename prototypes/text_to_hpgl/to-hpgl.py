#!/usr/bin/env python
import os

def translate_xy_hpgl(text):
    #result = ""
    # tokens = text.split(
    return text

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

def run():
    letter = "X"
    prefix = "../../fonts/hershey"
    try:
        file_name = utf8_to_filename(prefix, letter)
        print(file_name)
    except RuntimeError, e:
        print(e)


if __name__ == "__main__":
    run()
        


