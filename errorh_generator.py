#!/usr/bin/env python3

errors = open("lang_errors", "r").read().strip().splitlines()

intro = """\
#ifndef ERRORS_H
#define ERRORS_H
"""

error_template = """
#define {name} \\
    do {{ \\
        fprintf(stderr, \"{error}\"); \\
        exit(-1); \\
    }} while (0)
"""

outro = """
#endif\
"""

outfile = open("errors.h", "w")
outfile.write(intro)

for error in errors:
    print(error)
    outfile.write(error_template.format(name=error[:error.find(':')], error=error))

outfile.write(outro)
outfile.close()