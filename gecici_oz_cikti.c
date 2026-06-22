#include <stdio.h>
#include "matematik.h"
#include "oz_standart.h"

int main() {
printf("%s\n", "=== OZ+ TELEFON REHBERI ===");
printf("%s\n", "Rehberimize hos geldiniz.");
printf("%s\n", "Cikmak ve programi sonlandirmak icin 'cikis' yazabilirsiniz.");
printf("%s\n", "---------------------------");
OzSozluk* rehber = sozluk_olustur ( ) ;
sozluk_ekle ( rehber , "Ahmet" , "0555 123 45 67" ) ;
sozluk_ekle ( rehber , "Ayse" , "0532 987 65 43" ) ;
sozluk_ekle ( rehber , "Mehmet" , "0505 111 22 33" ) ;
sozluk_ekle ( rehber , "cikis" , "CIKIS_ISTENDI" ) ;
while (1 > 0 ) {
    printf("%s\n", "Kimin numarasini ariyorsunuz? (Ornek: Ayse)");
    char aranan_kisi[256] = ""; scanf(" %255[^\n]", aranan_kisi);     char* sonuc = sozluk_getir ( rehber , aranan_kisi ) ;
    printf("%s\n", "Kayitli Numara:");
    printf("%s\n", sonuc);
    printf("%s\n", "---------------------------");
}
return 0;
}
