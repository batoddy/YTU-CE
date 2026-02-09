/* ============================================================
   LAB-7 CHEAT SHEET
   KONU: PL/pgSQL Alias, Record/Cursor ve Trigger Tanımları
   ============================================================ */


/* ============================================================
   0. GEÇEN HAFTADAN HATIRLATMA + ALIAS/STRING QUOTE DETAYI
   ============================================================ */

-- ÖRNEK-1 (Geçen haftadan):
-- Dikkat: Fonksiyon gövdesinde tek tırnak (') kullanıldığı için
-- fonksiyon gövdesini '...' ile yazarsak hata alabiliriz.
-- Bu yüzden $$ ... $$ kullanımı güvenlidir.

CREATE OR REPLACE FUNCTION ornek1(num1 NUMERIC, num2 NUMERIC)
RETURNS NUMERIC AS $$
DECLARE
    toplam NUMERIC;
BEGIN
    toplam := num1 + num2;

    -- RAISE NOTICE ile parametreleri yazdırma (format: %)
    RAISE NOTICE 'sayi1:% , sayi2:%', num1, num2;

    RETURN toplam;
END;
$$ LANGUAGE plpgsql;


/* ============================================================
   1. RAISE (BİLGİLENDİRME / HATA MESAJI)
   ============================================================ */

-- Kullanım:
-- RAISE mesaj_turu 'MESAJ';
-- RAISE NOTICE 'Bilgilendirme';
-- RAISE EXCEPTION 'Hata Mesajı';
-- RAISE NOTICE 'Salary here is %', sal_variable;

-- Mesaj türleri:
-- DEBUG, LOG, INFO, NOTICE, WARNING, EXCEPTION

DO $$
DECLARE
    sal_variable NUMERIC := 12345;
BEGIN
    RAISE DEBUG 'debug mesaj';
    RAISE LOG 'log mesaj';
    RAISE INFO 'info mesaj';
    RAISE NOTICE 'notice mesaj';
    RAISE WARNING 'warning mesaj';
    RAISE NOTICE 'Salary here is %', sal_variable;
    -- RAISE EXCEPTION 'exception mesaj'; -- uncomment edersen hata fırlatır
END;
$$ LANGUAGE plpgsql;


/* ============================================================
   2. RECORD / TYPE (COMPOSITE TYPE) TANIMLAMA
   ============================================================ */

-- PL/pgSQL fonksiyonları tek değer döndürmek zorunda değildir.
-- Karmaşık sonuçlar / tablo benzeri kayıtlar döndürülebilir.
-- Composite veri tipleri için RECORD / TYPE tanımları kullanılır.

-- Genel şablon:
-- CREATE TYPE tur_ismi AS (alan1 tip1, alan2 tip2, ...);

-- Örnek (sunum):
CREATE TYPE urunler AS (miktar1 INTEGER, miktar2 INTEGER);

-- Bu türde değişken:
-- depo URUNLER;
-- (SQL/PLpgSQL case-sensitive değil; burada kavram gösterimi var)


/* ============================================================
   3. YARDIMCI SQL ÖRNEĞİ (FUNCTION ÖNCESİ)
   ============================================================ */

-- '123456789' ssn’li çalışanın:
-- adı, departman adı, maaşı
SELECT fname, dname, salary
FROM employee e, department d
WHERE e.dno = d.dnumber
  AND e.ssn = '123456789';


/* ============================================================
   4. ÖRNEK-2: TYPE + RETURNS composite + SELECT ... INTO + RAISE
   ============================================================ */

-- SSN parametre:
-- çalışanın ismi, departman ismi, maaşı -> ekrana yazdır
-- ve composite type olarak geri döndür

CREATE TYPE yeni_tur AS (
    isim     VARCHAR(15),
    dep_isim VARCHAR(25),
    maas     INTEGER
);

CREATE OR REPLACE FUNCTION ornek2(eno employee.ssn%TYPE)
RETURNS yeni_tur AS $$
DECLARE
    bilgi yeni_tur;
BEGIN
    -- SELECT ... INTO ile composite değişkeni dolduruyoruz
    SELECT fname, dname, salary
    INTO bilgi
    FROM employee e, department d
    WHERE e.dno = d.dnumber
      AND e.ssn = eno;

    -- Bilgilendirme çıktısı
    RAISE NOTICE
      'Calisan ismi: %, departmanin ismi: %, maasi: % TLdir.',
      bilgi.isim, bilgi.dep_isim, bilgi.maas;

    RETURN bilgi;
END;
$$ LANGUAGE plpgsql;

-- Çalıştırma:
SELECT ornek2('123456789');

-- Silme:
DROP FUNCTION ornek2(employee.ssn%TYPE);


/* ============================================================
   5. CURSOR (TABLO/ÇOK SATIR DÖNMEK / SATIR SATIR GEZMEK)
   ============================================================ */

-- Bir tablo döndürmek / çok satırı dolaşmak istiyorsak CURSOR kullanırız.
-- Şablon:
-- cursor_adi CURSOR FOR sql_query;

-- Örnek:
-- curs_all CURSOR FOR SELECT * FROM employee;


/* ============================================================
   6. ÖRNEK-3: CURSOR + FOR ... IN cursor LOOP + RAISE INFO
   ============================================================ */

-- Departman no verilen departmandaki çalışanların isimleri
-- (void döndürür, ekrana basar)

CREATE OR REPLACE FUNCTION ornek3(dnum NUMERIC)
RETURNS VOID AS $$
DECLARE
    yeni_cur CURSOR FOR
        SELECT fname, lname
        FROM employee
        WHERE dno = dnum;
BEGIN
    FOR satir IN yeni_cur LOOP
        RAISE INFO 'Employee name is % %', satir.fname, satir.lname;
    END LOOP;
END;
$$ LANGUAGE plpgsql;

SELECT ornek3(6);

DROP FUNCTION ornek3(NUMERIC);


/* ============================================================
   7. ÖRNEK-4: SUM() KULLANMADAN TOPLAM MAAŞ (CURSOR İLE)
   ============================================================ */

-- Departman no verilen departmandaki çalışanların toplam maaşı:
-- SUM() fonksiyonu kullanmadan (manuel toplama)

CREATE OR REPLACE FUNCTION ornek4(dnum NUMERIC)
RETURNS NUMERIC AS $$
DECLARE
    toplam_maas NUMERIC;
    curs CURSOR FOR
        SELECT salary
        FROM employee
        WHERE dno = dnum;
BEGIN
    toplam_maas := 0;

    FOR satir IN curs LOOP
        toplam_maas := toplam_maas + satir.salary;
    END LOOP;

    RETURN toplam_maas;
END;
$$ LANGUAGE plpgsql;

SELECT ornek4(6);

DROP FUNCTION ornek4(NUMERIC);


/* ============================================================
   8. ÖRNEK-4'ÜN OUT İLE ÇÖZÜMÜ
   ============================================================ */

-- OUT üzerinden döndürme

CREATE OR REPLACE FUNCTION dep_sum_salary(
    dnum NUMERIC,
    OUT sum_sal NUMERIC
)
AS $$
DECLARE
    emp_cursor CURSOR FOR
        SELECT salary
        FROM employee
        WHERE dno = dnum;
BEGIN
    sum_sal := 0;

    FOR emp_record IN emp_cursor LOOP
        sum_sal := sum_sal + emp_record.salary;
    END LOOP;
END;
$$ LANGUAGE plpgsql;

SELECT dep_sum_salary(6);

DROP FUNCTION dep_sum_salary(NUMERIC);


/* ============================================================
   9. ÖRNEK-5: ARRAY DÖNDÜRME + HAVING KULLANMADAN FİLTRE
   ============================================================ */

-- Verilen projede çalışanların maaşları 'bolen'e tam bölünüyorsa
-- ad, soyad, maaş bilgilerini HAVING kullanmadan listele ve döndür

-- Composite type:
CREATE TYPE calisan AS (
    isim    VARCHAR(15),
    soyisim VARCHAR(15),
    maas    INTEGER
);

CREATE OR REPLACE FUNCTION calisan_listele(
    pnum  project.pnumber%TYPE,
    bolen INTEGER
)
RETURNS calisan[] AS $$
DECLARE
    emp_cursor CURSOR FOR
        SELECT fname, lname, salary
        FROM employee, works_on
        WHERE ssn = essn
          AND pno = pnum;

    cal calisan[];
    i   INTEGER;
BEGIN
    i := 1;

    FOR emp_record IN emp_cursor LOOP
        -- Maaş tam bölünebilir mi?
        IF (emp_record.salary % bolen = 0) THEN
            -- NOTE: Array index 1’den başlar (array[1]..array[n])
            cal[i] := (emp_record.fname, emp_record.lname, emp_record.salary);
            i := i + 1;
        END IF;
    END LOOP;

    RETURN cal;
END;
$$ LANGUAGE plpgsql;

SELECT calisan_listele('61', 16);

DROP FUNCTION calisan_listele(project.pnumber%TYPE, INTEGER);