/* ============================================================
   LAB-4 CHEAT SHEET
   KONULAR:
   - Aggregate Fonksiyonlar
   - AS (Alias)
   - Subquery (İç sorgu)
   - GROUP BY
   - HAVING
   - ORDER BY (ASC / DESC / Çoklu)
   - LIMIT / OFFSET
   - NULL
   - EXTRACT (DATE)
   ============================================================ */


/* ============================================================
   1. AGGREGATE (GRUP) FONKSİYONLARI
   ============================================================ */

-- AVG(col)   : Ortalama
-- COUNT(*)   : Satır sayısı
-- MIN(col)   : En küçük
-- MAX(col)   : En büyük
-- SUM(col)   : Toplam

-- Aggregate fonksiyonlar:
-- ✔ SELECT ve HAVING içinde kullanılır
-- ❌ WHERE içinde KULLANILMAZ


/***** ÖRNEK 1 *****/
-- "Sales" departmanında:
-- kişi sayısı, toplam maaş, max, min, ortalama

SELECT
    COUNT(*),
    SUM(salary),
    MAX(salary),
    MIN(salary),
    AVG(salary)
FROM department d, employee e
WHERE d.dnumber = e.dno
  AND d.dname = 'Sales';


/* ============================================================
   2. AS (ALIAS – TAKMA AD)
   ============================================================ */

-- AS kullanılarak kolon isimleri değiştirilebilir
-- AS yazmak zorunlu değildir

SELECT
    AVG(salary) AS ortalama,
    SUM(salary) AS toplam
FROM employee
WHERE dno = 8;

-- AS OLMADAN:
SELECT
    AVG(salary) ortalama,
    SUM(salary) toplam
FROM employee
WHERE dno = 8;


/* ============================================================
   3. SUBQUERY (İÇ SORGU)
   ============================================================ */

-- En genç çalışanın çalıştığı projelerin numaraları

-- DOĞRU YÖNTEM
SELECT pno
FROM employee, works_on
WHERE ssn = essn
  AND bdate = (
      SELECT MAX(bdate)
      FROM employee
  );

-- IN ile de yazılabilir
SELECT pno
FROM employee, works_on
WHERE ssn = essn
  AND bdate IN (
      SELECT MAX(bdate)
      FROM employee
  );

-- YANLIŞ (AGGREGATE WHERE içinde kullanılamaz)
-- ERROR verir
-- WHERE bdate = MAX(bdate)


/* ============================================================
   4. GROUP BY
   ============================================================ */

-- GROUP BY:
-- Satırları gruplar
-- Aggregate fonksiyonlarla birlikte çalışır

-- KURAL:
-- SELECT içinde olup aggregate olmayan her kolon
-- GROUP BY içinde OLMALIDIR


/***** ÖRNEK *****/
-- Her departmanda kaç proje var?

SELECT
    dnum,
    COUNT(*)
FROM project
GROUP BY dnum;


/***** ÖRNEK *****/
-- Bilgisayar Müh. derslerini alan öğrenci sayıları

SELECT
    COUNT(*),
    code
FROM take
WHERE code LIKE 'BLM%'
GROUP BY code;


/* ============================================================
   5. HAVING
   ============================================================ */

-- HAVING:
-- Grup filtrelemek için kullanılır
-- Aggregate koşullar burada yazılır

-- WHERE → satır filtresi
-- HAVING → grup filtresi

-- GROUP BY OLMADAN HAVING OLMAZ


/***** ÖRNEK *****/
-- Ortalama maaşı 40000’den fazla olan departmanlar

SELECT dno
FROM employee
GROUP BY dno
HAVING AVG(salary) > 40000;


/***** ÖRNEK *****/
-- 2012 girişliler arasında sadece 1 kişi tarafından alınan dersler

SELECT
    code
FROM take
WHERE id LIKE '12%'
GROUP BY code
HAVING COUNT(*) = 1;


/* ============================================================
   6. WHERE + GROUP BY + HAVING BİRLİKTE
   ============================================================ */

-- 5 numaralı departman HARİÇ
-- ortalama maaşı 40000’den büyük departmanlar

SELECT
    dno AS departman_no,
    AVG(salary) AS ortalama_maas
FROM employee
WHERE dno != 5          -- <> veya != kullanılabilir
GROUP BY dno
HAVING AVG(salary) > 40000;


/* ============================================================
   7. ORDER BY
   ============================================================ */

-- Varsayılan: ASC (artan)

SELECT code, id
FROM take
WHERE id LIKE '12%'
ORDER BY id;

-- DESC (azalan)

SELECT code, id
FROM take
WHERE id LIKE '12%'
ORDER BY id DESC;

-- Çoklu sıralama

SELECT code, id
FROM take
ORDER BY id DESC, code ASC;


/* ============================================================
   8. LIMIT & OFFSET
   ============================================================ */

-- LIMIT A:
-- İlk A satır

SELECT *
FROM take
LIMIT 3;

-- LIMIT A OFFSET B:
-- B’den sonra A satır

SELECT *
FROM take
LIMIT 3 OFFSET 5;


/* ============================================================
   9. EN YÜKSEK MAAŞ (ORDER BY + LIMIT)
   ============================================================ */

SELECT fname, lname
FROM employee
ORDER BY salary DESC
LIMIT 1;

-- OFFSET ile eşdeğer
LIMIT 1 OFFSET 0;


/* ============================================================
   10. EN YÜKSEK MAAŞ (SUBQUERY ALTERNATİFİ)
   ============================================================ */

SELECT fname, lname
FROM employee
WHERE salary = (
    SELECT MAX(salary)
    FROM employee
);


/* ============================================================
   11. NULL
   ============================================================ */

-- NULL = bilinmeyen / boş değer
-- = NULL KULLANILMAZ

-- Yöneticisi olmayan çalışanlar

SELECT ssn, superssn
FROM employee
WHERE superssn IS NULL;


/* ============================================================
   12. EXTRACT (DATE)
   ============================================================ */

-- DATE alanından parça almak için

SELECT EXTRACT(YEAR FROM bdate) FROM employee;
SELECT EXTRACT(MONTH FROM bdate) FROM employee;
SELECT EXTRACT(DAY FROM bdate) FROM employee;
SELECT EXTRACT(WEEK FROM bdate) FROM employee;
SELECT EXTRACT(CENTURY FROM bdate) FROM employee;


/* ============================================================
   13. SORGU YAZIM SIRASI (SUNUMDAKİ)
   ============================================================ */

-- SELECT
-- FROM
-- WHERE
-- GROUP BY
-- HAVING
-- ORDER BY
-- LIMIT

/* ===================== SON ===================== */
