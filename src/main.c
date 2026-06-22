#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "utils.h"
#include "lexer.h"
#include "parser.h"

int derle_ve_calistir(const char* dosya_yolu) {
    char* kaynak_kod = dosya_oku(dosya_yolu);
    if (!kaynak_kod) return 1;

    printf("\n[1/3] '%s' okunuyor...\n", dosya_yolu);
    TokenArray tokens = tokenize(kaynak_kod);

    printf("[2/3] C koduna ceviriliyor...\n");
    const char* c_cikti_yolu = "gecici_oz_cikti.c";
    if (!parse_and_transpile(&tokens, c_cikti_yolu)) {
        free(kaynak_kod);
        free_token_array(&tokens);
        return 1;
    }

    printf("[3/3] GCC ile derleniyor...\n");
    char komut[256];
    snprintf(komut, sizeof(komut), "gcc %s -I./src/include -o program", c_cikti_yolu);
    
    int gcc_sonuc = system(komut);
    
    if (gcc_sonuc == 0) {
        printf("========================================\n");
        printf("Basariyla derlendi! Program calistiriliyor...\n");
        printf("========================================\n\n");
        system("./program");
        printf("\n========================================\n");
    } else {
        printf("Derleme sirasinda hata olustu!\n");
    }

    free(kaynak_kod);
    free_token_array(&tokens);
    return 0;
}

void interaktif_menu() {
    DIR *d;
    struct dirent *dir;
    d = opendir("examples");
    
    char dosyalar[50][256];
    int dosya_sayisi = 0;

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // Sadece .ozp uzantılı dosyaları al
            if (strstr(dir->d_name, ".ozp") != NULL) {
                strcpy(dosyalar[dosya_sayisi], dir->d_name);
                dosya_sayisi++;
            }
        }
        closedir(d);
    }

    if (dosya_sayisi == 0) {
        printf("examples/ klasorunde hic .ozp dosyasi bulunamadi!\n");
        return;
    }

    printf("\n=== OZ+ INTERAKTIF MENU ===\n");
    printf("Calistirmak istediginiz dosyayi secin:\n");
    for (int i = 0; i < dosya_sayisi; i++) {
        printf("%d) %s\n", i + 1, dosyalar[i]);
    }
    printf("0) Cikis\n");
    printf("Seciminiz: ");

    int secim;
    if (scanf("%d", &secim) != 1) {
        printf("Gecersiz giris.\n");
        return;
    }

    if (secim == 0) {
        printf("Cikis yapiliyor...\n");
        return;
    } else if (secim > 0 && secim <= dosya_sayisi) {
        char tam_yol[512];
        snprintf(tam_yol, sizeof(tam_yol), "examples/%s", dosyalar[secim - 1]);
        derle_ve_calistir(tam_yol);
    } else {
        printf("Gecersiz secim!\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        // Argüman verilmediyse menüyü göster
        interaktif_menu();
        return 0;
    }

    // Argüman verildiyse direkt derle
    return derle_ve_calistir(argv[1]);
}
