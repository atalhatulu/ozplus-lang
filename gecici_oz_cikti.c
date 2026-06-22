#include <stdio.h>
#include "matematik.h"
#include "oz_standart.h"

int main() {
long topla ( long a , long b )  {
return a + b ;
}
long cikar ( long a , long b )  {
return a - b ;
}
printf("%s\n", "=== OZ+ HESAP MAKINESI ===");
printf("%s\n", "Birinci sayiyi girin:");
long sayi1 ;
scanf("%ld", &sayi1);
printf("%s\n", "Ikinci sayiyi girin:");
long sayi2 ;
scanf("%ld", &sayi2);
printf("%s\n", "Toplam:");
long sonuc = topla ( sayi1 , sayi2 ) ;
printf("%ld\n", sonuc);
printf("%s\n", "Fark:");
long sonuc2 = cikar ( sayi1 , sayi2 ) ;
printf("%ld\n", sonuc2);
printf("%s\n", "Çarpım:");
long sonuc = carp ( sayi1 , sayi2 ) ;
printf("%ld\n", sonuc);
return 0;
}
