-- Rozdzia³ 4
-- Zadanie 1
SELECT MIN(placa_pod) AS MINIMUM, MAX(placa_pod) AS MAKSIMUM, MAX(placa_pod) - MIN(placa_pod) AS ROZNICA FROM pracownicy;
-- Zadanie 2
SELECT etat, AVG(placa_pod) AS SREDNIA FROM pracownicy GROUP BY etat ORDER BY SREDNIA DESC;
-- Zadanie 3
SELECT COUNT(*) AS PROFESOROWIE FROM pracownicy WHERE etat='PROFESOR';
SELECT COUNT(*) AS PROFESOROWIE FROM pracownicy GROUP BY etat HAVING etat = 'PROFESOR';
-- Zadanie 4
SELECT id_zesp, SUM(placa_pod + COALESCE(placa_dod, 0)) AS sumaryczne_place FROM pracownicy GROUP BY id_zesp ORDER BY id_zesp;
-- Zadanie 5
SELECT MAX(SUM(placa_pod + COALESCE(placa_dod, 0))) AS maks_sumaryczna_placa FROM pracownicy GROUP BY id_zesp;
-- Zadanie 6
SELECT id_szefa, MIN(placa_pod) AS minimalna FROM pracownicy WHERE id_szefa IS NOT null GROUP BY id_szefa ORDER BY minimalna DESC;
-- Zadanie 7
SELECT id_zesp, COUNT(*) AS ilu_pracuje FROM pracownicy GROUP BY id_zesp ORDER BY ilu_pracuje DESC;
-- Zadanie 8
SELECT id_zesp, COUNT(*) AS ilu_pracuje FROM pracownicy GROUP BY id_zesp HAVING COUNT(*) > 3 ORDER BY ilu_pracuje DESC;
-- Zadanie 9
SELECT id_prac, COUNT(*) AS zdublowane FROM pracownicy GROUP BY id_prac HAVING COUNT(*) > 1;							
-- Zadanie 10
SELECT etat, AVG(placa_pod) AS srednia, count(*) AS liczba FROM pracownicy WHERE extract(year FROM zatrudniony) <= 1990 GROUP BY etat;		
-- Zadanie 11
SELECT id_zesp, etat, ROUND(AVG(placa_pod + COALESCE(placa_dod, 0))) AS srednia, MAX(placa_pod + COALESCE(placa_dod, 0)) AS maksymalna FROM pracownicy WHERE etat IN ('PROFESOR', 'ASYSTENT') GROUP BY id_zesp , etat ORDER BY id_zesp, etat;
-- Zadania 12
SELECT extract(year FROM zatrudniony) AS rok, count(*) AS ilu_pracownikow FROM pracownicy GROUP BY extract(year FROM zatrudniony) ORDER BY rok;																	  
-- Zadanie 13
SELECT length(nazwisko) AS ile_liter, count(*) AS w_ilu_nazwiskach FROM pracownicy GROUP BY length(nazwisko) ORDER BY ile_liter;																										
-- Zadanie 14
SELECT COUNT( CASE when INSTR(UPPER(nazwisko), 'A', 1, 1)>0 then 1 else null end) AS ile_nazwisk_z_A FROM pracownicy;																										  
-- Zadanie 15
SELECT COUNT( CASE when INSTR(UPPER(nazwisko), 'A', 1, 1)>0 then 1 else null end) AS ile_nazwisk_z_A, 
																										  COUNT( CASE when INSTR(UPPER(nazwisko), 'E', 1, 1)>0 then 1 else null end) AS ile_nazwisk_z_E 
																										  FROM pracownicy;																										  
-- Zadania 16	
SELECT
	id_zesp, sum(placa_pod) AS suma_plac,
	LISTAGG(nazwisko || ':' || placa_pod, ',')
	WITHIN GROUP (ORDER BY nazwisko) AS pracownicy
	FROM pracownicy
	GROUP BY id_zesp
	ORDER BY id_zesp;