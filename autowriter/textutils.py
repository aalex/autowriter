import textwrap

def remove_after_last_period(text):
    sep = "."
    tokens = text.split(sep)
    ret = ""
    for i in xrange(len(tokens)):
        if i != len(tokens) - 1 and i != len(tokens) - 2:
            token = tokens[i].strip()
            if token != "":
                ret += token + ". "
    return ret.strip()

def remove_before_first_period(text):
    sep = "."
    tokens = text.split(sep)
    ret = ""
    for i in xrange(len(tokens)):
        if i != 0:
            token = tokens[i].strip()
            if token != "":
                ret += token + ". "
    return ret.strip()

# TODO: characters_per_line
# TODO: num_lines

def wrap_text(text, num_lines, characters_per_line):
    lines = textwrap.wrap(text, characters_per_line)
    result = ""
    i = 0
    for line in lines:
        if (i < num_lines):
            result = result + line + "\n"
        i = i + 1
    return result

def crop_text(text, num_words):
    # TODO: remove before after first period
    # TODO: remove after last period
    result = wrap_text(text, 10, 70)
    return result

