#include <stdio.h>
#include <stdlib.h>
/*
Cevap Yükleme Formu - Response Upload Form: https://l24.im/r95XvD


NOT: DOSYADAN OKUMA ISLEMI VE MAIN FONKSIYONU SIZE HAZIR OLARAK VERILMISTIR. 
SIZDEN SADECE findCourseDependencies FONKSIYONUNU YAZMANIZ BEKLENMEKTEDIR. 
DIGER FONKSIYONLARA DOKUNMAYIN.
*/
/*


NOTE: THE FILE READING OPERATION AND MAIN FUNCTION ARE PROVIDED. 
YOU ARE ONLY EXPECTED TO WRITE THE findCourseDependencies FUNCTION. 
DO NOT MODIFY OTHER FUNCTIONS.
*/



/*
@brief function that finds the order of courses that can be taken with given requirement conditions

@param n number of courses
@param dependencyMatrix a nxn matris that contains the course requirements. the row i is the requirements of course i

@return an array of length n which contains an order of courses that can be taken

*/

int* findCourseDependencies(int n, int** dependencyMatrix) {
    int *courseList=(int*) calloc (n,sizeof(int));
    
	//------------------------------- Cozumunuz ------------------------------- Solution ------------------------------- //
	int i=0, j=0, ctr=0;
	
	int* course_arr=(int*)calloc(n,sizeof(int));
	
	for(i=0;i<n;i++){
		course_arr[i] = n;
	}
	/*for(i=0;i<n;i++){
		courseMatrix[i]=(int*)calloc(n,sizeof(int));
	}*/
		
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			if(dependencyMatrix[i][j] == 1){
				course_arr[j] = course_arr[i] - 1;
				printf("course (%d:%d) %d = %d - 1 \n",i,j,course_arr[j],course_arr[i]);
			}
		}
	}
	
	for(i=0;i<n;i++){
		printf("course %d: %d\n",i,course_arr[i]);
	}
	
	
	
	//------------------------------- Cozumunuz ------------------------------- Solution ------------------------------- //
	return courseList;
}

//------------------------------- BURADAN SONRASINI DEGISTIRMEYINIZ ------------------------------- DO NOT CHANGE AFTER HERE ------------------------------- //

int main() {
    FILE *fp = fopen("kosul_matrisi.txt", "r");
    int i,j;
    if (!fp) {
        printf("Cannot open file.\n");
        return 1;
    }

    int n;
    fscanf(fp, "%d", &n);
	int** kosullar=(int**)malloc(n * sizeof(int*));
	for(i=0;i<n;i++){
		kosullar[i]=(int*)malloc(n * sizeof(int));
	}
	for(i=0;i<n;i++){
    	for(j=0;j<n;j++){
		fscanf(fp,"%d",&kosullar[i][j]);
	}
}
    

    fclose(fp);

    int* courseList=findCourseDependencies(n,kosullar);
    printf("Course Dependency:\n");
    for(i=0;i<n-1;i++)
		printf("%d->",courseList[i]);
	printf("%d",courseList[i]);
    
    for ( i = 0; i < n; i++) free(kosullar[i]);
    free(kosullar);
    free(courseList);

    return 0;
}
