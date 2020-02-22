-- Rozdzia³ 5b
-- Zadanie 1
insert into pracownicy(id_prac, nazwisko)
values ((select max(id_prac) + 1 from pracownicy), 'WOLNY');
--
SELECT nazwisko, p.id_zesp, z.nazwa FROM pracownicy p LEFT OUTER JOIN zespoly z ON p.id_zesp = z.id_zesp ORDER BY nazwisko;
-- Zadanie 2
SELECT z.nazwa, z.id_zesp,
CASE WHEN p.nazwisko IS NULL then 'brak pracownikow' else p.nazwisko end AS pracownik
FROM zespoly z LEFT OUTER JOIN pracownicy p ON p.id_zesp = z.id_zesp ORDER BY z.nazwa, nazwisko;
-- Zadanie 3
SELECT CASE WHEN z.nazwa IS NULL then 'brak zespolu' else z.nazwa end AS zespol, 
CASE WHEN p.nazwisko IS NULL then 'brak pracownikow' else p.nazwisko end AS pracownik
FROM zespoly z FULL OUTER JOIN pracownicy p ON p.id_zesp = z.id_zesp
ORDER BY z.nazwa, p.nazwisko;
--
delete from pracownicy where nazwisko = 'WOLNY';
-- Zadanie 4
SELECT z.nazwa AS zespol, count(p.placa_pod) AS liczba, sum(p.placa_pod) as suma_plac 
FROM zespoly z LEFT OUTER JOIN pracownicy p ON z.id_zesp = p.id_zesp GROUP BY z.nazwa ORDER BY z.nazwa;
-- Zadanie 5
SELECT z.nazwa AS zespol FROM zespoly z LEFT OUTER JOIN pracownicy p ON z.id_zesp = p.id_zesp
minus
SELECT z.nazwa FROM zespoly z JOIN pracownicy p ON z.id_zesp = p.id_zesp;
-- Zadanie 6
SELECT p.nazwisko, p.id_prac, s.nazwisko AS szef, p.id_szefa
FROM pracownicy p LEFT OUTER JOIN pracownicy s ON p.id_szefa = s.id_prac
ORDER BY p.nazwisko;
-- Zadanie 7
SELECT p.nazwisko, count(s.id_szefa) AS liczba_podwladnych
FROM pracownicy p LEFT OUTER JOIN pracownicy s ON p.id_prac = s.id_szefa GROUP BY p.nazwisko ORDER BY p.nazwisko;
-- Zadanie 8
SELECT p.nazwisko, p.etat, p.placa_pod, z.nazwa, s.nazwisko AS szef
FROM (pracownicy p LEFT OUTER JOIN zespoly z ON p.id_zesp = z.id_zesp ) 
LEFT OUTER JOIN pracownicy s ON p.id_szefa = s.id_prac
ORDER BY p.nazwisko;
-- Zadanie 9
SELECT p.nazwisko, z.nazwa FROM pracownicy p CROSS JOIN zespoly z ORDER BY p.nazwisko, z.nazwa;
-- Zadanie 10
SELECT count(*) FROM (pracownicy p CROSS JOIN zespoly z) CROSS JOIN etaty e;
-- Operatory zbiorowe
-- Zadanie 11
SELECT etat FROM pracownicy
WHERE extract (year from zatrudniony) = 1992
INTERSECT
SELECT etat FROM pracownicy
WHERE extract (year from zatrudniony) = 1993;
-- Zadanie 12
SELECT id_zesp FROM zespoly
MINUS
SELECT z.id_zesp FROM zespoly z INNER JOIN pracownicy p ON p.id_zesp = z.id_zesp;
-- Zadanie 13
SELECT id_zesp, nazwa FROM zespoly
MINUS
SELECT z.id_zesp, z.nazwa FROM zespoly z INNER JOIN pracownicy p ON p.id_zesp = z.id_zesp;
-- Zadanie 14
SELECT nazwisko, placa_pod, 'Ponizej 480 zlotych' AS prog FROM pracownicy WHERE placa_pod<480
UNION
SELECT nazwisko, placa_pod, 'Dokladnie 480 zlotych' AS prog FROM pracownicy WHERE placa_pod=480
UNION
SELECT nazwisko, placa_pod, 'Powyzej 480 zlotych' AS prog FROM pracownicy WHERE placa_pod>480
ORDER BY placa_pod;