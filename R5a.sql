-- Rozdzia³ 5a
-- Zadania 1
SELECT nazwisko, etat, p.id_zesp, nazwa FROM pracownicy p JOIN zespoly z ON p.id_zesp = z.id_zesp ORDER BY nazwisko;
-- Zadanie 2
SELECT nazwisko, etat, p.id_zesp, nazwa FROM pracownicy p JOIN zespoly z ON p.id_zesp = z.id_zesp WHERE z.adres = 'PIOTROWO 3A' ORDER BY nazwisko;
-- Zadanie 3
SELECT nazwisko, p.etat, p.placa_pod, e.placa_min, e.placa_max FROM pracownicy p JOIN etaty e ON p.etat = e.nazwa ORDER BY p.etat, nazwisko;
-- Zadanie 4
SELECT nazwisko, p.etat, p.placa_pod, e.placa_min, e.placa_max,  
CASE WHEN e.placa_min <= p.placa_pod and e.placa_max >= p.placa_pod then 'OK' ELSE 'NIE' END as czy_pensja_ok
FROM pracownicy p JOIN etaty e ON p.etat = e.nazwa ORDER BY p.etat, nazwisko;
-- Zadanie 5
SELECT nazwisko, p.etat, p.placa_pod, e.placa_min, e.placa_max
FROM pracownicy p JOIN etaty e ON p.etat = e.nazwa 
WHERE p.placa_pod < e.placa_min or p.placa_pod > e.placa_max ORDER BY p.etat, nazwisko;
-- Zadanie 6
SELECT nazwisko, placa_pod, p.etat, e.nazwa AS kat_plac, e.placa_min, e.placa_max 
FROM pracownicy p JOIN etaty e ON p.placa_pod >= e.placa_min and p.placa_pod <= e.placa_max
ORDER BY nazwisko, kat_plac;
-- Zadanie 7
SELECT nazwisko, placa_pod, p.etat, e.nazwa AS kat_plac, e.placa_min, e.placa_max 
FROM pracownicy p JOIN etaty e ON p.placa_pod >= e.placa_min and p.placa_pod <= e.placa_max
WHERE e.nazwa = 'SEKRETARKA'
ORDER BY nazwisko, kat_plac;
-- Zadanie 8
SELECT p.nazwisko, p.id_prac, s.nazwisko AS szef, p.id_szefa
FROM pracownicy p JOIN pracownicy s ON p.id_szefa = s.id_prac
ORDER BY p.nazwisko;
-- Zadanie 9
SELECT p.nazwisko, p.zatrudniony, s.nazwisko AS szef, s.zatrudniony AS szef_zat,
extract(year from p.zatrudniony) - extract(year from s.zatrudniony) AS lata
FROM pracownicy p JOIN pracownicy s ON p.id_szefa = s.id_prac
where extract(year from p.zatrudniony) - extract(year from s.zatrudniony) < 10
ORDER BY lata, p.nazwisko;
-- Zadanie 10
SELECT z.nazwa, count(*) AS liczba_prac, avg(p.placa_pod) AS srednia_placa
FROM pracownicy p JOIN zespoly z ON p.id_zesp = z.id_zesp
GROUP BY z.nazwa ORDER BY z.nazwa;
-- Zadanie 11
SELECT z.nazwa, 
CASE WHEN count(*)<3 then 'maly' when count(*)>6 then 'duzy' else 'sredni' end AS etykieta
FROM pracownicy p JOIN zespoly z ON p.id_zesp = z.id_zesp
GROUP BY z.nazwa ORDER BY z.nazwa;