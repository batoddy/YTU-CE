#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0 // debug terminal çıktılarını açmak için 0- RUN mode / 1- DEBUG mode

#define GET_CUSTOM_DB 0 // custom oluşturulan data seti input olarak almak için 0 - verilen DB'yi kullan / 1- oluşturulan 2. DB'yi kullan

#define TXT_PATH "ID-1_5M.txt"
#define TXT_PATH_CUSTOM "random_numbers.txt"

typedef struct Node
{
    int data;
    struct Node *next;
} Node;

/**
 * @brief
 *  Reads integer data from a text file.
 *  First line contains total element count (x),
 *  followed by x integer values.
 *
 * @param file_path Path to input .txt file
 * @param x Pointer to store number of elements
 * @param data_array Pointer to dynamically allocated integer array
 *
 * @return 0 on success, 1 on file or read error
 */
int read_txt(char *file_path, int *x, int **data_array);

/**
 * @brief
 *  Checks whether a given integer is a prime number.
 *
 * @param nbr Integer to be checked
 *
 * @return 1 if prime, 0 otherwise
 */
int is_prime(int nbr);

/**
 * @brief
 *  Finds the smallest prime number greater than or equal to given value.
 *
 * @param nbr Starting integer
 *
 * @return Nearest prime number >= nbr
 */
int find_nearest_prime(int nbr);

/**
 * @brief
 *  Determines hash table size based on input size.
 *  Hash table size is chosen as the nearest prime >= x / 10.
 *
 * @param x Number of input elements
 *
 * @return Hash table size
 */
int set_hash_size(int x);

/**
 * @brief
 *  Allocates and initializes hash table.
 *  All data fields are set to 0 and linked lists are initialized as NULL.
 *
 * @param hash_table Pointer to hash table pointer
 * @param hash_size Size of hash table
 */
void define_hash_table(Node **hash_table, int hash_size);

/**
 * @brief
 *  Inserts all elements into hash table using:
 *   - Linear probing up to k attempts
 *   - Chaining (linked list) if probing fails
 *
 * @param hash_table Hash table array
 * @param data_array Input data array
 * @param m Hash table size
 * @param x Number of input elements
 * @param k Maximum probing count
 */
void insert_hash_table(Node *hash_table, int *data_array, int m, int x, int k);

/**
 * @brief
 *  Searches all elements in the hash table.
 *  First performs linear probing up to k attempts,
 *  then searches linked list if necessary.
 *
 * @param hash_table Hash table array
 * @param data_array Input data array
 * @param m Hash table size
 * @param x Number of input elements
 * @param k Maximum probing count
 * @param try_count_array Array storing probing counts for each element
 */
void search_hash_table(Node *hash_table, int *data_array, int m, int x, int k, int *try_count_array);

/**
 * @brief
 *  Writes the complete hash table structure to a text file.
 *  Includes linear slots and chained linked lists.
 *
 * @param hash_table Hash table array
 * @param hash_size Hash table size
 * @param output_path Output file path
 */
void print_hash_table(Node *hash_table, int hash_size, const char *output_path);

/**
 * @brief
 *  Writes probing attempt counts for each input key to a text file.
 *
 * @param data_array Input data array
 * @param try_count_array Probing count array
 * @param x Number of input elements
 * @param output_path Output file path
 */
void print_try_count_array(int *data_array, int *try_count_array, int x, const char *output_path);

/**
 * @brief
 *  Computes and prints maximum and average probing counts.
 *
 * @param try_count_array Probing count array
 * @param x Number of input elements
 */
void analise_try_count_array(int *try_count_array, int x);

/**
 * @brief
 *  Hash function using modulo operation.
 *
 * @param t Input key
 * @param m Hash table size
 *
 * @return Hash index
 */
int h(int t, int m);

int main()
{
#if GET_CUSTOM_DB
    char file_path[20] = TXT_PATH_CUSTOM;
#else
    char file_path[20] = TXT_PATH;
#endif
    int x, hash_size;
    int *data_array = NULL, *try_count_array = NULL;
    Node *hash_table = NULL;

    int k;

    printf("Enter the probing count (k):");
    scanf("%d", &k);

    printf("Probing count  = %d \n", k);
    if (read_txt(file_path, &x, &data_array))
        return 1;

    printf("File Path: '%s' \n", file_path);

    try_count_array = (int *)calloc(x, sizeof(int));

    hash_size = set_hash_size(x);

    define_hash_table(&hash_table, hash_size);

    insert_hash_table(hash_table, data_array, hash_size, x, k);

    search_hash_table(hash_table, data_array, hash_size, x, k, try_count_array);

    analise_try_count_array(try_count_array, x);

#if DEBUG
    char hash_path[50];
    char try_path[50];

#if !GET_CUSTOM_DB
    sprintf(hash_path, "hash_table_k_%d.txt", k);
    sprintf(try_path, "try_array_k_%d.txt", k);
#else
    sprintf(hash_path, "hash_table_k_%d_custom.txt", k);
    sprintf(try_path, "try_array_k_%d_custom.txt", k);
#endif

    print_hash_table(hash_table, hash_size, hash_path);
    print_try_count_array(data_array, try_count_array, x, try_path);
#endif

    free(data_array);
    free(hash_table);
}

// ================== FUNCTIONS =================== //

int read_txt(char *file_path, int *x, int **data_array)
{
    int i = 0;
    FILE *fp;
    fp = fopen(file_path, "r");

    if (fp == NULL)
    {
        printf("FILE READ ERROR !!!");
        return 1;
    }
    fscanf(fp, "%d", x);
#if DEBUG
    printf("Dosya Boyutu: %d\n", *x);
#endif

    *data_array = (int *)calloc(*x, sizeof(int));

    for (int i = 0; i < *x; i++)
    {
        if (fscanf(fp, "%d", &(*data_array)[i]) != 1)
        {
            printf("Veri okuma hatasi: index=%d\n", i);
            free(*data_array);
            *data_array = NULL;
            fclose(fp);
            return 1;
        }
    }

#if DEBUG
    int a = 5;
    printf("Data: \n");
    for (i = 0; i < a; i++)
    {
        printf("%d- %d\n", i, (*data_array)[i]);
    }
    printf("...\n");
    for (i = *x - a; i < *x; i++)
    {
        printf("%d- %d\n", i, (*data_array)[i]);
    }
#endif

    fclose(fp);

    return 0;
}

int is_prime(int nbr)
{

    if (nbr < 2)
        return 0;
    for (int i = 2; i * i <= nbr; i++)
        if (nbr % i == 0)
            return 0;
    return 1;
}

int find_nearest_prime(int nbr)
{
    if (nbr < 2)
        return 2;

    if (is_prime(nbr))
        return nbr;

    for (int i = nbr + 1;; i++)
        if (is_prime(i))
            return i;
}

int set_hash_size(int x)
{
#if DEBUG
    printf("Hash table size: %d\n", find_nearest_prime(x / 10));
#endif
    return (find_nearest_prime(x / 10));
}

void define_hash_table(Node **hash_table, int hash_size)
{
    *hash_table = (Node *)calloc(hash_size, sizeof(Node));
    int i;
    for (i = 0; i < hash_size; i++) // kuyruğun NULL olarak kalması için NULL olarak initialize ediyoruz
    {
        (*hash_table)[i].data = 0;
        (*hash_table)[i].next = NULL;
    }
}

void insert_hash_table(Node *hash_table, int *data_array, int m, int x, int k)
{
    int i, insert_flag;
    int index, probe;
    int ctr;
    for (i = 0; i < x; i++) // insert all 1.5m data
    {
        ctr = 0;
        insert_flag = 0;

        index = h(data_array[i], m);
        while (ctr < k && insert_flag == 0) // prob deneme sayısını aşana kadar veya insert olana kadar deniyor
        {
            probe = (index + ctr) % m;
            if (hash_table[probe].data == 0) // boş yer bulursa insert et
            {
                hash_table[probe].data = data_array[i];
                insert_flag = 1;
                // printf("%d/%d -> %d (%d)\n", i, index, probe, ctr);
            }
            else
                ctr++;
        }

        if (insert_flag == 0) // eğer insert edemezse kuyruğa ekliyor
        {
            Node *tmp_node = (Node *)malloc(sizeof(Node));

            tmp_node->data = data_array[i];

            tmp_node->next = hash_table[index].next;
            hash_table[index].next = tmp_node;
            ctr++;
            // printf("%d/%d -> linked (%d)\n", i, index, ctr);
        }
    }
}

int h(int t, int m)
{
    return (t % m); // hash func
}

void search_hash_table(Node *hash_table, int *data_array, int m, int x, int k, int *try_count_array)
{
    int i, ctr, index, find_flag;
    int key, probe;

    for (i = 0; i < x; i++) // Search all elements of data array
    {
        ctr = 0;
        find_flag = 0;
        key = data_array[i];
        index = h(data_array[i], m);

        while (ctr < k && find_flag == 0) // max probing sayısı kadar linear search
        {
            probe = (index + ctr) % m;

            if (hash_table[probe].data == key) // eğer bulursa deneme sayısını kaydet
            {
                find_flag = 1;
                try_count_array[i] = ctr;
            }
            ctr++;
        }

        if (find_flag == 0) // linear search şile bulamazsa linked list yapısında ara
        {
            Node *current_node = hash_table[index].next;

            while (current_node != NULL && find_flag == 0) // bulana kadar next next diye git
            {
                if (current_node->data == key)
                {
                    try_count_array[i] = ctr;
                    find_flag = 1;
                }
                ctr++;
                current_node = current_node->next;
            }

            if (find_flag == 0) // bulamazsan debug için -1 olarak işaretle
            {
                try_count_array[i] = -1;
            }
        }
    }
}

void print_hash_table(Node *hash_table, int hash_size, const char *output_path)
{
    FILE *fp = fopen(output_path, "w");
    if (fp == NULL)
    {
        printf("Hash table output file error!\n");
        return;
    }

    for (int i = 0; i < hash_size; i++)
    {
        fprintf(fp, "%d: ", i);

        if (hash_table[i].data == 0 && hash_table[i].next == NULL)
        {
            fprintf(fp, "EMPTY\n");
            continue;
        }

        if (hash_table[i].data != 0)
            fprintf(fp, "%d", hash_table[i].data);

        Node *curr = hash_table[i].next;
        while (curr != NULL)
        {
            fprintf(fp, " -> %d", curr->data);
            curr = curr->next;
        }

        fprintf(fp, "\n");
    }

    fclose(fp);
}

void print_try_count_array(int *data_array, int *try_count_array, int x, const char *output_path)
{
    FILE *fp = fopen(output_path, "w");
    if (fp == NULL)
    {
        printf("Try count output file error!\n");
        return;
    }

    fprintf(fp, "# index key try_count\n");

    for (int i = 0; i < x; i++)
    {
        fprintf(fp, "%d %d %d\n",
                i,
                data_array[i],
                try_count_array[i]);
    }

    fclose(fp);
}

void analise_try_count_array(int *try_count_array, int x)
{
    int i;
    int max = try_count_array[0];
    int average = 0;

    for (i = 0; i < x; i++) // maxı ve ortalamayı bul
    {
        average += try_count_array[i];

        if (max < try_count_array[i])
        {
            max = try_count_array[i];
        }
    }
    average /= x;

    printf("Maximum probing = %d\n", max);
    printf("Average probing = %d\n", average);
}
