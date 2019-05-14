-- Rozdzia³ 2
-- Zadanie 1
SELECT * FROM zespoly;
-- Zadanie 2
SELECT * FROM pracownicy;
-- Zadanie 3
SELECT nazwisko, placa_pod*12 FROM pracownicy;
-- Zadanie 4
SELECT etat, placa_pod + COALESCE(placa_dod, 0) AS MIESIECZNE_ZAROBKI FROM pracownicy;
-- Zadanie 5
SELECT * FROM zespoly ORDER BY nazwa;
-- Zadanie 6
SELECT DISTINCT etat FROM pracownicy;
-- Zadanie 7
SELECT * FROM pracownicy WHERE etat='ASYSTENT'; 
-- Zadanie 8
SELECT ID_PRAC, NAZWISKO, ETAT, PLACA_POD, ID_ZESP FROM pracownicy WHERE id_zesp IN (30, 40) ORDER BY placa_pod DESC;
-- Zadanie 9
SELECT nazwisko, id_zesp, placa_pod FROM pracownicy WHERE placa_pod BETWEEN 300 AND 800;
-- Zadanie 10
SELECT nazwisko, etat, id_zesp FROM pracownicy WHERE nazwisko LIKE '%SKI';
-- Zadanie 11
SELECT id_prac, id_szefa, nazwisko, placa_pod FROM pracownicy WHERE placa_pod > 1000 AND id_szefa IS NOT null;
-- Zadanie 12
SELECT nazwisko, id_zesp FROM pracownicy WHERE (id_zesp=20) AND (nazwisko LIKE 'M%' OR nazwisko LIKE '%SKI');
-- Zadanie 13
SELECT nazwisko, etat, placa_pod/(20*8) AS stawka FROM pracownicy WHERE etat NOT IN ('ADIUNKT', 'ASYSTENT', 'STAZYSTA') AND placa_pod NOT BETWEEN 400 AND 800 ORDER BY stawka;
-- Zadanie 14
SELECT nazwisko, etat, placa_pod, COALESCE(placa_dod, 0) FROM pracownicy WHERE placa_pod + COALESCE(placa_dod, 0) > 1000 ORDER BY etat, nazwisko;
-- Zadanie 15
SELECT nazwisko || ' pracuje od ' || zatrudniony || ' i zarabia ' || placa_pod AS Profesorowie FROM pracownicy WHERE etat='PROFESOR' ORDER BY placa_pod DESC;