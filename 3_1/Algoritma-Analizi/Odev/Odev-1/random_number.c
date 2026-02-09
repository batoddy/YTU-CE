#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    int x;
    float x_;
    FILE *fp;

    printf("Kac adet sayi uretilecek (milyon): ");
    scanf("%f", &x_);

    x = (int)(x_ * 1000000);

    // Bellek ayir
    int *arr = (int *)malloc(x * sizeof(int));
    if (arr == NULL)
    {
        printf("Bellek ayirma hatasi!\n");
        return 1;
    }

    // 1..x araligini doldur
    for (int i = 0; i < x; i++)
        arr[i] = i + 1;

    // Rastgelelik icin seed
    srand((unsigned int)time(NULL));

    // Fisher-Yates Shuffle
    for (int i = x - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }

    // Dosyayi ac
    fp = fopen("random_numbers.txt", "w");
    if (fp == NULL)
    {
        printf("Dosya acilamadi!\n");
        free(arr);
        return 1;
    }

    // Ilk satira x yaz
    fprintf(fp, "%d\n", x);

    // Sayilari yaz
    for (int i = 0; i < x; i++)
        fprintf(fp, "%d\n", arr[i]);

    fclose(fp);
    free(arr);

    printf("random_numbers.txt olusturuldu.\n");
    return 0;
}
