// MATRIX IN FUNCTION
#include <stdio.h>
int toplaMatv1(int **,int,int);
int toplaMatv2 (int [][3], int, int);

int main()
{
	int mat[2][3]={{0,1,2},{3,4,5}};
	printf("toplamv1:%d\n",toplaMatv1((int**)mat,2,3)); 
	//cast yapmasaniz da calisir ancak warning alirsiniz
	printf("toplamv2:%d\n",toplaMatv2(mat,2,3));
	
	
	return 0;
		
}


int toplaMatv1 (int **mat, int dim1, int dim2)
{
	int topla=0,i,j;
	for (i=0;i<dim1;i++)
		for (j=0;j<dim2;j++)
			topla=topla+*((int *)mat+i*dim2+j);
			//topla=topla+*(*(mat+i)+j);  illegal kullanim boyutlari bilinmeden scale yapilamaz
	return topla;	
}

int toplaMatv2 (int mat[][3], int dim1, int dim2)
{
	int topla=0,i,j;
	for (i=0;i<dim1;i++)
		for (j=0;j<dim2;j++)
			topla=topla+mat[i][j];
			
	printf("eleman 1-1:%d\n",*((int *)mat+1*dim2+1)); 	//legal
	printf("eleman 1-1:%d\n",*(*(mat+1)+1)); 			//legal. Kolon sayisi biliniyor
	
	return topla;	
}

