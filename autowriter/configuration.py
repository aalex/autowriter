#!/usr/bin/env python
"""
Parses and contains the configuration.
"""

from xml.dom import minidom

def get_text(nodelist):
    """
    Returns the text in an XML node.
    """
    rc = []
    for node in nodelist:
         if node.nodeType == node.TEXT_NODE:
             rc.append(node.data)
             return ''.join(rc)

class Configuration(object):
    """
    Contains the configuration options for style of the writer.
    """
    def __init__(self, config_file):
        # Values:
        # List of text files to get inspired from
        self.text_files = []
        # Length of the text to generate
        self.length = 100

        # Populate them
        self._parse(config_file)

    def _parse(self, config_file):
        doc = minidom.parse(config_file)
        root = doc.getElementsByTagName("writer")[0]

        # TODO: load more than one text file
        self.text_files.append(get_text(root.getElementsByTagName("text")[0].childNodes))
        self.length = int(get_text(root.getElementsByTagName("length")[0].childNodes))
