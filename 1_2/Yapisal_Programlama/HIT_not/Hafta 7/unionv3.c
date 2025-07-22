#include <stdio.h>
union u
{
	struct{
	char f1,f2;
	short f3;	
	}s;
	unsigned char f4[6];
	// f1 		f2 		f3
	// f4[0]	f4[1]	f4[2]....
};

int main()
{
	static char* pt=0;
	// initialization of unions
	// union i�inde dizi ve struct'in yerlerini de�i�tirirsek a�a��daki kod dizinin t�m elemanlar�n� doldurur
//	union u test={1,2,3,4,5,6};	//struct dolar , dizinin ilk 3 eleman� dolar -> 1 2 3        1 2 3 0 0 0
union u test={.f4={1,2,3,4,5,6}}; //struct dolar , dizinin t�m elemanlar� dolar -> 1 2 1027           1 2 3 4 5 6
	//  00000100 00000011
	//		4		3
	//little endian: least-significant byte at the smallest address
	//Intel CPUs are little-endian, while Motorola 680x0 CPUs are big-endian
	printf("%d %d %d\n",test.s.f1,test.s.f2,test.s.f3);
	printf("%d %d %d %d %d %d\n",test.f4[0],test.f4[1],test.f4[2],test.f4[3],test.f4[4],test.f4[5]);
	return 0;
}
