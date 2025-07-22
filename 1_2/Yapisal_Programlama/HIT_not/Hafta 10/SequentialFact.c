#include <stdio.h>
int Recurse(int n)
{
	int i,fact=1;
	for (i=1;i<=n;i++)
		fact=fact*i;
	return fact;
}
int main( )
{ 
	int say=4;
	printf("%d",Recurse(say));
	return 0;
} 
