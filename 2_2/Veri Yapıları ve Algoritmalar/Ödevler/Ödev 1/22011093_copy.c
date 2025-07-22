#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b);
void printArray(int *arr, int start, int end);
int read_txt(int **key, int **lock, int *len);
int partition(int *arr, int low, int high, int pivot);
void quickSortMatch(int *key, int *lock, int low, int high, int step);

int main()
{
    int *key, *lock;
    int len;

    read_txt(&key, &lock, &len);

    quickSortMatch(key, lock, 0, len - 1, 1);

    printf("Islem Sonu:\n");
    printf("Keys: ");
    printArray(key, 0, len - 1);
    printf("Locks: ");
    printArray(lock, 0, len - 1);

    free(key);
    free(lock);
    return 0;
}

void quickSortMatch(int *key, int *lock, int low, int high, int step)
{

        if (low >= high)
        return;

    printf("%d. Adim\n", step);
    printf("Keys: ");
    printArray(key, low, high);
    printf("Locks: ");
    printArray(lock, low, high);

    int pivot = key[low]; // Key dizisinden pivot seçiyoruz
    printf("Seçilen Key: %d\n", pivot);

    // Lock dizisini pivot'a göre partition et
    int pivotIndexInLock = partition(lock, low, high, pivot);

    printf("Locklar duzenleniyor:\n");
    printf("Keys: ");
    printArray(key, low, high);
    printf("Locks: ");
    printArray(lock, low, high);

    // Aynı pivot ile key dizisini partition et
    pivotIndexInLock = partition(key, low, high, lock[pivotIndexInLock]);

    printf("Keyler Lock -> %d ile duzenleniyor\n", lock[pivotIndexInLock]);
    printf("Keys: ");
    printArray(key, low, high);
    printf("Locks: ");
    printArray(lock, low, high);
    printf("%d. Adim Sonu:\n", step);
    printf("Keys: ");
    printArray(key, 0, high);
    printf("Locks: ");
    printArray(lock, 0, high);
    printf("\n");

    quickSortMatch(key, lock, low, pivotIndexInLock - 1, step + 1);
    quickSortMatch(key, lock, pivotIndexInLock + 1, high, step + 1);
}

int partition(int *arr, int low, int high, int pivot)
{
    int i = low;
    for (int j = low; j < high; j++)
    {
        if (arr[j] < pivot)
        {
            swap(&arr[i], &arr[j]);
            i++;
        }
        else if (arr[j] == pivot)
        {
            swap(&arr [j], &arr[high]);
            j--; // bu elemanı tekrar kontrol et
        }
    }
    swap(&arr[i], &arr[high]);
    return i;
}

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void printArray(int *arr, int start, int end)
{
    for (int i = start; i <= end; i++)
    {
        printf("%d", arr[i]);
        if (i < end)
            printf("/");
    }
    printf("\n");
}

int read_txt(int **key, int **lock, int *len)
{
    FILE *fp = fopen("veri.txt", "r");
    if (!fp)
    {
        perror("Dosya acilamadi");
        return 1;
    }

    fscanf(fp, "%d", len);
    *key = (int *)calloc(*len, sizeof(int));
    *lock = (int *)calloc(*len, sizeof(int));

    for (int i = 0; i < *len; i++)
        fscanf(fp, "%d", &(*key)[i]);
    for (int i = 0; i < *len; i++)
        fscanf(fp, "%d", &(*lock)[i]);

    fclose(fp);
    return 0;
}
