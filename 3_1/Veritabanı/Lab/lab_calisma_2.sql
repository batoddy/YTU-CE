-- 1) 5 numaralı departmanda çalışan işçilerin ad soyad bilgileri

SELECT fname, minit, lname, dno FROM employee WHERE dno=5

-- 2) hangi şehirlerde sales var 

SELECT dlocation FROM department, dept_locations WHERE department.dnumber=dept_locations.dnumber AND dname = 'Sales'

-- 3) Atlanta da yaşayan çalışanların ad soyad çalıştığı departman

SELECT fname, lname, dname, address FROM employee e, department d WHERE e.address LIKE '%Atlanta%' AND dno = dnumber 

-- 4) OperatingSystems projesinde çalışanların ad soyad
 SELECT fname, lname FROM project p, employee e, works_on w
 WHERE pname='OperatingSystems' AND 
 p.pnumber = w.pno  AND
 w.essn = e.ssn
 
select fname, lname
from project
join works_on on pnumber = p_no
join employee on essn = ssn
where pname = 'OperatingSystems';

-- 5) Kızının ismi alice, çalıtığı departmanların isimleri

-- 6) maaşı 70.000 üzeri olanların çalıştıkları projeler



