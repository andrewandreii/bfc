#!/usr/bin/env python3

errors = open("lang_errors", "r").read().strip().splitlines()

intro = """\
#ifndef ERRORS_H
#define ERRORS_H

#define expected_token_error(got, expected) \\
    do {{ \\
        fprintf(stderr, \"Error encountered at line %d, col %d\\n\", got->source_file_pos.line_no, (got)->source_file_pos.col_no); \\
        fprintf(stderr, \"Expected token of type: %s not %s.\\n\", token_names[expected], token_names[(got)->type]); \\
        exit(-1); \\
    }} while (0)
"""

error_template = """
#define {name}(t) \\
    do {{ \\
        fprintf(stderr, \"Error encountered at line %d, col %d\\n\", (t)->source_file_pos.line_no, (t)->source_file_pos.col_no); \\
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