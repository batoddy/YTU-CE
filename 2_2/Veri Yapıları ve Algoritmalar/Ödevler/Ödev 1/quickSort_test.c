#include <stdio.h>

#define N 10

void swap(int *, int *);

void print_arr(int *, int);

int main()
{
    // i < pivot < j
    int arr[N] = {5, 7, 4, 2, 9, 10, 3, 8, 12, 17};
    int pivot;
    int i = -1, j = 0, idx;
    pivot = arr[0];

    print_arr(arr, N);

    for (idx = 1; idx < N; idx++)
    {
        if (arr[idx] > pivot)
        {
            i++;
        }
        else if (arr[idx] < pivot)
        {
            swap(&arr[idx], &arr[j + 1]);
            j++;
        }
        print_arr(arr, N);
        printf("i: %d, j: %d\n", i, j);
    }

    swap(&arr[0], &arr[j]);
    print_arr(arr, N);
    return 0;
}

void swap(int *a, int *b)
{
    int tmp = *b;
    *b = *a;
    *a = tmp;
}

void print_arr(int *arr, int start, int end)
{
    int i;
    for (i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    // printf("\n");
}