with open('src/parser.c', 'r') as f:
    c = f.read()

import re
c = re.sub(r'\} else if \(t\.tip == TOKEN_IKINOKTA\) \{\s*fprintf\(out, "\\{\\\\n"\);\s*current_scope\+\+;',
           '} else if (t.tip == TOKEN_IKINOKTA) {\\n            if (in_eger) {\\n                fprintf(out, ") ? ");\\n            } else {\\n                fprintf(out, "{\\\\n");\\n                current_scope++;\\n            }',
           c)

with open('src/parser.c', 'w') as f:
    f.write(c)
