#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Web page.
"""
from twisted.web.resource import Resource

class WebWriterPage(Resource):
    """
    Renders the Web page for the auto writer.
    """
    isLeaf = True

    def __init__(self, app):
        self._app = app

    def render_GET(self, request):
        text = self._app.generate_text()
        return """<html>
            <head>
            <title>autowriter</title>
            <meta http-equiv="content-type" content="text/html; charset=utf-8" />
            <style type="text/css">
            h1 {
                font: 24px bold arial;
            }
            body {
                font: 16px arial, sans-serif;
                color: #ccc;
                background-color: #000;
            }
            pre {
                border: 1px #fff;
            }
            </style>
            <body>
                <h1>autowriter</h1>
                <pre>%s</pre>
            </body>
            </html>""" % (text)

