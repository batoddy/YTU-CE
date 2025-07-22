#include <stdio.h>
#include <stdlib.h>

#define N 10

void swap(int *, int *);
void print_arr(int *, int);

int read_txt(int **key, int **lock, int *len);
int partition(int *, int, int);
void quickSort(int *, int, int);

int main()
{
    int *key, *lock;
    int len, i;
    read_txt(&key, &lock, &len);

    printf("len: %d\n", len);

    printf("Key: ");
    for (i = 0; i < len; i++)
        printf("%d ", key[i]);

    printf("\nLock: ");
    for (i = 0; i < len; i++)
        printf("%d ", lock[i]);
    printf("\n ");

    quickSort(key, 0, len - 1);

    printf(" \nQucik Sorted Array: \n");
    print_arr(key, len);

    free(key);
    free(lock);
    return 0;
}

int partition(int *arr, int low, int high)
{
    int border;
    int i = low;
    int j;
    int pivot = arr[low];
    printf("\n");

    for (j = low; j <= high; j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
        print_arr(arr, high - low);
        printf("i: %d, j: %d\n", i, j);
    }

    swap(&arr[i], &arr[low]);
    printf("\n Sorted \n");
    print_arr(arr, high - low + 1);
    printf("\n");
    border = i;

    return border;
}

void quickSort(int *arr, int low, int high)
{
    int border;
    if (low < high)
    {
        border = partition(arr, low, high);
        quickSort(arr, low, border - 1);
        quickSort(arr, border + 1, high);
    }
}

void print_arr(int *arr, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    // printf("\n");
}

void swap(int *a, int *b)
{
    int tmp = *b;
    *b = *a;
    *a = tmp;
}

int read_txt(int **key, int **lock, int *len)
{
    FILE *fp;
    int i = 0;

    fp = fopen("veri.txt", "r");

    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    fscanf(fp, "%d", len);

    *key = (int *)calloc(sizeof(int), *len);
    *lock = (int *)calloc(sizeof(int), *len);

    for (i = 0; i < *len; i++)
        fscanf(fp, "%d ", &(*key)[i]);

    for (i = 0; i < *len; i++)
        fscanf(fp, "%d ", &(*lock)[i]);

    fclose(fp);
    return 0;
}
