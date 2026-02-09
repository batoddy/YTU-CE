/*
 * Algoritma Analizi - Odev 2 - Problem 1
 * Sunucu Yerlestirme Problemi (Backtracking)
 *
 * Ogrenci No: 22011093
 */
#include <stdio.h>

#define N 4          // Sunucu sayisi
#define M 2          // Kabin sayisi
#define CAPACITY 100 // Kabin guc kapasitesi (W)
#define K 1.5        // Sogutma verimlilik esigi

typedef struct
{
    int P; // Guc tuketimi (W)
    int H; // Isi uretim katsayisi
} Server;

typedef struct
{
    int totalP; // Toplam guc
    int totalH; // Toplam isi
} Cabinet;

int solutionCount = 0;
int depth = 0;

/**
 * @brief Girintili cikti icin bosluk basar
 */
void printIndent()
{
    for (int i = 0; i < depth; i++)
        printf("  ");
}

/**
 * @brief Sunucunun kabine sigip sigmayacagini kontrol eder
 * @param serverIndex Yerlestirilecek sunucunun indeksi
 * @param cabinetIndex Hedef kabinin indeksi
 * @param servers Sunucu dizisi
 * @param cabinets Kabin dizisi
 * @return 1: sigar, 0: sigmaz
 */
int checkCapacity(int serverIndex, int cabinetIndex,
                  Server servers[], Cabinet cabinets[])
{
    return (cabinets[cabinetIndex].totalP + servers[serverIndex].P <= CAPACITY);
}

/**
 * @brief Sunucunun kabindeki diger sunucularla cakisip cakismadigini kontrol eder
 * @param serverIndex Yerlestirilecek sunucunun indeksi
 * @param cabinetIndex Hedef kabinin indeksi
 * @param conflict Cakisma matrisi (NxN)
 * @param place Her sunucunun hangi kabinde oldugunu tutan dizi
 * @return 1: cakisma yok, 0: cakisma var
 */
int checkConflict(int serverIndex, int cabinetIndex,
                  int conflict[N][N], int place[])
{
    int result = 1;
    int i = 0;
    while (i < serverIndex && result == 1)
    {
        if (conflict[serverIndex][i] == 1 && place[i] == cabinetIndex)
        {
            result = 0;
        }
        i++;
    }
    return result;
}

/**
 * @brief Tek bir kabinin isi kosulunu saglaip saglamadigina bakar
 * @param cab Kontrol edilecek kabin
 * @return 1: uygun, 0: isi fazla
 */
int checkHeat(Cabinet *cab)
{
    int result = 1;
    if (cab->totalP > 0)
    {
        double ratio = (double)cab->totalH / cab->totalP;
        if (ratio > K)
        {
            result = 0;
        }
    }
    return result;
}

/**
 * @brief Tum kabinlerin isi kosulunu kontrol eder
 * @param cabinets Kabin dizisi
 * @return 1: hepsi uygun, 0: en az biri uygun degil
 */
int checkAllHeat(Cabinet cabinets[])
{
    int result = 1;
    int c = 0;
    while (c < M && result == 1)
    {
        if (!checkHeat(&cabinets[c]))
        {
            result = 0;
        }
        c++;
    }
    return result;
}

/**
 * @brief Bulunan cozumu ekrana yazdirir
 * @param place Sunucularin kabin atamalari
 * @param cabinets Kabin bilgileri
 */
void printSolution(int place[], Cabinet cabinets[])
{
    solutionCount++;
    printf("\n");
    printIndent();
    printf("=== Cozum %d ===\n", solutionCount);

    for (int c = 0; c < M; c++)
    {
        printIndent();
        printf("Kabin %d: {", c + 1);
        int first = 1;
        for (int i = 0; i < N; i++)
        {
            if (place[i] == c)
            {
                if (!first)
                    printf(", ");
                printf("S%d", i + 1);
                first = 0;
            }
        }
        printf("}");
        if (cabinets[c].totalP > 0)
        {
            double ratio = (double)cabinets[c].totalH / cabinets[c].totalP;
            printf(" -> P=%dW, H=%d, oran=%.2f",
                   cabinets[c].totalP, cabinets[c].totalH, ratio);
        }
        printf("\n");
    }
}

/**
 * @brief Backtracking ile sunuculari kabinlere yerlestirir
 * @param i Su an yerlestirilecek sunucu indeksi
 * @param servers Sunucu dizisi
 * @param cabinets Kabin dizisi
 * @param conflict Cakisma matrisi
 * @param place Yerlestirme sonuclarini tutan dizi
 */
void backtrack(int i, Server servers[], Cabinet cabinets[],
               int conflict[N][N], int place[])
{
    // Yerlestirme kontrolu
    if (i == N)
    {
        if (checkAllHeat(cabinets))
        {
            printSolution(place, cabinets);
        }
        else
        {
            printIndent();
            printf("Isi kisiti tutmadi, geri donuluyor.\n");
        }
    }
    else
    {
        printIndent();
        printf("S%d yerlestirilecek:\n", i + 1);
        depth++;

        for (int c = 0; c < M; c++)
        {
            printIndent();
            printf("Kabin %d -> ", c + 1);

            if (!checkCapacity(i, c, servers, cabinets)) // Kapasite kontrolü
            {
                printf("kapasite yetersiz (%d+%d > %d)\n",
                       cabinets[c].totalP, servers[i].P, CAPACITY);
            }

            else if (!checkConflict(i, c, conflict, place)) // Çakışma Kontorlü
            {
                printf("cakisma var\n");
            }

            else // yoksa yerleştir
            {
                place[i] = c;
                cabinets[c].totalP += servers[i].P;
                cabinets[c].totalH += servers[i].H;

                double ratio = (double)cabinets[c].totalH / cabinets[c].totalP;
                printf("OK (P=%d, H=%d, oran=%.2f)\n",
                       cabinets[c].totalP, cabinets[c].totalH, ratio);

                backtrack(i + 1, servers, cabinets, conflict, place);

                // Geri alma
                cabinets[c].totalP -= servers[i].P;
                cabinets[c].totalH -= servers[i].H;

                printIndent();
                printf("S%d geri alindi (Kabin %d)\n", i + 1, c + 1);
            }
        }

        depth--;
    }
}

int main()
{
    printf("N=%d sunucu, M=%d kabin\n", N, M);
    printf("Kabin kapasitesi: %dW\n", CAPACITY);
    printf("Sogutma esigi K: %.1f\n\n", K);

    Server servers[N] = {
        {60, 120}, // S1
        {50, 40},  // S2
        {40, 30},  // S3
        {30, 60}   // S4
    };

    printf("Sunucular:\n");
    printf("  Sunucu  P(W)   H    H/P\n");
    for (int i = 0; i < N; i++)
    {
        printf("  S%d      %3d   %3d   %.2f\n",
               i + 1, servers[i].P, servers[i].H,
               (double)servers[i].H / servers[i].P);
    }

    int conflict[N][N] = {
        {0, 1, 0, 0},
        {1, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};

    printf("\nCakisma: S1-S2 ayni kabine konamaz\n");
    printf("\n----------------------------------\n\n");

    Cabinet cabinets[M] = {{0, 0}, {0, 0}};
    int place[N];

    backtrack(0, servers, cabinets, conflict, place);

    printf("\n----------------------------------\n");
    printf("Toplam %d cozum bulundu.\n", solutionCount);

    return 0;
}