#include <stdio.h>
int main(int argc, char *argv[])
{
	int j = 5, k = 5.5, m = -4, x;
	unsigned int i = -1;
	float y = 8, z;
	char a = '3', b = '6';

	printf("%c %d %c\n", (b - a) + b, (b - a) + b, b);
	z = -4 / y + k;
	printf("%f\n", z);
	x = y + 8 / m;
	printf("%d\n", x);
	printf("%d\n", (++m) - (j--));

	--argc;
	while (argc >= 1)
	{
		printf("%s ", *(argv + argc));
		--argc;
	}
}
