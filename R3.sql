-- Rozdzia³ 3
-- Zadanie 1
SELECT nazwisko, SUBSTR(etat, 1, 2) || id_prac AS KOD FROM pracownicy;
-- Zadanie 2
SELECT nazwisko, TRANSLATE(nazwisko, 'KLM', 'XXX') AS WOJNA_LITEROM FROM pracownicy;
-- Zadanie 3
SELECT nazwisko FROM pracownicy WHERE INSTR(SUBSTR(nazwisko, 1, LENGTH(nazwisko)/2), 'L', 1, 1)>0 ;
-- Zadanie 4
SELECT nazwisko, ROUND(placa_pod * 1.15) AS PODWYZKA FROM pracownicy;
-- Zadanie 5
SELECT nazwisko, placa_pod, 0.2*placa_pod AS INWESTYCJA, POWER(1.1, 10)*0.2*placa_pod AS KAPITAL, POWER(1.1, 10)*0.2*placa_pod - 0.2*placa_pod  AS ZYSK FROM pracownicy;
-- Zadanie 6
SELECT nazwisko, TO_CHAR(zatrudniony, 'YY/MM/DD') AS zatrudni, FLOOR((MONTHS_BETWEEN(DATE '2000-01-01', zatrudniony))/12) AS STAZ_W_2000 FROM pracownicy;							
-- Zadanie 7
SELECT nazwisko, TO_CHAR(zatrudniony, 'MONTH,	DD YYYY') AS DATA_ZATRUDNIENIA FROM pracownicy;
-- Zadanie 8
SELECT TO_CHAR(CURRENT_DATE, 'DAY') AS DZIEN FROM DUAL;
-- Zadanie 9
SELECT nazwa, adres, 
CASE(substr(adres, 1, instr(adres, ' ') - 1 ))
			when 'PIOTROWO' then 'NOWE MIASTO'
			when 'STRZELECKA' then 'STARE MIASTO'
			when 'WLODKOWICA' then 'GRUNWALD'
			when 'MIELZYNSKIEGO' then 'NOWE MIASTO'
			end
			AS DZIELNICA
			FROM zespoly;
-- Zadanie 10
SELECT nazwisko, placa_pod, 
			CASE
			when placa_pod > 480 then 'POWYZEJ 480'
			when placa_pod = 480 then 'DOK£ADNIE 480'
			else 'PONI¯EJ 480'
			end
			AS PRÓG
			FROM pracownicy;