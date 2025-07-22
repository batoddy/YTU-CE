#include <stdio.h>
#include <stdint.h>

int main()
{
    int64_t s1, s2;
    int64_t ctr = 0;

    s1 = 0x99999999;
    s2 = 0x1;

    while (s2 < 0xFFFFFFFF)
    {
        if ((s1 & s2) == s2)
        {
            ctr++;
        }
        s2 = s2 << 1;
        printf("s2:%X -- ctr:%d\n", s2, ctr);
    }

    printf("%X --> %d\n", s1, ctr);
}