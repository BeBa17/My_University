-- Rozdzia³ 6b
-- Zadanie 1 
select z.id_zesp, z.nazwa, z.adres from zespoly z 
where (select count(*) from pracownicy where z.id_zesp = id_zesp) = 0;
-- Zadanie 2
select nazwisko, placa_pod, etat from pracownicy p
where placa_pod > (select avg(placa_pod) from pracownicy where etat = p.etat)
order by placa_pod DESC;
-- Zadanie 3
select nazwisko, placa_pod from pracownicy p 
where placa_pod >= 0.75*(select placa_pod from pracownicy where id_prac = p.id_szefa);
-- Zadanie 4
select nazwisko from pracownicy p 
where etat='PROFESOR' and p.id_prac not in 
(select distinct id_szefa from pracownicy s where etat = 'STAZYSTA') ;
-- Zadanie 5 - zrobic
select z.nazwa, m.maks from 
(select max(sum(placa_pod)) as maks from pracownicy group by id_zesp) m 
			join (select id_zesp, sum(placa_pod) as sumy from pracownicy group by id_zesp) s on sumy = maks 
			join zespoly z on z.id_zesp = s.id_zesp;
-- Zadanie 6
select nazwisko, placa_pod from pracownicy o where o.placa_pod >= (
select p.placa_pod from pracownicy p 
where ( select count(*) from pracownicy where placa_pod >= p.placa_pod ) = 3);
-- Zadanie 7
select extract( year from zatrudniony) as rok, count(*) as liczba 
			from pracownicy group by extract( year from zatrudniony) order by liczba DESC;
-- Zadanie 8
select rok, liczba from (select extract( year from zatrudniony) as rok, count(*) as liczba 
from pracownicy group by extract( year from zatrudniony)) 
where liczba = 
(select max(liczba) from 
(select extract( year from zatrudniony) as rok, count(*) as liczba from pracownicy p 
 group by extract( year from zatrudniony) 
));
-- Zadanie 9
select nazwisko, p.placa_pod, p.placa_pod - srednia as roznica from pracownicy p inner join (
  select avg(placa_pod) as srednia, id_zesp from pracownicy group by id_zesp) z
 on p.id_zesp = z.id_zesp order by nazwisko;
 -- Zadanie 10
select nazwisko, p.placa_pod, p.placa_pod - srednia as roznica from pracownicy p inner join (
  select avg(placa_pod) as srednia, id_zesp from pracownicy group by id_zesp) z
 on p.id_zesp = z.id_zesp where p.placa_pod - srednia > 0 order by nazwisko;	
 -- Zadanie 11
select nazwisko, z.podwladni from pracownicy p inner join 
 (select count(id_szefa) as podwladni, id_szefa from pracownicy group by id_szefa) z on p.id_prac = z.id_szefa 
 where p.etat = 'PROFESOR' and p.id_zesp in (select id_zesp from zespoly s where adres like 'PIOTROWO%')
 order by podwladni DESC;
 -- Zadanie 12
select nazwa, srednia_w_zespole, srednia_ogolna,
case 
when srednia_w_zespole < srednia_ogolna then ':('
when srednia_w_zespole > srednia_ogolna then ':)'
else '???' 
end as nastroje
from 
(select nazwa, avg(placa_pod) as srednia_w_zespole from pracownicy p 
full outer join zespoly z on p.id_zesp = z.id_zesp group by nazwa) a 
cross join 
(select avg(placa_pod) as srednia_ogolna from pracownicy) b;
-- Zadanie 13
select nazwa, placa_min, placa_max from etaty e order by 
(select count(*) as liczba from pracownicy p where p.etat = e.nazwa) DESC, nazwa;