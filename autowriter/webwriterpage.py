#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Web page.
Author: Alexandre Quessy
Date: July 2013
"""
from twisted.web.resource import Resource

class WebWriterPage(Resource):
    """
    Renders the Web page that show the generated text.
    The text is generated again each time the user refreshes the Web page.
    """
    isLeaf = True

    def __init__(self, app):
        self._app = app

    def render_GET(self, request):
        text = self._app.generate_text()
        hpgl = self._app.to_hpgl(text)
        self._app.plot_last_generated_text()

        result = """<html>
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
            .hpgl {
                font-size: 10px;
            }
            </style>
            <body>
                <h1>autowriter</h1>
                <pre>%s</pre>
                <pre style="hpgl">%s</pre>
            </body>
            </html>""" % (text, hpgl)
        return result

