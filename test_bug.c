#include <stdio.h>
#include <string.h>

int main() {
    char* str = "_oz_son_hata_mesaji";
    if (strncmp(str, "_oz_", 4) != 0) {
        printf("True\n");
    } else {
        printf("False\n");
    }
    return 0;
}
