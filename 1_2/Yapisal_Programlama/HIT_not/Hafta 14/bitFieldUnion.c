// BIT FIELDS
// store gender, age and name of people using bitfiels
// allocate space for n people ,get information from user 
// write a function that returns the id and age of oldest woman
// write a function that calculates the risk factor by (riskFactor=age-ageThr)*2;

#include <stdio.h>
#include <stdlib.h>
typedef union
{
	struct{
	unsigned int gender:1;	//0-1
	unsigned int age:7;		//0-127 :)
	}health;
	unsigned char riskFactor;
}PERSON;

int findOldestWoman(PERSON*, int, int*);
int main()
{
	int gender_t,age_t,i,n,maxAge=0;
	PERSON *personArray;
	printf("give the number of the people\n");
	scanf("%d",&n);
	personArray=(PERSON *)calloc(n, sizeof(PERSON));
	for (i=0;i<n;i++)
	{
		printf("give gender (0-male, 1-female) and age\n");
		scanf("%d %d",&gender_t,&age_t);
		personArray[i].health.gender=gender_t;
		personArray[i].health.age=age_t;
	}
	//calculateRisk
	printf("no:%d \n", findOldestWoman(personArray,n,&maxAge));
	printf("age:%d \n",maxAge);
    calculateRisk(personArray,n,45) ;
	for (i=0;i<n;i++)
		printf("%d  %d\n", i, personArray[i].riskFactor);
		

	return 0;
}

int findOldestWoman(PERSON *array, int n, int* max)
{
	int i,loc;
	for (i=0;i<n;i++)
	{
		if ((array[i].health.gender==1) && (array[i].health.age>*max))
		{
			*max=array[i].health.age;
			loc=i;
		}
	}
	return loc;
}

calculateRisk(PERSON *array, int n, int ageThr)
{
	int i,loc;
	for (i=0;i<n;i++)
	{
		if ((array[i].health.gender==1) && (array[i].health.age>ageThr))
		{
			array[i].riskFactor=(array[i].health.age-ageThr)*2;
		}
	}
}
