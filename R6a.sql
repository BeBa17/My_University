-- Rozdzia³ 6a
-- Zadanie 1
select nazwisko, etat, id_zesp from pracownicy 
where id_zesp=(select id_zesp from pracownicy p where nazwisko='BRZEZINSKI') order by nazwisko;
-- Zadanie 2
select nazwisko, etat, z.nazwa from pracownicy p JOIN zespoly z on p.id_zesp = z.id_zesp
where p.id_zesp=(select id_zesp from pracownicy where nazwisko='BRZEZINSKI') order by nazwisko;
-- Zadanie 3
select nazwisko, etat, p.zatrudniony from pracownicy p 
where p.zatrudniony=(select min(zatrudniony) from pracownicy where etat='PROFESOR');
-- Zadanie 4
select nazwisko, p.zatrudniony, id_zesp from pracownicy p 
where p.zatrudniony in (select max(zatrudniony) from pracownicy group by id_zesp) order by p.zatrudniony;
-- Zadanie 5
select id_zesp, nazwa, adres from zespoly where id_zesp not in (select distinct id_zesp from pracownicy);
-- Zadanie 6
select p.nazwisko from pracownicy p 
where p.id_prac not in (select distinct id_szefa from pracownicy where etat='STAZYSTA') and p.etat='PROFESOR';
-- Zadanie 7
select p.id_zesp, sum(p.placa_pod) as suma_plac
from pracownicy p join zespoly z on p.id_zesp = z.id_zesp
group by p.id_zesp
having sum(p.placa_pod) = (select max(sum(placa_pod))
from pracownicy p join zespoly z on p.id_zesp = z.id_zesp group by p.id_zesp);
-- Zadanie 8
select z.nazwa, sum(p.placa_pod) as suma_plac
from pracownicy p join zespoly z on p.id_zesp = z.id_zesp
group by z.nazwa
having sum(p.placa_pod) = (select max(sum(placa_pod))
from pracownicy p join zespoly z on p.id_zesp = z.id_zesp group by p.id_zesp);
-- Zadanie 9
select z.nazwa, count(*) as ilu_pracownikow from pracownicy p join zespoly z on p.id_zesp = z.id_zesp
group by z.nazwa having count(*)> (select count(*) from pracownicy p join zespoly z on p.id_zesp = z.id_zesp where 
z.nazwa = 'ADMINISTRACJA' group by z.id_zesp) order by z.nazwa;
-- Zadanie 10
select p.etat from pracownicy p 
having count(*) = ( select max(count(*)) from pracownicy group by etat) group by p.etat;
-- Zadanie 11
select p.etat, listagg(nazwisko, ', ') within group (order by nazwisko) as pracownicy from pracownicy p 
having count(*) = ( select max(count(*)) from pracownicy group by etat) group by p.etat;
-- Zadanie 12
select p.nazwisko as pracownik, s.nazwisko as szef 
from pracownicy p join pracownicy s on p.id_szefa = s.id_prac
where abs(p.placa_pod - s.placa_pod) = ( select min(abs(p.placa_pod-s.placa_pod)) 
from pracownicy p join pracownicy s on p.id_szefa = s.id_prac);