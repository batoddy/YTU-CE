#include <stdio.h> 
#include <stdlib.h> 

typedef struct {
	char isim[20], soyisim[20];
	int numara;
	float puan;
}OGRENCI;

void dosyaOlustur(char* );	//verilen bir isimdeki dosyayý wb modda acar. Kayit sayisini ogrenir. Kayitlar icin yer alloce eder, bilgileri alir 
void dosyayaYaz(FILE*,int,OGRENCI*);	//kendisine gönderilen file*'a yine kendisine gönderilen bilgileri yazar
FILE * dosyaAc(char*,char*); //verilen isimdeki bir dosyayi istenen formatta açar ve file* döndürür
void dosyaListele(FILE *);  //kendisine gönderilen file 'daki kayýtlarý listeler

int main()
{
	FILE *fa,*fb,*fs;
	OGRENCI ogr1,ogr2;
	char dosyaA[30],dosyaB[30],dosyaC[30];
	printf("sirasiyla giris dosyalarinin ve cikis dosyanin adlarini veriniz\n");
	scanf("%s %s %s",dosyaA,dosyaB,dosyaC);
	dosyaOlustur(dosyaA);
	dosyaOlustur(dosyaB);
	fa=dosyaAc(dosyaA,"rb");
	fb=dosyaAc(dosyaB,"rb");
	fs=dosyaAc(dosyaC,"wb");
	fread(&ogr1,sizeof(OGRENCI),1,fa);
	fread(&ogr2,sizeof(OGRENCI),1,fb);
	
	while (!feof(fa) && !feof(fb))
	{
		if (ogr1.puan<ogr2.puan)
		{
			fwrite(&ogr2, sizeof(OGRENCI),1,fs);
			fread(&ogr2, sizeof(OGRENCI),1,fb);			
		}
		else
		{
			fwrite (&ogr1,sizeof(OGRENCI),1,fs);
			fread(&ogr1, sizeof(OGRENCI),1,fa);   			
		}
	}
	
	if (feof(fa))
	{
		while (!feof(fb))
		{
			fwrite (&ogr2,sizeof(OGRENCI),1,fs);
			fread(&ogr2,sizeof(OGRENCI),1,fb);
		}
	}
	else
	{
		while (!feof(fa))
		{
			fwrite (&ogr1,sizeof(OGRENCI),1,fs);
			fread(&ogr1,sizeof(OGRENCI),1,fa);
		}
	}

	fclose(fa);
	fclose(fb);
	fclose(fs);
	fs=dosyaAc(dosyaC,"rb");
	dosyaListele(fs);
	fclose(fs);
}

FILE* dosyaAc(char *dosya_ad, char mode[3])
{
	FILE *fs;
	fs=fopen (dosya_ad,mode);
	if (fs==NULL)
	{
		printf("dosya acilamadi\n");
		exit(0);
	}
	return fs;
}
void dosyayaYaz(FILE* fs, int n, OGRENCI* data)
{
	fwrite (data,sizeof(OGRENCI),n,fs);
}

void dosyaOlustur(char* dosya_ad)
{
	int n,i;
	OGRENCI ogr, *ogrDizi;
	FILE *fs;
	fs=dosyaAc(dosya_ad,"wb");
	printf("kitapcikta kac ogrenci var\n");
	scanf("%d",&n);
	ogrDizi=(OGRENCI*) calloc (n, sizeof(OGRENCI));
	printf("azalan puan sirasi ile ogrenci bilgilerini veriniz (isim,soyisim,numara,puan)\n");
	for (i=0;i<n;i++)
		scanf("%s %s %d %f",ogrDizi[i].isim,ogrDizi[i].soyisim,&ogrDizi[i].numara,&ogrDizi[i].puan);

	dosyayaYaz(fs,n,ogrDizi);
	fclose(fs);
	free(ogrDizi);
}
	
void dosyaListele(FILE *fs)
{
	OGRENCI ogr;
	fread(&ogr,sizeof(OGRENCI),1,fs);
	while (!feof (fs))
	{
		printf("%s %s %d %f\n",ogr.isim,ogr.soyisim,ogr.numara,ogr.puan);
		fread(&ogr,sizeof(OGRENCI),1,fs);
	}
}

