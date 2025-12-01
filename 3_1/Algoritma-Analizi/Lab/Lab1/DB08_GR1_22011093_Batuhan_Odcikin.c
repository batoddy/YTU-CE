	#include<stdio.h>
/*
Bir restorantta ki müsteri sayilari ve müsterilerin mekana giris ve cikis vakitleri verilmistir. Sizden restoranta ayni anda bulunan maksimum müsteri sayisini bulmaniz istenmektedir. 

Orn:
Input.txt:
7 // musteri sayisi
5 8 // 1. musterinin giris ve cikis saatleri. musteri 5. saatte restoranta giris yapmistir ve 8. saatte cikis yapmistir. 5 ve 8 dahildir.
2 4
3 9
1 3
4 8
5 8
3 5

Output: 5
-------- ACIKLAMA -------
Saatlerin doluluk orani:
0 - 0 Musteri Sayisi
1 - 1 Musteri Sayisi
2 - 2 Musteri Sayisi
3 - 4 Musteri Sayisi
4 - 4 Musteri Sayisi
5 - 5 Musteri Sayisi -- Maksimum Musteri
6 - 4 Musteri Sayisi
7 - 4 Musteri Sayisi
8 - 4 Musteri Sayisi
9 - 1 Musteri Sayisi

NOT: DOSYADAN OKUMA ISLEMI VE MAIN FONKSIYONU SIZE HAZIR OLARAK VERILMISTIR. SIZDEN SADECE findBusiestCount FONKSIYONUNU YAZMANIZ BEKLENMEKTEDIR. DIGER FONKSIYONLARA DOKUNMAYIN.
*/

/*
The number of customers in a restaurant and the times they enter and exit the restaurant are given. You are asked to find the maximum number of customers in the restaurant at one time.
Exp:
Input.txt:
7 // number of customerss
5 8 // 1. Customer's check-in and check-out times. The customer entered the restaurant at 5 o'clock and checked out at 8 o'clock. 5 and 8 are included.
2 4
3 9
1 3
4 8
5 8
3 5

Output: 5
-------- Explanation -------
Occupancy rate of hours:
0 - 0 Customer Number
1 - 1 Customer Number
2 - 2 Customer Number
3 - 4 Customer Number
4 - 4 Customer Number
5 - 5 Customer Number -- Maximum number of Customers
6 - 4 Customer Number
7 - 4 Customer Number
8 - 4 Customer Number
9 - 1 Customer Number

NOTE: THE READING FROM FILE OPERATION AND THE MAIN FUNCTION ARE PROVIDED TO YOU READY-MADE. YOU ARE EXPECTED TO WRITE ONLY THE findBusiestCount FUNCTION. DO NOT TOUCH THE OTHER FUNCTIONS.
*/
/*
@brief function that finds the maximum number of customers at any time in a restaurant.

@param customerNumber number of total customers of all times
@param customerInfo customers entry and exit times. a matrix of (customerNumberx2) size. first column is the entry time, second column is the exit time

@return maximum number of customers at any time

*/
int findBusiestCount(int customerNumber,int** customerInfo){
int max_hours = 0;
int max_customer; // maximum counter
int max_hour; // maximum counter
int i,j; // temporary for loop variables

for(i = 0; i<customerNumber;i++){  
	if(max_hours < customerInfo[i][1]+1){
		max_hours = customerInfo[i][1]+1;
	}
}

int*gunArr = (int*)calloc(sizeof(int),max_hours);

// increment hours that customers stays by customer
for(i = 0;i<customerNumber;i++){ 
	for(j = customerInfo[i][0]; j<=customerInfo[i][1]; j++){
		gunArr[j]++;
	}
}

// print gun Arr for Debugging
printf("Day Array: "); 
for(i=0;i<max_hours;i++){
	printf("\n\t[Hour %d: %d customers]",i,gunArr[i]);
}
printf("\n");

// find max customer number and determine hour of the day
max_customer = gunArr[0];
for(i=0;i<max_hours;i++){
	if(gunArr[i] > max_customer){
		max_customer = gunArr[i];
		max_hour = i;
	}
}

return (max_hour); // return hour
}

//------------------------------- BURADAN SONRASINI DEGISTIRMEYINIZ ------------------------------- DO NOT CHANGE AFTER HERE ------------------------------- //


int* readFile(char* fileName,int* customerNumber){
int customerEntrance,customerExit;
int i;
int** customerInfo;
FILE *f=fopen(fileName,"r");
fscanf(f,"%d\n",customerNumber);
customerInfo=(int**)malloc((*customerNumber)*sizeof(int*));
for(i=0;i<*customerNumber;i++){
	fscanf(f,"%d %d\n",&customerEntrance,&customerExit);
	customerInfo[i]=(int*)malloc(2*sizeof(int));
	customerInfo[i][0]=customerEntrance;
	customerInfo[i][1]=customerExit;
}
return customerInfo;	
}





int main(){

int customerNumber;
int **customerInfo;
int i;

customerInfo=readFile("input.txt",&customerNumber);
printf("customer Number: %d\n",customerNumber);
for(i=0;i<customerNumber;i++){
	printf("For Customer %d-- Entrance: %d Exit: %d\n",i+1,customerInfo[i][0],customerInfo[i][1]);
}

int busiestCount=findBusiestCount(customerNumber,customerInfo);
printf("Output: %d",busiestCount);

	
}
