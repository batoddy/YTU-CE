BLM3041 Veritabanı Yönetimi 2.Quizi 12.12.2024  B GRUBU 
Öğrenci No: 22011647
Ad: Sinem          Soyad: Sarak

1-) Şirkette en düşük maaşa sahip departman yöneticisinin ismini ve soyismini iki farklı yöntemle bulunuz. (15p)

1. yöntem: min kullanımı:

select fname,lname
from employee join department on (mgrssn = ssn)
where salary = (select min(salary)
from employee join department on (mgrssn = ssn))

2. yöntem: kıyaslayarak en küçük maaşı bulma

select distinct fname,lname
from employee
where ssn in (select ssn from employee join department on (mgrssn = ssn))
AND salary <= ALL(select distinct salary
from employee
where ssn in (select ssn from employee join department on (mgrssn = ssn)))

sonuç:

"fname"		"lname"
"Franklin"	"Wong"

--------------------------------------------------------------------------------------
2-) Her bir departmanda maaşı 30000’den büyük olan kaç çalışan olduğunu departman isimlerine göre listeleyiniz. 
Maaşı 30000’den büyük olan çalışan sayısı 3’ten az olan departmanları dahil etmeyiniz. (15p)


SELECT count(ssn) as numberofemployees, dname
FROM employee join department on (dno = dnumber)
where salary>30000
group by dname
having count(ssn)> 3


"numberofemployees"	"dname"
13			"Sales"
10			"Hardware"
8			"Software"


--------------------------------------------------------------------------------------


3-)  Hiçbir projede görev almayan çalışanlardan en büyük SSN numarasına sahip olanı bulunuz.(15p)


select max(ssn)
from employee
where ssn not in (select essn
from employee join works_on on (ssn = essn))


"max"
"666666602"

--------------------------------------------------------------------------------------


4-) “Research” departmanına bağlı kaç proje yürütüldüğünü bulan ve döndüren pl/pgSQL fonksiyonunu yazınız (fonksiyon sayı döndürmelidir). 
Fonksiyonu çağırınız. (15p)

fonksiyon:

CREATE or replace FUNCTION getProjectCount (projectName varchar(20))
RETURNS integer AS $$
	DECLARE projectnum integer;
BEGIN

	select count(pnumber) into projectnum
	from department join project on (dnum = dnumber)
	where dname = projectname
	group by dname;
return projectnum;
END; $$ LANGUAGE 'plpgsql';


çağırılması:

select getProjectCount('Research')

sonucu:

"getprojectcount"
3

--------------------------------------------------------------------------------------


5-)  “Administration” departmanında maaşı departman ortalamasının altında kalan kişilerin maaşına 
500 ekleyen pl/pgSQL fonksiyonunu yazınız. Fonksiyonu çağırınız.(20p)


fonksiyon:

CREATE or replace FUNCTION raiseSalary ()
RETURNS void AS $$
	DECLARE avgSalary integer;
BEGIN

	select avg(salary) into avgSalary
	from department join employee on (dno = dnumber)
	where dname = 'Administration';

	update employee 
	set salary = salary + 500
	where salary<avgSalary;
END; $$ LANGUAGE 'plpgsql';


çağrılması:

select raiseSalary ()

--------------------------------------------------------------------------------------


6-) SSN numarası verilen çalışanın 2 veya daha fazla çocuğu var ise 'çocuk yardımı alacak', yoksa 'yardım alamaz' şeklinde dönüş yapan pl/pgSQL fonksiyonunu yazınız.
'123456789' SSN numaralı çalışan için fonksiyonu çağırınız. (20p)
(ipucu: RETURNS TEXT olarak belirttikten sonra fonksiyon içerisinde RETURN 'blah'; yazmak metin ifadesi döndürür.)

fonksiyon:


CREATE or replace FUNCTION getkidshelp (employeessn char(9))
RETURNS TEXT AS $$
	DECLARE numberofkids integer;
BEGIN

	select count(*) into numberofkids
	from dependent join employee on (ssn = essn)
	where relationship <> 'Spouse' AND ssn = employeessn
	group by ssn;

	if numberofkids < 2 then
		RETURN 'yardım alamaz';
	ELSE
		RETURN 'çocuk yardımı alacak';
	end if;
	
END; $$ LANGUAGE 'plpgsql';


çağrılması:

select getkidshelp ('123456789')

sonuç:

"çocuk yardımı alacak"


-- =====================================================
SELECT e.fname,count(*) FROM Employee e,project pr WHERE pr.dnum=e.dno GROUP BY e.fname ORDER BY e.fname


SELECT pname proje_ismi, AVG(hours) ort_saat FROM project p, works_on w WHERE p.pnumber=w.pno GROUP BY pname HAVING AVG(hours)>20 ORDER BY pname



CREATE OR REPLACE FUNCTION soru3(depname department.dname%TYPE)
RETURNS VOID AS $$
DECLARE
	male_min integer;
	female_min integer;
	divideTwo integer;
	minSalaryMale integer;
	minSSNMale integer;
BEGIN
	SELECT avg(salary) INTO male_min FROM Employee e,Department dp WHERE dp.dname=depname AND e.sex='M';
	SELECT avg(salary) INTO female_min FROM Employee e,Department dp WHERE dp.dname=depname AND e.sex='F';
	divideTwo := (male_min+female_min)/2;
	SELECT salary INTO minSalaryMale FROM Employee e WHERE e.sex='M' GROUP BY salary ORDER BY salary LIMIT 1;
	IF divideTwo>minSalaryMale THEN
			SELECT e.ssn INTO minSSNMale FROM Employee e WHERE e.sex='M' AND e.salary = minSalaryMale;
			UPDATE Employee SET salary=divideTwo WHERE employee.ssn=minSSNMale;
	END IF;
END;
$$ LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION soru4(peopleNumber integer,value1 integer,value2 integer)
RETURNS VOID AS $$
DECLARE
	youngestSSN integer;
	youngestDepartment integer;
	howManyWorker integer;
	howManyProject integer;
	departmentManager integer;
BEGIN
	SELECT e.ssn INTO youngestSSN FROM Employee e WHERE bdate IN(SELECT max(bdate) FROM Employee e);
	SELECT e.dno INTO youngestDepartment FROM Employee e WHERE e.ssn=youngestSSN;
	SELECT count(*) INTO howManyWorker FROM Department dp WHERE dp.dname=youngestDepartment;
	SELECT count(*) INTO howManyProject FROM Project pr WHERE pr.dnum=youngestDepartment;
	IF (howManyWorker>peopleNumber)AND(howManyProject<value1) THEN
		SELECT mgrssn INTO departmentManager FROM Department dp WHERE dp.dnumber=youngestDepartment;
		DELETE FROM Works_on wo WHERE essn IN (SELECT ssn FROM Employee e WHERE e.ssn<departmentManager AND e.sex='F'); 
		SELECT max(salary) FROM Employee e,Department dp WHERE dp.dno=youngestDepartment GROUP BY salary ORDER BY salary 
		
	END IF;
END;
$$ LANGUAGE 'plpgsql';
