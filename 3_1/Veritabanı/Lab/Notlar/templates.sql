-- 1. FONKSIYON TEMPLATE =======================================================================

CREATE OR REPLACE FUNCTION ornek1_out(
    num1 NUMERIC,
    num2 NUMERIC,
    OUT num3 NUMERIC
)
AS $$
DECLARE
    temp NUMERIC;
BEGIN
    temp := num1 + num2;
    num3 := temp;
END;
$$ LANGUAGE plpgsql;

-- 1.1 FONKSIYON CAGIRMA =======================================================================

SELECT ornek1_out(22, 63);

 -- 2.FONKSİYON SİLME =======================================================================

DROP FUNCTION ornek1(NUMERIC, NUMERIC);

 -- 3. IF'li FONKSİYON =======================================================================

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

-- Yöntem 2: Manager’ları join’le, maaşa göre sırala, en küçüğü seç =========================
SELECT e.fname, e.lname
FROM department d
JOIN employee e ON e.ssn = d.mgrssn
ORDER BY e.salary ASC
LIMIT 1;
