#include <stdio.h>
int main()
{
char *aylar[12]={"ocak","subat","mart","nisan","mayis","haziran","temmuz","agustos","eylul","ekim","kasim","aralik"};
int n,i;
printf("Bir rakam veriniz\n");
scanf("%d",&n);
if ((n>12) || (n<1))
{
	printf ("yanlis giris");
	return 0;
}
else
	printf("%s ",aylar[n-1]);

printf("----------------------------------------------------\n");
for (i=0;i<12;i++)
	printf("%p adresinde %s var\n",&aylar[i],aylar[i]);


return 0;
}
