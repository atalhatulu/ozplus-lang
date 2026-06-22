import re
with open('src/parser.c', 'r') as f:
    c = f.read()

# 1. TOKEN_SAYI double to long
c = c.replace('fprintf(out, "printf(\\"%%f\\\\n\\", (double)%s);\\n", tokens->tokens[i+1].deger);',
              'fprintf(out, "printf(\\"%%ld\\\\n\\", %sL);\\n", tokens->tokens[i+1].deger);')

# 2. implicit assignment (type inference)
old_id = '} else if (t.tip == TOKEN_ID) {\n            fprintf(out, "%s ", t.deger);'
new_id = '''} else if (t.tip == TOKEN_ID) {
            // Implicit declaration (Type Inference - Python like)
            if (strncmp(t.deger, "_oz_", 4) != 0 && !is_declared(t.deger) && i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_ESIT) {
                if (i + 2 < tokens->count && tokens->tokens[i+2].tip == TOKEN_METIN) {
                    fprintf(out, "char* ");
                    declare_var(t.deger, 1);
                } else if (i + 2 < tokens->count && tokens->tokens[i+2].tip == TOKEN_SAYI && strchr(tokens->tokens[i+2].deger, '.') != NULL) {
                    fprintf(out, "double ");
                    declare_var(t.deger, 2);
                } else if (i + 2 < tokens->count && tokens->tokens[i+2].tip == TOKEN_ID && strcmp(tokens->tokens[i+2].deger, "sozluk_olustur") == 0) {
                    fprintf(out, "Sozluk* ");
                    declare_var(t.deger, 3);
                } else if (i + 2 < tokens->count && tokens->tokens[i+2].tip == TOKEN_ANAHTAR && strcmp(tokens->tokens[i+2].deger, "hic") == 0) {
                    fprintf(out, "void* ");
                    declare_var(t.deger, 0); // void* icin type 0 veya ozel type
                } else {
                    fprintf(out, "long ");
                    declare_var(t.deger, 0);
                }
            }
            fprintf(out, "%s ", t.deger);'''
c = c.replace(old_id, new_id)

# 3. New keywords handling
old_kir = '} else if (strcmp(t.deger, "kir") == 0) {\n                fprintf(out, "break;\\n");\n            }'
new_kir = '''} else if (strcmp(t.deger, "kir") == 0) {
                fprintf(out, "break;\\n");
            } else if (strcmp(t.deger, "devam_et") == 0) {
                fprintf(out, "continue;\\n");
            } else if (strcmp(t.deger, "sabit") == 0) {
                fprintf(out, "const ");
            } else if (strcmp(t.deger, "Dogru") == 0) {
                fprintf(out, "1 ");
            } else if (strcmp(t.deger, "Yanlis") == 0) {
                fprintf(out, "0 ");
            } else if (strcmp(t.deger, "hic") == 0) {
                fprintf(out, "NULL ");
            } else if (strcmp(t.deger, "eger") == 0) {
                in_eger = 1;
                fprintf(out, "( ");
            }'''
c = c.replace(old_kir, new_kir)

# 4. degilse_ise
old_degilse = '''} else if (strcmp(t.deger, "degilse") == 0) {
                fprintf(out, "} else ");
                if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_IKINOKTA) {
                    fprintf(out, "{\\n");
                    current_scope++;
                    i++; // IKINOKTA'yi atla
                }
            }'''
new_degilse = '''} else if (strcmp(t.deger, "degilse") == 0) {
                if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_ANAHTAR && strcmp(tokens->tokens[i+1].deger, "ise") == 0) {
                    current_scope--;
                    for(int j=symbol_count-1; j>=0; j--) {
                        if(symbol_scopes[j] > current_scope) { 
                            if (symbol_types[j] == 3) fprintf(out, "sozluk_sil(%s);\\n", symbol_table[j]);
                            else if (symbol_types[j] == 4) fprintf(out, "free(%s);\\n", symbol_table[j]);
                            free(symbol_table[j]); symbol_count--; 
                        }
                    }
                    fprintf(out, "} else if (");
                    i++; // ise'yi atla
                } else {
                    current_scope--;
                    for(int j=symbol_count-1; j>=0; j--) {
                        if(symbol_scopes[j] > current_scope) { 
                            if (symbol_types[j] == 3) fprintf(out, "sozluk_sil(%s);\\n", symbol_table[j]);
                            else if (symbol_types[j] == 4) fprintf(out, "free(%s);\\n", symbol_table[j]);
                            free(symbol_table[j]); symbol_count--; 
                        }
                    }
                    fprintf(out, "} else ");
                    if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_IKINOKTA) {
                        fprintf(out, "{\\n");
                        current_scope++;
                        i++; // IKINOKTA'yi atla
                    }
                }
            } else if (strcmp(t.deger, "degilse_ise") == 0) {
                current_scope--;
                for(int j=symbol_count-1; j>=0; j--) {
                    if(symbol_scopes[j] > current_scope) { 
                        if (symbol_types[j] == 3) fprintf(out, "sozluk_sil(%s);\\n", symbol_table[j]);
                        else if (symbol_types[j] == 4) fprintf(out, "free(%s);\\n", symbol_table[j]);
                        free(symbol_table[j]); symbol_count--; 
                    }
                }
                fprintf(out, "} else if (");
            }'''
c = c.replace(old_degilse, new_degilse)

# 5. in_eger flag
c = c.replace('int is_global = 1;', 'int is_global = 1;\n    int in_eger = 0;')

# 6. ternary operator
old_ternary = '''} else if (t.tip == TOKEN_IKINOKTA) {
            fprintf(out, "{\\n");
            current_scope++;
        } else if (t.tip == TOKEN_VIRGUL) {
            fprintf(out, ", ");
        }'''
new_ternary = '''} else if (t.tip == TOKEN_IKINOKTA) {
            if (in_eger) {
                fprintf(out, ") ? ");
            } else {
                fprintf(out, "{\\n");
                current_scope++;
            }
        } else if (t.tip == TOKEN_VIRGUL) {
            if (in_eger) {
                fprintf(out, " : ");
                in_eger = 0;
            } else {
                fprintf(out, ", ");
            }
        }'''
c = c.replace(old_ternary, new_ternary)

# 7. Semicolon fix
old_semi = '''        if (i + 1 < tokens->count) {
            Token next = tokens->tokens[i+1];
            if (next.satir > t.satir) {
                if (t.tip != TOKEN_IKINOKTA && t.tip != TOKEN_ANAHTAR && 
                    !(t.tip == TOKEN_ANAHTAR && strcmp(t.deger, "son") == 0) &&
                    !(t.tip == TOKEN_ANAHTAR && strcmp(t.deger, "yazdir") == 0)) {
                    if (t.tip == TOKEN_ANAHTAR && strcmp(t.deger, "degilse") == 0) {
                        fprintf(out, "{\\n"); 
                    } else {
                        fprintf(out, ";\\n");
                    }
                }
            }
        }'''
new_semi = '''        if (i + 1 < tokens->count) {
            Token next = tokens->tokens[i+1];
            if (next.satir > t.satir) {
                int needs_semi = 1;
                if (t.tip == TOKEN_IKINOKTA) needs_semi = 0;
                if (t.tip == TOKEN_ANAHTAR) {
                    if (strcmp(t.deger, "Dogru") == 0 || strcmp(t.deger, "Yanlis") == 0 ||
                        strcmp(t.deger, "hic") == 0 || strcmp(t.deger, "kir") == 0 ||
                        strcmp(t.deger, "devam_et") == 0 || strcmp(t.deger, "don") == 0) {
                        needs_semi = 1;
                    } else if (strcmp(t.deger, "degilse") == 0) {
                        fprintf(out, "{\\n");
                        needs_semi = 0;
                    } else {
                        needs_semi = 0;
                    }
                }
                if (needs_semi) {
                    fprintf(out, ";\\n");
                }
            }
        }'''
c = c.replace(old_semi, new_semi)

with open('src/parser.c', 'w') as f:
    f.write(c)

