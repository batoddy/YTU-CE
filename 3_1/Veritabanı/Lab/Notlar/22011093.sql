BLM3041 Veritabanı Yönetimi 2.Quizi 19.12.2025    Süre: 50 dk
Öğrenci Numarası: 22011093
Ad: Batuhan         Soyad: Odçıkın

--Her soru 20 puandır. Sorularda gereksiz tablo kullanımından puan kırılır.
--Yanlış çıktı üreten/çıktı üretmeyen sorgular 0 olarak puanlanacaktır.
--Başarılar.

1-)En fazla sayıda farklı lokasyonda ofisi bulunan departmanın ismini veren SQL sorgusunu yazınız.

SELECT d.dname
FROM department d
JOIN dept_locations dl ON dl.dnumber = d.dnumber
GROUP BY d.dnumber, d.dname
ORDER BY COUNT(DISTINCT dl.dlocation) DESC
LIMIT 1;

2-) Ortalama maaş 45000 den büyük olan sonuçlar gösterilecek şekilde her departman numarası için 
herbir cinsiyetten kaç çalışan olduğunu ve bu grubun ortalama maaşını veren SQL sorgusunu yazınız.

SELECT
    e.dno        AS departman_no,
    e.sex        AS cinsiyet,
    COUNT(*)     AS calisan_sayisi,
    AVG(e.salary) AS ortalama_maas
FROM employee e
GROUP BY e.dno, e.sex
HAVING AVG(e.salary) > 45000
ORDER BY e.dno, e.sex;


3-) "Research" departmanına bağlı olarak yürütülen projelerde çalışan kişilerin bir üst seviyedeki 
yöneticilerinin çalıştığı projelerin isimlerini listeleyen PL/pgSQL fonksiyonunu yazınız.

CREATE OR REPLACE FUNCTION research_yonetici_projeleri()
RETURNS TABLE(pname VARCHAR(25))
AS $$
DECLARE
    research_no department.dnumber%TYPE;
BEGIN

    SELECT dnumber
    INTO research_no
    FROM department
    WHERE dname = 'Research';


    RETURN QUERY
    SELECT DISTINCT p2.pname::VARCHAR(25)
    FROM employee e
    JOIN works_on w   ON w.essn = e.ssn
    JOIN project p    ON p.pnumber = w.pno
    JOIN employee m   ON m.ssn = e.superssn         
    JOIN works_on w2  ON w2.essn = m.ssn             
    JOIN project p2   ON p2.pnumber = w2.pno
    WHERE p.dnum = research_no
      AND e.superssn IS NOT NULL;
END;
$$LANGUAGE plpgsql;


--Fonksiyon çağrısı:
select * from research_yonetici_projeleri()


4-) Proje numarası, proje ismi ve projenin bağlı bulunduğu departman ismini tutan "project_info" isimli yeni bir veri türü tanımlayın.


CREATE TYPE project_info AS (
    proje_no   project.pnumber%TYPE,
    proje_adi  project.pname%TYPE,
    departman_adi department.dname%TYPE
);

5-) Ad ve soyad bilgisi verilen çalışanın akrabalarının isimlerini ve akrabalık derecelerini cursor kullanarak
bilgi mesajı olarak yazdıran PL/pgSQL fonksiyonunu yazınız.(15p) Fonksiyonu çağırınız.(5p)


CREATE OR REPLACE FUNCTION yazdir_akraba_bilgisi(p_fname VARCHAR, p_lname VARCHAR)
RETURNS VOID
LANGUAGE plpgsql
AS $$
DECLARE
    emp_ssn employee.ssn%TYPE;

    dep_cur CURSOR FOR
        SELECT d.dependent_name, d.relationship
        FROM dependent d
        WHERE d.essn = emp_ssn;
BEGIN

    SELECT ssn
    INTO emp_ssn
    FROM employee
    WHERE fname = p_fname
      AND lname = p_lname;

    IF emp_ssn IS NULL THEN
        RAISE EXCEPTION 'Çalışan bulunamadı: % %', p_fname, p_lname;
    END IF;


    FOR rec IN dep_cur LOOP
        RAISE INFO 'Akraba: %, Derece: %', rec.dependent_name, rec.relationship;
    END LOOP;
END;
$$;


SELECT yazdir_akraba_bilgisi('John', 'Smith')
