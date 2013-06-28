import unittest
from autowriter import textutils

class TestTextUtils(unittest.TestCase):
    def test_wrap_text(self):
        cols = 70
        rows = 2
        source = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris nec "
        "felis non lacus pretium bibendum quis id massa. Praesent sit amet "
        "imperdiet magna, quis consectetur metus. Duis vel augue odio. Lorem "
        "ipsum dolor sit amet, consectetur adipiscing elit."
        expected = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris nec\n"
        "felis non lacus pretium bibendum quis id massa."

        result = textutils.wrap_text(source, rows, cols)
        if expected != result:
            self.fail("Didn't get what is expected: %s" % (result))

    def test_remove_before_first_period(self):
        source = "One. Two. Three."
        expected = "Two. Three."
        result = textutils.remove_before_first_period(source)
        if expected != result:
            self.fail("Didn't get what is expected: %s" % (result))

    def test_remove_after_last_period(self):
        source = "One. Two. Three."
        expected = "One. Two."
        result = textutils.remove_after_last_period(source)
        if expected != result:
            self.fail("Didn't get what is expected: %s" % (result))

