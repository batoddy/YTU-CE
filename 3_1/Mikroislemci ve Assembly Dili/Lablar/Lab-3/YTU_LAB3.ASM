SSG SEGMENT PARA STACK 'STACK'
        DW 30 DUP(?)
SSG ENDS

DSG SEGMENT PARA 'DATA'
    is_prime_flag DW  8 DUP (2)
    fact_count    DW  8 DUP (0)
    max_factor    DW  8 DUP (0)
    factors       DW  64 DUP (0)
    number_list   DW  2, 162, 13, 997, 91, 60060, 1, 0
    number_count  DW  8

    CR            EQU 13
    LF            EQU 10
    HATA          DB  'HATA',0                            ; GETN içindeki hata mesajı için; kullanmıyorsan kalabilir
    TXT_HDR       DB  CR,LF,'--- RESULTS ---',CR,LF,0
    TXT_I         DB  'i=',0
    TXT_NUM       DB  '  num=',0
    TXT_PRIME     DB  '  prime=',0
    TXT_COUNT     DB  '  count=',0
    TXT_FACT      DB  '  factors:',0
    TXT_MAX       DB  '  max=',0


DSG ENDS

CSG SEGMENT PARA 'CODE'
                ASSUME CS:CSG, DS:DSG, SS:SSG
START PROC FAR

                PUSH   DS
                XOR    AX, AX
                PUSH   AX
                MOV    AX, DSG
                MOV    DS, AX

                CALL   READ_ARRAY                ; read 8 unsigned 16-bit decimals into number_list

                MOV    CX, 0
                LEA    SI, number_list

    MAIN_LOOP:  
                MOV    AX, [SI]                  ; Load number into AX
    
    
                CALL   IS_PRIME                  ; AX -> SAYI ; BX -> flag

                CMP    BX, 1
                JE     SKIP_PRIME
                CALL   FACTORIZE                 ; AX -> SAYI
    SKIP_PRIME: 
                ADD    SI, 2
                INC    CX
                CMP    CX, number_count
                JNE    MAIN_LOOP
    
                CALL   PRINT_ARRAY               ; print arrays simply

                RETF
START ENDP


IS_PRIME PROC                                    ; number -> AX
    ; asal ise is_prime_flag -> 1 ; değil ise is_prime_flag -> 0
                PUSH   AX
                PUSH   DX
                PUSH   CX
                PUSH   SI

                LEA    DI, is_prime_flag
                ADD    DI, CX
                ADD    DI, CX

                MOV    BX, AX
                CMP    BX,2
                JB     NOT_PRIME
                JE     PRIME
                    
    ; Çift mi değil mi kontorl
                MOV    AX,BX
                XOR    DX,DX
                MOV    CX, 2
                DIV    CX
                CMP    DX, 0
                JE     NOT_PRIME

    ; Çarpan kontorlü ( sayi/d < d'ye kadar )
                MOV    CX,3
    CHECK_LOOP: MOV    AX,BX
                XOR    DX,DX
                DIV    CX
                CMP    DX,0
                JE     NOT_PRIME

                ADD    CX,2
                MOV    AX,CX
                MUL    CX
                CMP    AX,BX
                JBE    CHECK_LOOP

    PRIME:      
                MOV    [DI], 1
                MOV    BX, 1

                JMP    DONE
        
    NOT_PRIME:  
                MOV    [DI], 0
                MOV    BX, 0


    DONE:       
                POP    SI
                POP    CX
                POP    DX
                POP    AX

                RET
IS_PRIME ENDP



FACTORIZE PROC                                   ; sayi data segmentten alınacak
                PUSH   AX
                PUSH   BX
                PUSH   DX
                PUSH   SI
                PUSH   DI
                PUSH   CX

                MOV    BX, AX
                MOV    AX , 16
                MUL    CX
                LEA    SI, factors
                ADD    SI, AX
                LEA    DI, fact_count
                ADD    DI, CX
                ADD    DI, CX


                CMP    BX, 2
                JB     FZ_DONE

    FZ_DIV2:    MOV    AX, BX
                XOR    DX,DX
                MOV    CX, 2
                DIV    CX
                    
                CMP    DX, 0
                JNE    FZ_AFTER2
                MOV    [SI], 2
                ADD    SI, 2
                ADD    [DI],1
                MOV    BX, AX
                JMP    FZ_DIV2


    FZ_AFTER2:                                   ; d*d <= N
                MOV    CX, 3
        
    FZ_ODD_LOOP:
                MOV    AX, BX
                XOR    DX,DX
                DIV    CX
                CMP    AX, CX
                JB     FZ_CHK_LAST

                CMP    DX , 0
                JNE    FZ_NEXT_ODD

                MOV    [SI],CX
                ADD    SI, 2
                ADD    [DI],1
                MOV    BX, AX
                JMP    FZ_ODD_LOOP

    FZ_NEXT_ODD:
                ADD    CX, 2
                JMP    FZ_ODD_LOOP

    FZ_CHK_LAST:
                CMP    BX, 1
                JBE    FZ_DONE
                    
                MOV    [SI], BX
                ADD    SI, 2
                ADD    WORD PTR [DI],1

    FZ_DONE:    
                SUB    SI, 2

                POP    CXx

                CMP    WORD PTR [DI], 0
                JE     MF_SET_ZERO

                MOV    AX, [SI]                  ; AX = son çarpan
                LEA    DI, max_factor
                ADD    DI, CX
                ADD    DI, CX
                MOV    [DI], AX
                JMP    MF_POP_TAIL

    MF_SET_ZERO:
                LEA    DI, max_factor
                ADD    DI, CX
                ADD    DI, CX
                MOV    WORD PTR [DI], 0

    MF_POP_TAIL:
                POP    DI
                POP    SI
                POP    DX
                POP    BX
                POP    AX
                RET
FACTORIZE ENDP

    ; MAX_FACTOR PROC
    ;
    ;                 RET
    ; MAX_FACTOR ENDP

PRINT_ARRAY PROC NEAR
                PUSH   AX
                PUSH   BX
                PUSH   CX
                PUSH   DX
                PUSH   SI
                PUSH   DI

    ; Başlık
                MOV    AX, OFFSET TXT_HDR
                CALL   PUT_STR

                XOR    BX, BX                    ; i = 0..7
                MOV    CX, number_count

    PA_OUTER:   
    ; i=
                MOV    AX, OFFSET TXT_I
                CALL   PUT_STR
                MOV    AX, BX
                CALL   PUTN

    ; num= number_list[i]
                MOV    AX, OFFSET TXT_NUM
                CALL   PUT_STR
                MOV    DX, BX
                SHL    DX, 1                     ; 2*i
                LEA    SI, number_list
                ADD    SI, DX
                MOV    AX, WORD PTR [SI]
                CALL   PUTN

    ; prime= is_prime_flag[i]
                MOV    AX, OFFSET TXT_PRIME
                CALL   PUT_STR
                LEA    DI, is_prime_flag
                ADD    DI, DX
                MOV    AX, WORD PTR [DI]
                CALL   PUTN

    ; count= fact_count[i]
                MOV    AX, OFFSET TXT_COUNT
                CALL   PUT_STR
                LEA    DI, fact_count
                ADD    DI, DX
                MOV    AX, WORD PTR [DI]
                CALL   PUTN

    ; factors: factors[i][0..count-1]
                MOV    AX, OFFSET TXT_FACT
                CALL   PUT_STR

    ; factors tabanı: &factors + 16*i
                MOV    AX, 16
                MUL    BX                        ; DX:AX = 16*i
                LEA    SI, factors
                ADD    SI, AX

    ; count kadar faktör yaz
                PUSH   CX
                MOV    CX, WORD PTR [DI]         ; DI hâlâ fact_count[i]
                JCXZ   PA_EOL

    PA_FLOOP:   
                MOV    AX, WORD PTR [SI]
                CALL   PUTN
                MOV    AL, ' '
                CALL   PUTC
                ADD    SI, 2
                LOOP   PA_FLOOP

    PA_EOL:     
                MOV    AL, CR
                CALL   PUTC
                MOV    AL, LF
                CALL   PUTC
                POP    CX

                INC    BX
                LOOP   PA_OUTER

                POP    DI
                POP    SI
                POP    DX
                POP    CX
                POP    BX
                POP    AX
                RET
PRINT_ARRAY ENDP


READ_ARRAY PROC NEAR
                PUSH   SI
                PUSH   CX

                LEA    SI, number_list
                MOV    CX, number_count

    RA_LOOP:    
                CALL   GETN
                MOV    [SI], AX
                ADD    SI, 2
                LOOP   RA_LOOP

                POP    CX
                POP    SI
                RET
READ_ARRAY ENDP
    ; ==================== END READ_ARRAY =======================

GETC PROC NEAR
    ;------------------------------------------------------------------------
    ; Klavyeden basılan karakteri AL yazmacına alır ve ekranda gösterir.
    ; işlem sonucunda sadece AL etkilenir.
    ;------------------------------------------------------------------------
                MOV    AH, 1h
                INT    21H
                RET
GETC ENDP

PUTC PROC NEAR
    ;------------------------------------------------------------------------
    ; AL yazmacındaki değeri ekranda gösterir. DL ve AH değişiyor. AX ve DX
    ; yazmaçlarının değerleri korumak için PUSH/POP yapılır.
    ;------------------------------------------------------------------------
                PUSH   AX
                PUSH   DX
                MOV    DL, AL
                MOV    AH,2
                INT    21H
                POP    DX
                POP    AX
                RET
PUTC ENDP

GETN PROC NEAR
    ;------------------------------------------------------------------------
    ; Klavyeden basılan sayiyi okur, sonucu AX yazmacı üzerinden dondurur.
    ; DX: sayının işaretli olup/olmadığını belirler. 1 (+), -1 (-) demek
    ; BL: hane bilgisini tutar
    ; CX: okunan sayının islenmesi sırasındaki ara değeri tutar.
    ; AL: klavyeden okunan karakteri tutar (ASCII)
    ; AX zaten dönüş değeri olarak değişmek durumundadır. Ancak diğer
    ; yazmaçların önceki değerleri korunmalıdır.
    ;------------------------------------------------------------------------
                PUSH   BX
                PUSH   CX
                PUSH   DX
    GETN_START: 
                MOV    DX, 1                     ; sayının şimdilik + olduğunu varsayalım
                XOR    BX, BX                    ; okuma yapmadı Hane 0 olur.
                XOR    CX,CX                     ; ara toplam değeri de 0’dır.
    NEW:        
                CALL   GETC                      ; klavyeden ilk değeri AL’ye oku.
                CMP    AL,CR
                JE     FIN_READ                  ; Enter tuşuna basilmiş ise okuma biter
                CMP    AL, '-'                   ; AL ,'-' mi geldi ?
                JNE    CTRL_NUM                  ; gelen 0-9 arasında bir sayı mı?
    NEGATIVE:   
                MOV    DX, -1                    ; - basıldı ise sayı negatif, DX=-1 olur
                JMP    NEW                       ; yeni haneyi al
    CTRL_NUM:   
                CMP    AL, '0'                   ; sayının 0-9 arasında olduğunu kontrol et.
                JB     error
                CMP    AL, '9'
                JA     error                     ; değil ise HATA mesajı verilecek
                SUB    AL,'0'                    ; rakam alındı, haneyi toplama dâhil et
                MOV    BL, AL                    ; BL’ye okunan haneyi koy
                MOV    AX, 10                    ; Haneyi eklerken *10 yapılacak
                PUSH   DX                        ; MUL komutu DX’i bozar işaret için saklanmalı
                MUL    CX                        ; DX:AX = AX * CX
                POP    DX                        ; işareti geri al
                MOV    CX, AX                    ; CX deki ara değer *10 yapıldı
                ADD    CX, BX                    ; okunan haneyi ara değere ekle
                JMP    NEW                       ; klavyeden yeni basılan değeri al
    ERROR:      
                MOV    AX, OFFSET HATA
                CALL   PUT_STR                   ; HATA mesajını göster
                JMP    GETN_START                ; o ana kadar okunanları unut yeniden sayı almaya başla
    FIN_READ:   
                MOV    AX, CX                    ; sonuç AX üzerinden dönecek
                CMP    DX, 1                     ; İşarete göre sayıyı ayarlamak lazım
                JE     FIN_GETN
                NEG    AX                        ; AX = -AX
    FIN_GETN:   
                POP    DX
                POP    CX
                POP    DX
                RET
GETN ENDP

PUTN PROC NEAR
                PUSH   CX
                PUSH   DX
                PUSH   BX

    ; AX == 0 ise doğrudan '0' yaz
                CMP    AX, 0
                JNE    PU_DIV
                MOV    DL, '0'
                MOV    AH, 02h
                INT    21h
                JMP    PU_DONE

    PU_DIV:     
                XOR    CX, CX                    ; basılacak hane sayacı
    PU_LOOP:    
                XOR    DX, DX                    ; unsigned bölme: DX=0
                MOV    BX, 10
                DIV    BX                        ; AX = AX/10, DX = kalan
                ADD    DL, '0'                   ; kalan ASCII
                PUSH   DX                        ; yığına koy
                INC    CX
                TEST   AX, AX
                JNZ    PU_LOOP

    PU_OUT:     
                POP    DX
                MOV    AH, 02h
                INT    21h
                LOOP   PU_OUT

    PU_DONE:    
                POP    BX
                POP    DX
                POP    CX
                RET
PUTN ENDP

PUT_STR PROC NEAR
    ;------------------------------------------------------------------------
    ; AX de adresi verilen sonunda 0 olan dizgeyi karakter karakter yazdırır.
    ; BX dizgeye indis olarak kullanılır. Önceki değeri saklanmalıdır.
    ;------------------------------------------------------------------------
                PUSH   BX
                MOV    BX,	AX                    ; Adresi BX’e al
                MOV    AL, BYTE PTR [BX]         ; AL’de ilk karakter var
    PUT_LOOP:   
                CMP    AL,0
                JE     PUT_FIN                   ; 0 geldi ise dizge sona erdi demek
                CALL   PUTC                      ; AL’deki karakteri ekrana yazar
                INC    BX                        ; bir sonraki karaktere geç
                MOV    AL, BYTE PTR [BX]
                JMP    PUT_LOOP                  ; yazdırmaya devam
    PUT_FIN:    
                POP    BX
                RET
PUT_STR ENDP

CSG ENDS
 
     END START
