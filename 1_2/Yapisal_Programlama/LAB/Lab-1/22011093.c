#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n1, n2;
    int *polinom1[2], *polinom2[2];
    int sonuc[10][2];

    int i;

    printf("Ilk polinomun terim sayisi?\n"); // polinom 1 kullanıcıdan çekme
    scanf("%d", &n1);

    for (i = 0; i < n1; i++)
    {
        polinom1[i] = (int *)malloc(n1 * sizeof(int));
        printf("Ust ve Katsayisi?\n");
        printf("Ust:");
        scanf("%d", &polinom1[i][0]);
        printf("Katsayi:");
        scanf("%d", &polinom1[i][1]);
    }

    printf("Ikinci polinomun terim sayisi?\n"); // polinom 2 kullanıcıdan çekme
    scanf("%d", &n2);

    for (i = 0; i < n2; i++)
    {
        polinom2[i] = (int *)malloc(n2 * sizeof(int));
        printf("Ust ve Katsayisi?\n");
        printf("Ust:");
        scanf("%d", &polinom2[i][0]);
        printf("Katsayi:");
        scanf("%d", &polinom2[i][1]);
    }

    // polinomların çarpılması
    int t, p;
    for (t = 0; t < n1; t++)
    {
        for (p = 0; p < n2; p++)
        {
            sonuc[t][0] = polinom1[t][0] * polinom1[p][0];
            sonuc[t][1] = polinom1[t][1] * polinom1[p][1];
        }
    }

    int j, p;
    for (j = 0; j < n1; j++)
    {
        for (p = 0; p < 2; p++)
        {
            printf("%d ", polinom1[j][p]);
        }
        printf("\n");
    }
    printf("\n");
    for (j = 0; j < n2; j++)
    {
        for (p = 0; p < 2; p++)
        {
            printf("%d ", polinom2[j][p]);
        }
        printf("\n");
    }

    return 0;
}
