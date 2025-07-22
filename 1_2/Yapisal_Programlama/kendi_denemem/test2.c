#include <stdio.h>

void write_file(char *file_name, char *str);
void read_file(char *file_name, char *str);

int main()
{
    char write[20] = "Hello World";
    char read[20];

    FILE *fp;
    fp = fopen("file_name.txt", "w");
    fclose(fp);

    write_file("file_name.txt", write);
    read_file("file_name.txt", read);
    printf("%s", read);
}

void write_file(char *file_name, char *str)
{
    FILE *fp;
    fp = fopen(file_name, "w");
    fprintf(fp, "%s", str);
    // fputs(str, fp);
    fclose(fp);
}

void read_file(char *file_name, char *str)
{
    FILE *fp;
    fp = fopen(file_name, "r");
    fscanf(fp, "%s", str);
    // fgets(str, 20, fp);
    fclose(fp);
}