#include <stdio.h>
// Two simple functions
void fun1(int,int);
void fun2(int,int);
void (*wrapper(int,int))();


void fun1(int a,int b)
{
	printf("Fun1:%d\n",a-b);  //Quick Sort 
	
}

void fun2(int a,int b)
{ 
	printf("Fun2:%d\n",b-a); //Merge Sort
}

// wrapper() receives two integers and
// returns a function pointer

//void (*wrapper(int a,int b))(int a,int b)  //also legal
void (*wrapper(int a,int b))()
{
	if (a>b)		//the code detecting the optimum sorting algorithm
		return fun1;
	else
		return fun2;
}

// wrapper2() receives two integers as parameters
// and calls fun1() and fun2() functions
void wrapper2(int a,int b)
{
	if (a>b)		//the code detecting the optimum sorting algorithm
		fun1(a,b);
	else
		fun2(a,b);
}

int main()
{
	wrapper(17,12)(10,20);					//legal
	(*wrapper(17,12))(10,20);				//legal
	wrapper2(10,20);
	return 0;
}
