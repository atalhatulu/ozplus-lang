#include <stdio.h>
#include "lexer.h"

int main() {
    TokenList tokens = lex_source("examples/cop_toplayici.ozp");
    for(int i=0; i<tokens.count; i++) {
        printf("Token: %s, satir: %d\n", tokens.tokens[i].deger, tokens.tokens[i].satir);
    }
    return 0;
}
