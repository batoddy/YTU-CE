/* ============================================================
   LAB-6 CHEAT SHEET
   KONU: PL/pgSQL FONKSİYON TANIMI
   ============================================================ */

/* ============================================================
   3. PL/pgSQL FONKSİYON DÖNÜŞ TÜRLERİ
   ============================================================ */

-- 1) Tek değer döndürebilir
-- 2) Birden fazla değer → OUT parametre
-- 3) Composite (birleşik) veri döndürebilir
-- 4) Cursor / pointer döndürebilir
-- 5) Hiçbir değer döndürmeyebilir (RETURNS VOID)


/* ============================================================
   4. PL/pgSQL AKTİF ETME
   ============================================================ */

-- Standart PostgreSQL’de önce dili oluşturmalıyız
CREATE LANGUAGE plpgsql;


/* ============================================================
   5. FONKSİYON TANIM ŞABLONU
   ============================================================ */

CREATE OR REPLACE FUNCTION fonksiyon_adi(
    param1 TYPE,
    param2 TYPE,
    OUT paramN TYPE   -- opsiyonel
)
RETURNS TYPE AS $$
DECLARE
    -- değişken tanımları
BEGIN
    -- işlemler
    RETURN;
EXCEPTION
    -- hata yakalama (opsiyonel)
END;
$$ LANGUAGE plpgsql;


/* ============================================================
   6. DECLARE (DEĞİŞKEN TANIMI)
   ============================================================ */

-- Genel kullanım
-- isim TYPE := ilk_deger;

DECLARE
    user_id INTEGER;
    quantity NUMERIC;
    url VARCHAR(20);

-- Tablo kolon tipinden alma
DECLARE    my_var employee.salary%TYPE;


/* ============================================================
   7. RETURN & OUT KULLANIMI
   ============================================================ */

-- ✔ RETURN → fonksiyondan çıkış
-- ✔ OUT → çoklu dönüş
-- ❌ RETURNS VOID olmayan fonksiyonlarda RETURN ZORUNLU


/* ============================================================
   8. ÖRNEK 1 – İKİ SAYININ TOPLAMI (RETURN)
   ============================================================ */

CREATE OR REPLACE FUNCTION ornek1(
    num1 NUMERIC,
    num2 NUMERIC
)
RETURNS NUMERIC AS $$
DECLARE
    toplam NUMERIC;
BEGIN
    toplam := num1 + num2;
    RETURN toplam;
END;
$$ LANGUAGE plpgsql;

-- Çalıştırma
SELECT ornek1(22, 63);


/* ============================================================
   9. ÖRNEK 1 – RETURN OLMADAN (OUT)
   ============================================================ */

CREATE OR REPLACE FUNCTION ornek1_out(
    num1 NUMERIC,
    num2 NUMERIC,
    OUT num3 NUMERIC
)
AS $$
BEGIN
    num3 := num1 + num2;
END;
$$ LANGUAGE plpgsql;

SELECT ornek1_out(22, 63);


/* ============================================================
   10. FONKSİYON SİLME
   ============================================================ */

DROP FUNCTION ornek1(NUMERIC, NUMERIC);


/* ============================================================
   11. SELECT INTO (PL/pgSQL’E ÖZEL)
   ============================================================ */

-- SQL’den farklı olarak sonuç değişkene alınır

SELECT AVG(salary)
INTO maas
FROM employee;


/* ============================================================
   12. ÖRNEK 2 – PARAMETRELİ FONKSİYON
   ============================================================ */

CREATE OR REPLACE FUNCTION ornek2(
    depname department.dname%TYPE
)
RETURNS REAL AS $$
DECLARE
    maas NUMERIC;
BEGIN
    SELECT AVG(salary)
    INTO maas
    FROM employee e, department d
    WHERE e.dno = d.dnumber
      AND d.dname = depname;

    RETURN maas;
END;
$$ LANGUAGE plpgsql;

-- NOT: REAL → virgülden sonra 7 basamak

SELECT ornek2('Hardware');


/* ============================================================
   13. ÖRNEK 3 – PARAMETRESİZ + OUT
   ============================================================ */

CREATE OR REPLACE FUNCTION ornek3(
    OUT min_deptno department.dnumber%TYPE,
    OUT max_deptno department.dnumber%TYPE
)
AS $$
BEGIN
    SELECT MIN(dnumber), MAX(dnumber)
    INTO min_deptno, max_deptno
    FROM department;
END;
$$ LANGUAGE plpgsql;

SELECT ornek3();


/* ============================================================
   14. YARDIMCI ÖRNEK – COUNT
   ============================================================ */

CREATE OR REPLACE FUNCTION calisan_sayisi()
RETURNS NUMERIC AS $$
DECLARE
    sayi NUMERIC;
BEGIN
    SELECT COUNT(*) INTO sayi
    FROM employee
    WHERE dno = 6;

    RETURN sayi;
END;
$$ LANGUAGE plpgsql;


/* ============================================================
   15. ÖRNEK 4 – RETURNS VOID
   ============================================================ */

CREATE OR REPLACE FUNCTION ornek4()
RETURNS VOID AS $$
DECLARE
    num_worker NUMERIC(3) := 0;
BEGIN
    SELECT COUNT(*)
    INTO num_worker
    FROM employee
    WHERE dno = 6;

    IF (num_worker < 10) THEN
        UPDATE employee
        SET salary = salary * 1.05
        WHERE dno = 6;
    END IF;
END;
$$ LANGUAGE plpgsql;

SELECT ornek4();


/* ============================================================
   16. IF – ELSIF – ELSE
   ============================================================ */

IF kosul THEN
    -- yapılacaklar
ELSIF baska_kosul THEN
    -- yapılacaklar
ELSE
    -- yapılacaklar
END IF;


/* ============================================================
   17. CASE
   ============================================================ */

CASE secici
    WHEN 1 THEN ...
    WHEN 2 THEN ...
    ELSE ...
END CASE;


CREATE OR REPLACE FUNCTION myfunc1(x INTEGER)
RETURNS TEXT AS $$
DECLARE
    msg TEXT;
BEGIN
    CASE x
        WHEN 1, 2 THEN msg := 'one or two';
        WHEN 3, 4 THEN msg := 'three or four';
        ELSE msg := 'other value';
    END CASE;

    RETURN msg;
END;
$$ LANGUAGE plpgsql;


/* ============================================================
   18. WHILE DÖNGÜSÜ
   ============================================================ */

WHILE kosul LOOP
    -- işlemler
END LOOP;


/* ============================================================
   19. FOR DÖNGÜLERİ
   ============================================================ */

FOR i IN 1..5 LOOP
    RAISE NOTICE 'Sayı: %', i;
END LOOP;

FOR i IN REVERSE 5..1 BY 2 LOOP
    RAISE NOTICE 'Sayı: %', i;
END LOOP;


/* ============================================================
   20. ÖRNEK 5 – BASİT RETURN
   ============================================================ */

CREATE OR REPLACE FUNCTION increment(t INTEGER)
RETURNS INTEGER AS $$
BEGIN
    RETURN t + 1;
END;
$$ LANGUAGE plpgsql;


/* ============================================================
   21. ÖRNEK 6 – KOMPLEKS KOŞULLU FONKSİYON
   ============================================================ */

CREATE OR REPLACE FUNCTION kosullu_zam_yap(
    bolum_ismi department.dname%TYPE,
    ort_maas REAL,
    f_top_maas employee.salary%TYPE,
    zam_orani REAL
)
RETURNS VOID AS $$
DECLARE
    ger_ort_maas REAL;
    kadin_maaslari INTEGER;
    bolum_no department.dnumber%TYPE;
BEGIN
    SELECT dnumber
    INTO bolum_no
    FROM department
    WHERE dname = bolum_ismi;

    SELECT AVG(salary)
    INTO ger_ort_maas
    FROM employee
    WHERE dno = bolum_no;

    SELECT SUM(salary)
    INTO kadin_maaslari
    FROM employee
    WHERE dno = bolum_no
      AND sex = 'F';

    IF ger_ort_maas < ort_maas
       AND kadin_maaslari > f_top_maas THEN

        UPDATE employee
        SET salary = salary * zam_orani / 100 + salary
        WHERE ssn IN (
            SELECT essn
            FROM employee, works_on
            WHERE ssn = essn
              AND dno = bolum_no
            GROUP BY essn
            HAVING COUNT(*) > 1
        );
    END IF;
END;
$$ LANGUAGE plpgsql;


/* ===================== SON ===================== */
