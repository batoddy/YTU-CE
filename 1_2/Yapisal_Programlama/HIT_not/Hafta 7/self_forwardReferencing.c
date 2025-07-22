/* FORWARD REFERENCING */
struct s1{
 	int a;
  	struct s2 *ps2;
}; 

struct s2{
 	int b;
  	struct s1 *ps1;
}; 
/* SELF REFERENCING */
struct Sr
{
	int a,b;
	float c;
	struct Sr* pointer_to_Sr;
};

// typedef ile forward veya self referencing yapilamaz
/*
typedef struct;
{
	int a,b;
	float c;
	Sr* pointer_to_Sr;
}Sr;
*/
///////////////////////////////
int main()
{
	struct s1 x;
	struct s2 y;
	
	x.a=1;		x.ps2=&y;
	
	y.b=2;		y.ps1=&x;
	
	printf("--%d--",(x.ps2)->b);	//2
	printf("--%d--",(*(y.ps1)).a); 	//1
// *(y.ps1).a yazilirsa hata verir   .'nin *'a gore onceligi daha yuksek
	
	struct Sr z,t;
	z.pointer_to_Sr=&t;
	return 0;
}

