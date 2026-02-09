/* ============================================================
   SQL CHEAT SHEET
   KONULAR:
   - JOIN (INNER / LEFT / RIGHT / eski stil)
   - WHERE
   - GROUP BY
   - HAVING
   - ORDER BY
   - LIMIT / OFFSET
   ============================================================ */


/* ============================================================
   1) JOIN (TABLO BİRLEŞTİRME)
   ============================================================ */

-- 1.1 INNER JOIN (EN SIK KULLANILAN)
-- Sadece eşleşen satırlar gelir
SELECT e.fname, e.lname, d.dname
FROM employee e
JOIN department d ON e.dno = d.dnumber;

-- 1.2 LEFT JOIN
-- Sol tablo her zaman gelir, eşleşmeyenler NULL
SELECT e.fname, d.dname
FROM employee e
LEFT JOIN department d ON e.dno = d.dnumber;

-- 1.3 RIGHT JOIN
-- Sağ tablo her zaman gelir
SELECT e.fname, d.dname
FROM employee e
RIGHT JOIN department d ON e.dno = d.dnumber;

-- 1.4 ESKİ STİL JOIN (sınavlarda çıkar)
SELECT e.fname, d.dname
FROM employee e, department d
WHERE e.dno = d.dnumber;


/* ============================================================
   2) WHERE (SATIR FİLTRELEME)
   ============================================================ */

-- WHERE satırları filtreler
SELECT *
FROM employee
WHERE salary > 30000;

-- NULL kontrolü
SELECT *
FROM employee
WHERE superssn IS NULL;

-- LIKE (pattern)
SELECT *
FROM employee
WHERE fname LIKE 'A%';

-- IN (liste)
SELECT *
FROM employee
WHERE dno IN (1, 4, 5);

-- IN (subquery)
SELECT *
FROM employee
WHERE dno IN (
    SELECT dnumber
    FROM department
    WHERE dname = 'Research'
);


/* ============================================================
   3) GROUP BY (GRUPLAMA)
   ============================================================ */

-- GROUP BY satırları gruplar
SELECT dno, COUNT(*) AS calisan_sayisi
FROM employee
GROUP BY dno;

-- KURAL:
-- SELECT içinde aggregate olmayan her kolon GROUP BY’da olmalı

-- DOĞRU
SELECT dno, AVG(salary)
FROM employee
GROUP BY dno;

-- YANLIŞ
-- SELECT fname, COUNT(*) FROM employee GROUP BY dno;


/* ============================================================
   4) HAVING (GRUP FİLTRELEME)
   ============================================================ */

-- HAVING aggregate sonuçlarını filtreler
SELECT dno, COUNT(*) AS cnt
FROM employee
GROUP BY dno
HAVING COUNT(*) >= 3;

-- WHERE + GROUP BY + HAVING birlikte
SELECT dno, AVG(salary) AS avg_sal
FROM employee
WHERE salary > 30000        -- satır filtresi
GROUP BY dno
HAVING AVG(salary) > 40000; -- grup filtresi

-- KURAL:
-- WHERE  -> aggregate YOK
-- HAVING -> aggregate VAR


/* ============================================================
   5) JOIN + GROUP BY + HAVING (SINAV KLASIĞİ)
   ============================================================ */

-- Her departmanda maaşı 30000’den büyük kaç çalışan var?
-- Sayısı 2’den az olan departmanları gösterme

SELECT d.dname, COUNT(*) AS cnt
FROM department d
JOIN employee e ON e.dno = d.dnumber
WHERE e.salary > 30000
GROUP BY d.dname
HAVING COUNT(*) >= 2;


/* ============================================================
   6) ORDER BY (SIRALAMA)
   ============================================================ */

-- Varsayılan ASC
SELECT fname, salary
FROM employee
ORDER BY salary;

-- DESC
SELECT fname, salary
FROM employee
ORDER BY salary DESC;

-- Çoklu sıralama
SELECT *
FROM employee
ORDER BY dno ASC, salary DESC;


/* ============================================================
   7) LIMIT / OFFSET
   ============================================================ */

-- En yüksek maaşlı 1 kişi
SELECT fname, lname, salary
FROM employee
ORDER BY salary DESC
LIMIT 1;

-- 3. en yüksek maaştan itibaren 2 kişi
SELECT fname, lname, salary
FROM employee
ORDER BY salary DESC
LIMIT 2 OFFSET 2;


/* ============================================================
   8) EN SIK KULLANILAN SINAV ŞABLONLARI
   ============================================================ */

-- ŞABLON 1: En büyük / en küçük
SELECT *
FROM <<tablo>>
ORDER BY <<kolon>> DESC
LIMIT 1;

-- ŞABLON 2: Aggregate + HAVING
SELECT <<group_col>>, COUNT(*) 
FROM <<tablo>>
GROUP BY <<group_col>>
HAVING COUNT(*) > <<deger>>;

-- ŞABLON 3: JOIN + filtre
SELECT <<cols>>
FROM <<A>> a
JOIN <<B>> b ON <<a.x = b.y>>
WHERE <<kosul>>;

-- ŞABLON 4: Subquery ile eşitleme
SELECT *
FROM <<tablo>>
WHERE <<kolon>> = (
    SELECT MAX(<<kolon>>)
    FROM <<tablo>>
);


/* ============================================================
   9) ALTIN KURALLAR (SINAVLIK)
   ============================================================ */

-- 1) Aggregate varsa -> HAVING
-- 2) Satır filtresi -> WHERE
-- 3) Tablo birleştirme -> JOIN
-- 4) GROUP BY yoksa HAVING OLMAZ
-- 5) = NULL OLMAZ -> IS NULL

/* ===================== SON ===================== */
