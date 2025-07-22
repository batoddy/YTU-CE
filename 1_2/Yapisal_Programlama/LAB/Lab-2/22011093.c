#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n, indx = 0, size = 2, i;
    int *sepet;

    printf("Enter number n: "); // n sayısı alınır
    scanf("%d", &n);

    sepet = (int *)calloc(size, sizeof(int)); // ilk memory 2*sizeof(int) boyutunda oluşturulur

    printf("%d. Adim Sepet = []\t\t eleman sayisi = %d | boyut = %d | n = %d\n", indx, indx, size, n);

    while (n != -1) // kod girdi olarak 1 verildiği zaman da çalışmaktadır
    {
        sepet[indx] = n; // n sayısı sepete eklenir
        if (n == 1)      // 1 değerine ulaşılmışsa n değişkeni exit flag'e eşitlenir
            n = -1;

        else if ((n % 2) == 1) // sağlanan koşullara göre n değeri değiştirilir
            n = 3 * n + 1;

        else if ((n % 2) == 0)
            n = n / 2;

        indx++;
        if (indx == size) // dizinin boyutu ile indexi karşılaştırılır, sepet dolmuşsa boyut 2 katına çıkarılı
        {
            size *= 2;
            sepet = realloc(sepet, size * sizeof(int));
        }
        printf("%d. Adim Sepet = [", indx); // Output yazdırma
        for (i = 0; i < indx; i++)
        {
            printf("%d", sepet[i]);
            if (i != indx - 1) // yalnızca virgülün doğru konumlanması için eklenmiş bir if bloğu
                printf(",");
        }
        printf("]\t\t eleman sayisi = %d | boyut = %d | n = %d\n", indx, size, n);
    }
    free(sepet); // kullanımı bittikten sonra bellek boşaltılır
}