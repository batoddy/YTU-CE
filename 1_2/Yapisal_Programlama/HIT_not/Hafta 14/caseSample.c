int main()
{
	char answer[19];
	scanf("%s",answer);
	switch (answer[0]) 
	{ 
		case 'y': 
		case 'Y': printf("TRUE"); break; 
		case 'n':
 		case 'N': 
		return printf("false"); break;  
		default: printf( "Please answer Y or N\n");
	}
	return 0;
	
}

