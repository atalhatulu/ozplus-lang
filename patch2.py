import re
with open('src/parser.c', 'r') as f:
    c = f.read()

# Replace IKINOKTA
old = '} else if (t.tip == TOKEN_IKINOKTA) {\n            fprintf(out, "{\\n");\n            current_scope++;'
new = '''} else if (t.tip == TOKEN_IKINOKTA) {
            if (in_eger) {
                fprintf(out, ") ? ");
            } else {
                fprintf(out, "{\\n");
                current_scope++;
            }'''
c = c.replace(old, new)

with open('src/parser.c', 'w') as f:
    f.write(c)
