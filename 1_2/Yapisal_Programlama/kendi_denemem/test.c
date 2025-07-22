
#include <stdio.h>
#include <string.h>

int main()
{
    FILE *fp;
    char write1[20] = "Hello World\n";
    char write2[20] = "Fuck You World\n";
    char read1[20], read2[20];
    fp = fopen("file.txt", "w");

    fputs(write1, fp);
    // fwrite(write2, sizeof(write2), 1, fp);

    fclose(fp);

    fopen("file.txt", "r");

    fgets(read1, 20, fp);

    fseek(fp, 0, SEEK_SET);
    fgets(read2, 20, fp);

    // fread(read2, 20, 1, fp);

    fclose(fp);

    printf("%s", read1);
    printf("%s", read2);
    return 0;
}