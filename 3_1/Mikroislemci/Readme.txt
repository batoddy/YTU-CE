MOUNT C C:\<kodların bulunduğu dır>

DIR DENE* 		-> Adının başında DENE olan dosyaları getirir

EDIT TANITIM1.ASM 	-> Yeni dosya açar

MASM TANITIM1.ASM 	-> .obj dosyasına çevirme

LINK TANITIM1.OBJ 	-> .exe executable dosyaya çevirir

TANITIM1 		-> TANITIM1 dosyasını çalıştırır

DEL TANITIM1 		-> TANITIM1 dosyasını siler.

DEBUG TANITIM1.EXE	-> Debug işlemi
-U 	-> Unassemble
-R	-> Register
-T	-> Trace
-D DS:0 -> Data Segmentini 0dan itibaren göster (SS: Stack Segment, DS: Data Segment, CS: Code Segment)
-G 	-> Terminate remaining steps
-Q	-> Quit debug
 


VSCode MASM/TASM eklentisi author: clcxsrolau
