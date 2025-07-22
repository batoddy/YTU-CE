#include <stdio.h>
#include <stdlib.h>
void ccl(int, int, int ,int **,int ,int ,int **);

void main() {
 
  int **m, **label;
  int row_count;
  int col_count;
  int component = 1;
  int i,j;
 
  printf("Matris boyutlari\n");
  scanf("%d %d",&row_count,&col_count);
  
  m=(int **) calloc (row_count,sizeof(int*));
  label=(int **) calloc (row_count,sizeof(int*));
  for (i=0;i<row_count;i++)
  {
  	m[i]=(int*) calloc (col_count, sizeof(int));
  	label[i]=(int*) calloc (col_count, sizeof(int));
  }

 printf("Matris elemanlari\n");
 for (i = 0; i < row_count; i++) 
    for (j = 0; j < col_count; j++) 
		scanf("%d",&m[i][j]);
   
  for (i = 0; i < row_count; i++) 
    for (j = 0; j < col_count; j++) 
      if (!label[i][j] && m[i][j]) ccl(i, j, component++, m,row_count,col_count,label);
   
   for (i = 0; i < row_count; i++) 
   {
    	for (j = 0; j < col_count; j++) 
    		printf("%d ",label[i][j]);
     	printf("\n");
	}
}

void ccl(int x, int y, int current_label,int **m,int row_count,int col_count,int **label) {

  	if (x < 0 || x == row_count) return; // out of bounds
  	if (y < 0 || y == col_count) return; // out of bounds
  	if (label[x][y] || !m[x][y]) return; // already labeled or not marked with 1 in m

	label[x][y] = current_label;
	ccl(x + 1, y + 0, current_label,m,row_count,col_count,label); 	ccl(x + 0, y + 1, current_label,m,row_count,col_count,label);
	ccl(x - 1, y + 0, current_label,m,row_count,col_count,label);   ccl(x + 0, y - 1, current_label,m,row_count,col_count,label);	
	
	// mark the current cell
	//*((int *)label+x*col_count+y)=current_label; //dynamic memory alloc ile ayrýlan yerde elemanalar iteratif yerleþmediginden bu kullaným runtime hatasi verir
	
	// dört komsuluga ulasmannin daha guzel bir yolu 
	/*int direction;
	int dx[] = {+1, 0, -1, 0};
	int dy[] = {0, +1, 0, -1};
	for (direction = 0; direction < 4; ++direction)
		ccl(x + dx[direction], y + dy[direction], current_label,m,row_count,col_count,label);*/

}
