#!/usr/bin/env python
"""
From http://stackoverflow.com/questions/1150144/generating-random-sentences-from-custom-text-in-pythons-nltk

But I got this::

  Traceback (most recent call last):
    File "./proto_02.py", line 58, in <module>
      text = markov_gen.generate_markov_text(50)
    File "./proto_02.py", line 51, in generate_markov_text
      w1, w2 = w2, random.choice(self.cache[(w1, w2)])
  KeyError: ('THE', 'END')
"""
import random
#FILE_NAME = "source_pg43038.txt"
FILE_NAME = "memoire_quessy.txt"

class Markov(object):
    def __init__(self, open_file):
        self.cache = {}
        self.open_file = open_file
        self.words = self.file_to_words()
        self.word_size = len(self.words)
        self.database()

    def file_to_words(self):
        self.open_file.seek(0)
        data = self.open_file.read()
        words = data.split()
        return words

    def triples(self):
        """ Generates triples from the given data string. So if our string were
        "What a lovely day", we'd generate (What, a, lovely) and then
        (a, lovely, day).
        """
        if len(self.words) < 3:
            return
        for i in range(len(self.words) - 2):
            yield (self.words[i], self.words[i+1], self.words[i+2])

    def database(self):
        for w1, w2, w3 in self.triples():
            key = (w1, w2)
            if key in self.cache:
                self.cache[key].append(w3)
            else:
                self.cache[key] = [w3]

    def generate_markov_text(self, size=25):
        seed = random.randint(0, self.word_size-3)
        seed_word, next_word = self.words[seed], self.words[seed+1]
        w1, w2 = seed_word, next_word
        gen_words = []
        for i in xrange(size):
            gen_words.append(w1)
            try:
                w1, w2 = w2, random.choice(self.cache[(w1, w2)])
            except KeyError, e:
                print(e)
        gen_words.append(w2)
        return ' '.join(gen_words)

if __name__ == "__main__":
    _file = open(FILE_NAME, "rU")
    markov_gen = Markov(_file)
    text = markov_gen.generate_markov_text(1000)
    print(text)

