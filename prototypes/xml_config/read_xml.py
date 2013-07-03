#!/usr/bin/env python

from xml.dom import minidom

def get_text(nodelist):
    rc = []
    for node in nodelist:
         if node.nodeType == node.TEXT_NODE:
             rc.append(node.data)
             return ''.join(rc)

doc = minidom.parse("config/example1.xml")
root = doc.getElementsByTagName("writer")[0]
text = get_text(root.getElementsByTagName("text")[0].childNodes)
length = int(get_text(root.getElementsByTagName("length")[0].childNodes))

print(text)
print(length)

