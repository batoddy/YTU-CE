#include <stdio.h>
//5 kom�uluk ba�lant�s� olan yollar, 1.sutundan ba�lamaktad�r
//Ayn� yol ayn� numara ile i�aretlidir
//Son sutuna ula�abilen en uzun yolun uzunlugu ve numaras� nedir?
int yolBilgisiGetir(char (*)[10],int, int, int , int *); //Verilen bir yolid icin o yol sonuca ulasirmi ve uzunlugu ne bulur
//matrisi, yolun hangi sat�rdan ba�lad���n� al�r.uzunlu�u olu�turur. yol sonuca ula�t�ysa id, de�ilse 0 dondurur

int adimAt(char (*)[10],int , int, int,int *, int *); //Atiabilecek ad�m� kontrol eder ve lokasyonu gunceller
// yolbilgisiGetir i�erisinde her adim adimAt fonksiyonu ile at�l�r
//bu fonksiyon matrisi, takip edilecek yol id'yi ve bulunulan lokasyonu al�r ve ad�m at�labiliyorsa lokasyonu gunceller
//ad�m atabiliryorsa id, aksi takdirde 0 dondurur

int main()
{
	char yollar[8][10]=			{	{0, 0, 0, 0, 0, 2, 0, 0, 2, 2},
									{1, 0, 0, 1, 2, 0, 2, 2, 0, 0},
									{0, 1, 1, 2, 1, 0, 0, 1, 1, 1},
									{0, 0, 0, 2, 0, 1, 1, 3, 3, 0},
									{2, 2, 0, 2, 0, 0, 0, 3, 0, 0},
									{0, 0, 2, 0, 3, 0, 0, 3, 0, 0},
									{3, 3, 0, 3, 0, 3, 0, 3, 0, 0},
									{0, 0, 3, 3, 0, 0, 3, 0, 0, 0}};
						
	int sonuc,uzunluk=0,max=0,i;
	char yolId;
	for (i=0;i<8;i++)
	{
		//Tum sat�rlar�n ilk kolonuna bak�yoruz eger bir yol varsa
		if (yollar[i][0]!=0)
		{
			//yolBilgisiGetir fonksiyonu cagriliyor
			//bu fonksiyon eger yol sona ula�m��sa yol id sini aksi takdirde 0 dondurur
			//yol id'si donmusse uzunluk yolun uzunlugunu tutuyordur
			sonuc=yolBilgisiGetir(yollar,8,10,i,&uzunluk);
			
			if ((sonuc!=0) && (uzunluk>max)) 
			{
				max=uzunluk;
				yolId=sonuc;	
			}
		}
	}
		printf("\nEn uzun yol no:%d uzunlugu:%d\n",yolId,max);
		
	
}

//Verilen bir yolid icin o yol sonuca ulasirmi ve uzunlugu ne bulur
int yolBilgisiGetir(char yollar[][10],int boy, int en, int baslangic, int *uzunluk)
{
	int i=baslangic, j=0, id=yollar[baslangic][0], adimVarmi;
	(*uzunluk)=0;
    do{
		//her adim adimAt fonksiyonu ile at�l�r
		//bu fonksiyon bulunulan lokasyonu al�r ve ad�m at�labiliyorsa lokasyonu gunceller
		//aksi takdirde 0 dondurur
		adimVarmi=adimAt(yollar,boy,en,id,&i, &j);
		(*uzunluk)++;
		
	}while (adimVarmi!=0);

	if (j==(en-1)) return id;
	return 0;
}

//Atiabilecek ad�m� kontrol eder ve lokasyonu gunceller
int adimAt(char yollar[][10],int boy, int en, int id,int *i, int *j)
{
	
	//gectigimiz yolu 0 yapalim
	yollar[(*i)][(*j)]=0;
	if (((*i)-1>=0) 	&&  (yollar[(*i)-1][(*j)]==id))				{ (*i)--;  return id;}
	if (((*i)+1<boy) 	&&  (yollar[(*i)+1][(*j)]==id))				{ (*i)++;  return id;}
	if (((*j)+1<en) 	&&  (yollar[(*i)][(*j)+1]==id))				{ (*j)++;  return id;}
	if (((*i)-1>=0) 	&& ((*j)+1<en) &&  (yollar[(*i)-1][(*j)+1]==id))	{ (*i)--; (*j)++;  return id; }
	if (((*i)+1<boy) 	&& ((*j)+1<en) &&  (yollar[(*i)+1][(*j)+1]==id))	{ (*i)++; (*j)++;  return id; }
	return 0;
}

