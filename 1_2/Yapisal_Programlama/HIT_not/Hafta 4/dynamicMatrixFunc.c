// MATRIX IN FUNCTION
#include <stdio.h>
#include <stdlib.h>
float toplaMatv1(float **, int, int);

int main()
{
	int row, column, i, j;
	float **matrix, *p;
	printf("1-9 araliginda olmak uzere satir ve sutun sayisini veriniz\n");
	scanf("%d %d", &row, &column);
	matrix = (float **)calloc(row, sizeof(float *));
	printf("adress of matrix:%p adress in matrix:%p\n", &matrix, matrix);
	for (i = 0; i < row; i++)
		matrix[i] = (float *)calloc(column, sizeof(float));

	for (i = 0; i < row; i++)
		for (j = 0; j < column; j++)
			matrix[i][j] = i + (float)j / 10;

	for (i = 0; i < row; i++)
	{
		for (j = 0; j < column; j++)
			printf("%f", matrix[i][j]);
		printf("\n");
	}
	printf("toplamv1:%f\n", toplaMatv1(matrix, row, column));
	// cast yapmasaniz da calisir ancak warning alirsiniz
	return 0;
}

float toplaMatv1(float **mat, int dim1, int dim2)
{
	float topla = 0;
	int i, j;
	for (i = 0; i < dim1; i++)
		for (j = 0; j < dim2; j++)
			// topla=topla+*((float *)mat+i*dim2+j); //illegal kullanim i ve i+1. sat�r elemanlar� ard���k de�il
			// topla=topla+*(*(mat+i)+j);  			//legal
			topla = topla + mat[i][j];
	return topla;
}
