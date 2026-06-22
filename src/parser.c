#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Basit sembol tablosu ve tip yönetimi
static char* symbol_table[1024];
static int symbol_types[1024]; // 0: long, 1: metin(char*), 2: ondalik(double)
static int symbol_count = 0;

static int is_declared(const char* name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i], name) == 0) return 1;
    }
    return 0;
}

static int get_var_type(const char* name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i], name) == 0) return symbol_types[i];
    }
    return 0;
}

static void declare_var(const char* name, int type) {
    if (!is_declared(name)) {
        symbol_table[symbol_count] = strdup(name);
        symbol_types[symbol_count] = type;
        symbol_count++;
    }
}

static void oz_hata_ver(int satir, const char* mesaj) {
    printf("\n[OZ+ DERLEME HATASI] Satir %d: %s\n", satir, mesaj);
}

static void free_symbol_table() {
    for (int i = 0; i < symbol_count; i++) {
        free(symbol_table[i]);
    }
    symbol_count = 0;
}

int parse_and_transpile(TokenArray* tokens, const char* c_cikti_yolu) {
    FILE* out = fopen(c_cikti_yolu, "w");
    if (!out) {
        printf("Hata: Cikti dosyasi olusturulamadi!\n");
        return 0;
    }
    free_symbol_table(); // Eski sembolleri temizle

    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include \"matematik.h\"\n");
    fprintf(out, "#include \"oz_standart.h\"\n\n");
    fprintf(out, "int main() {\n");

    int is_global = 1;
    int in_islev_decl = 0;

    for (int i = 0; i < tokens->count; i++) {
        Token t = tokens->tokens[i];
        
        if (t.tip == TOKEN_EOF) break;

        if (t.tip == TOKEN_ANAHTAR) {
            if (strcmp(t.deger, "degisken") == 0) {
                if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_ID) declare_var(tokens->tokens[i+1].deger, 0);
                fprintf(out, "long ");
            } else if (strcmp(t.deger, "metin") == 0) {
                if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_ID) declare_var(tokens->tokens[i+1].deger, 1);
                fprintf(out, "char* ");
            } else if (strcmp(t.deger, "ondalik") == 0) {
                if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_ID) declare_var(tokens->tokens[i+1].deger, 2);
                fprintf(out, "double ");
            } else if (strcmp(t.deger, "dizi") == 0) {
                if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_ID) declare_var(tokens->tokens[i+1].deger, 0); // Varsayilan long array
                fprintf(out, "long ");
            } else if (strcmp(t.deger, "girdi") == 0) {
                // girdi x veya girdi al x
                if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_ID && strcmp(tokens->tokens[i+1].deger, "al") == 0) {
                    i++; // 'al' kelimesini atla
                }
                if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_ID) {
                    fprintf(out, "scanf(\"%%ld\", &%s);\n", tokens->tokens[i+1].deger);
                    i++;
                }
            } else if (strcmp(t.deger, "dongu") == 0) {
                fprintf(out, "while (");
            } else if (strcmp(t.deger, "ise") == 0) {
                fprintf(out, "if (");
            } else if (strcmp(t.deger, "degilse") == 0) {
                if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_ANAHTAR && strcmp(tokens->tokens[i+1].deger, "ise") == 0) {
                    fprintf(out, "} else if (");
                    i++; // ise'yi atla
                } else {
                    fprintf(out, "} else ");
                    // Sonraki token IKINOKTA ise sadece '{' koy ve onu atla, '(' kapatmaya gerek yok
                    if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_IKINOKTA) {
                        fprintf(out, "{\n");
                        i++; // IKINOKTA'yi atla
                    }
                }
            } else if (strcmp(t.deger, "son") == 0) {
                fprintf(out, "}\n");
            } else if (strcmp(t.deger, "yazdir") == 0) {
                if (i + 1 < tokens->count) {
                    if (tokens->tokens[i+1].tip == TOKEN_ID) {
                        if (!is_declared(tokens->tokens[i+1].deger)) {
                            oz_hata_ver(t.satir, "Kullanilmaya calisilan degisken daha once tanimlanmamis!");
                            fclose(out); return 0;
                        }
                        int is_array = 0;
                        char array_idx[64] = "";
                        if (i + 2 < tokens->count && tokens->tokens[i+2].tip == TOKEN_KOSELI_AC) {
                            is_array = 1;
                            snprintf(array_idx, sizeof(array_idx), "[%s]", tokens->tokens[i+3].deger); // Basitce icindeki id/sayiyi alalim
                            i += 3; // Koseyi kapatmak icin i'yi arttiracagiz
                            if (i + 1 < tokens->count && tokens->tokens[i+1].tip == TOKEN_KOSELI_KAPA) i++;
                        }
                        
                        int vtype = get_var_type(tokens->tokens[i+1 - (is_array ? 4 : 0)].deger);
                        if (vtype == 1) {
                            fprintf(out, "printf(\"%%s\\n\", %s%s);\n", tokens->tokens[i+1 - (is_array ? 4 : 0)].deger, array_idx);
                        } else if (vtype == 2) {
                            fprintf(out, "printf(\"%%f\\n\", %s%s);\n", tokens->tokens[i+1 - (is_array ? 4 : 0)].deger, array_idx);
                        } else {
                            fprintf(out, "printf(\"%%ld\\n\", %s%s);\n", tokens->tokens[i+1 - (is_array ? 4 : 0)].deger, array_idx);
                        }
                        i++;
                    } else if (tokens->tokens[i+1].tip == TOKEN_METIN) {
                        fprintf(out, "printf(\"%%s\\n\", \"%s\");\n", tokens->tokens[i+1].deger);
                        i++;
                    } else if (tokens->tokens[i+1].tip == TOKEN_SAYI) {
                        fprintf(out, "printf(\"%%f\\n\", (double)%s);\n", tokens->tokens[i+1].deger);
                        i++;
                    } else if (tokens->tokens[i+1].tip == TOKEN_PARANTEZ_AC) {
                        // islem yazdirmaya calisiyor (5 + 3) gibi. Sadece int (long) desteklesin simdilik.
                        fprintf(out, "printf(\"%%ld\\n\", (long)");
                        i++;
                        while(i < tokens->count && tokens->tokens[i].satir == t.satir) {
                            // Satır sonuna kadar al
                            Token exp = tokens->tokens[i];
                            if(exp.tip == TOKEN_ID) fprintf(out, "%s ", exp.deger);
                            else if(exp.tip == TOKEN_SAYI) fprintf(out, "%s ", exp.deger);
                            else if(exp.tip == TOKEN_ARTI) fprintf(out, "+ ");
                            else if(exp.tip == TOKEN_EKS) fprintf(out, "- ");
                            else if(exp.tip == TOKEN_CARP) fprintf(out, "* ");
                            else if(exp.tip == TOKEN_BOL) fprintf(out, "/ ");
                            else if(exp.tip == TOKEN_PARANTEZ_AC) fprintf(out, "( ");
                            else if(exp.tip == TOKEN_PARANTEZ_KAPA) fprintf(out, ") ");
                            i++;
                        }
                        fprintf(out, ");\n");
                        i--; // Dıştaki for döngüsü bir artıracak, geri alalım
                    } else {
                        oz_hata_ver(t.satir, "'yazdir' komutundan sonra gecerli bir deger gelmeli!");
                        fclose(out); return 0;
                    }
                }
            } else if (strcmp(t.deger, "zaman_baslat") == 0) {
                fprintf(out, "_oz_zaman_baslat();\n");
            } else if (strcmp(t.deger, "zaman_bitir") == 0) {
                fprintf(out, "_oz_zaman_bitir();\n");
            } else if (strcmp(t.deger, "islev") == 0) {
                // islev isim: gibi varsayalım şimdilik
                fprintf(out, "long ");
                is_global = 0; // Fonksiyon başladığı için globali geçebiliriz
                in_islev_decl = 1;
            } else if (strcmp(t.deger, "don") == 0) {
                fprintf(out, "return ");
            }
        } else if (t.tip == TOKEN_ID) {
            fprintf(out, "%s ", t.deger);
        } else if (t.tip == TOKEN_METIN) {
            fprintf(out, "\"%s\" ", t.deger);
        } else if (t.tip == TOKEN_SAYI) {
            fprintf(out, "%s ", t.deger);
        } else if (t.tip == TOKEN_ESIT) {
            fprintf(out, "= ");
        } else if (t.tip == TOKEN_ARTI) {
            fprintf(out, "+ ");
        } else if (t.tip == TOKEN_EKS) {
            fprintf(out, "- ");
        } else if (t.tip == TOKEN_CARP) {
            fprintf(out, "* ");
        } else if (t.tip == TOKEN_BOL) {
            fprintf(out, "/ ");
        } else if (t.tip == TOKEN_MOD) {
            fprintf(out, "%% ");
        } else if (t.tip == TOKEN_IKINOKTA) {
            if (in_islev_decl) {
                fprintf(out, " {\n");
                in_islev_decl = 0;
            } else {
                fprintf(out, ") {\n");
            }
        } else if (t.tip == TOKEN_ESIT_ESIT) {
            fprintf(out, "== ");
        } else if (t.tip == TOKEN_ESIT_DEGIL) {
            fprintf(out, "!= ");
        } else if (t.tip == TOKEN_VE) {
            fprintf(out, "&& ");
        } else if (t.tip == TOKEN_VEYA) {
            fprintf(out, "|| ");
        } else if (t.tip == TOKEN_PARANTEZ_AC) {
            fprintf(out, "( ");
        } else if (t.tip == TOKEN_PARANTEZ_KAPA) {
            fprintf(out, ") ");
        } else if (t.tip == TOKEN_KOSELI_AC) {
            fprintf(out, "[ ");
        } else if (t.tip == TOKEN_KOSELI_KAPA) {
            fprintf(out, "] ");
        } else if (t.tip == TOKEN_VIRGUL) {
            fprintf(out, ", ");
        } else if (t.tip == TOKEN_KUCUK_ESIT) {
            fprintf(out, "<= ");
        } else if (t.tip == TOKEN_BUYUK_ESIT) {
            fprintf(out, ">= ");
        } else if (t.tip == TOKEN_KUCUK) {
            fprintf(out, "< ");
        } else if (t.tip == TOKEN_BUYUK) {
            fprintf(out, "> ");
        }

        // Noktalı virgül mantığı
        if (i + 1 < tokens->count) {
            Token next = tokens->tokens[i+1];
            if (next.satir > t.satir) {
                if (t.tip != TOKEN_IKINOKTA && t.tip != TOKEN_ANAHTAR && 
                    !(t.tip == TOKEN_ANAHTAR && strcmp(t.deger, "son") == 0) &&
                    !(t.tip == TOKEN_ANAHTAR && strcmp(t.deger, "yazdir") == 0)) {
                    if (t.tip == TOKEN_ANAHTAR && strcmp(t.deger, "degilse") == 0) {
                        fprintf(out, "{\n"); 
                    } else {
                        fprintf(out, ";\n");
                    }
                }
            }
        }
    }

    fprintf(out, "return 0;\n}\n");
    fclose(out);
    return 1;
}
