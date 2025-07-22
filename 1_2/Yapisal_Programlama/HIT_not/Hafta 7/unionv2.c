#include <stdio.h>
union doub
{
	unsigned char c[2];
	unsigned int val;
};

union u
{
	struct{
	char f1,f2;
	short f3;	
	}s;
	unsigned char f4[6];
	
};

int main()
{
	int j,i;
	char ch;
	union doub d;
	// int ii:4;    b�yle bir bit field olmaz
	for (j=0;j<2;j++)
		scanf("%d",&d.c[j]);		
	
	printf("val1:%d\n",d.val);
	printf("c1_1:%d c2_1:%d\n",d.c[0],d.c[1]);
	
	d.c[0]=0;
	
	printf("val2:%d\n",d.val);
	printf("c1_2:%d c2_2:%d\n",d.c[0],d.c[1]);
	printf("ad1:%p ad2:%p\n",&d.c[0],&d.c[1]);
	
	int s=1,f;
	s=d.val;
	f=s>>1;
	printf("f:%d",f);
	
	d.c[0]=1;
	d.c[0]=1;
	printf("val2:%d\n",d.val);
	
	union u test={1,2,3,4,5,6};	//struct dolar , dizinin ilk 3 eleman� dolar -> 1 2 3        1 2 3 0 0 0
	//union u test={.f4={1,2,3,4,5,6}}; //struct dolar , dizinin t�m elemanlar� dolar -> 1 2 1027           1 2 3 4 5 6
	
	printf("%d %d %d\n",test.s.f1,test.s.f2,test.s.f3);
	printf("%d %d %d %d %d %d\n",test.f4[0],test.f4[1],test.f4[2],test.f4[3],test.f4[4],test.f4[5]);
	return 0;
}
