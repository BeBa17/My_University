#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <time.h>

#define MAX_KSIAZEK 10
#define MUTEX 0
#define CZYTELNICY 1
#define MIEJSCE_NA_POLCE 2
#define KSIAZEK 3
#define LICZBA_PROCESOW 10 
#define ROZMIAR_TABLICY LICZBA_PROCESOW+1
#define KLUCZ_KOL 4646
#define KLUCZ_SEM 1443

static struct sembuf buff;
int shmid1, shmid2, shmid3, semid, msgid, shmid5;
pid_t wpid;
int *buf;
int *procesy;
int *tablicaKsiazek;
int* dane;
int id;
struct msgbuf msg, msg2;
struct msqid_ds mbuf;
int numberOfBooks;

void pisarz(int);
void czytelnik(int);
void czytaj(int,int);
void napiszKsiazke(int);
int findMax(int);
void relaks();
void odczyt();
int rejestrCzytelnicy(int);
int rejestrCzytelnik(int);

void V(int semnum, int wartosc){
   buff.sem_num = semnum;
   buff.sem_op = wartosc;
   buff.sem_flg = 0;
   if (semop(semid, &buff, 1) == -1){
      perror("Podnoszenie semafora");
      exit(1);
   }
}

void P(int semnum, int wartosc){
   buff.sem_num = semnum;
   buff.sem_op = -wartosc;
   buff.sem_flg = 0;
   if (semop(semid, &buff, 1) == -1){
       perror("Opuszczanie semafora");
      exit(1);
     }
}

void fill(int *p, int lP){
    int i, j;
    int rows = lP + 1;
    int cols = lP + 1;
    for(i=0; i<rows; i++)
    {
        for(j=0;j<cols;j++)
        {
            *(p + i*cols + j) = 0;
        }
    }
}

struct msgbuf {
  long mtype;
  int mvalue;
}

main()
{
  system("ipcrm -a");
  wpid = getpid();
  shmid1 = shmget(43000, sizeof(int), IPC_CREAT|0600);
  if (shmid1 == -1){
    perror("Utworzenie segmentu pamieci wspoldzielonej");
    exit(1);
  }
  shmid2 = shmget(44000, ROZMIAR_TABLICY*sizeof(int), IPC_CREAT|0600);
  if (shmid2 == -1){
    perror("Utworzenie segmentu pamieci wspoldzielonej");
    exit(1);
  }
  shmid3 = shmget(60000, ROZMIAR_TABLICY*ROZMIAR_TABLICY*sizeof(int), IPC_CREAT|0600);
  if (shmid3 == -1){
    perror("Utworzenie segmentu pamieci wspoldzielonej");
    exit(1);
  }
  shmid5 = shmget(43500, 5*sizeof(int), IPC_CREAT|0600);
  if (shmid5 == -1){
    perror("Utworzenie segmentu pamieci wspoldzielonej");
    exit(1);}
  buf = (int*)shmat(shmid1, NULL, 0);
  if (buf == NULL){
  perror("Przylaczenie segmentu pamieci wspoldzielonej1");
  exit(1);
  }
  procesy = (int*)shmat(shmid2, NULL, 0);
  if (procesy == NULL){
  perror("Przylaczenie segmentu pamieci wspoldzielonej2");
  exit(1);
  }
  tablicaKsiazek = (int*)shmat(shmid3,NULL,0);
  if (tablicaKsiazek == NULL){
  perror("Przylaczenie segmentu pamieci wspoldzielonej3");
  exit(1);
  }
  fill(tablicaKsiazek, ROZMIAR_TABLICY);
  dane = (int*)shmat(shmid5, NULL,0);
  if (dane == NULL){
  perror("Przylaczenie segmentu pamieci wspoldzielonej3");
  exit(1);}
  dane[0]=0; dane[1]=0; dane[2]=0; dane[3]=0; dane[4]=0;

  msgid = msgget(KLUCZ_KOL, IPC_CREAT|0600);
  if (msgid == -1){
  perror("Utworzenie kolejki komunikatow");
  exit(1);}
  else
  { printf("ID kolejki: %d\n", msgid);}
  msgctl(msgid, IPC_STAT, &mbuf);
  numberOfBooks = (int)(mbuf.msg_qnum);
  semid = semget(KLUCZ_SEM, 4, IPC_CREAT|0600);
  if (semid == -1){
  perror("Utworzenie tablicy semaforow");
  exit(1);}
  else{
  if (semctl(semid, 0, SETVAL, (int)1) == -1){ // MUTEX
  perror("Nadanie wartosci semaforowi 0");
  exit(1);}
  if (semctl(semid, 1, SETVAL, (int)MAX_KSIAZEK) == -1){ // CZYTELNICY
  perror("Nadanie wartosci semaforowi 1");
  exit(1);}
  if (semctl(semid, 2, SETVAL, (int)MAX_KSIAZEK) == -1){ // MIEJSC_NA_POLCE
  perror("Nadanie wartosci semaforowi 2");
  exit(1);}
  if (semctl(semid, 3, SETVAL, (int)0) == -1){ // KSIAZEK
  perror("Nadanie wartosci semaforowi 3");
  exit(1);}
  }
  srand(time(NULL));
  for(int j=1;j<=LICZBA_PROCESOW;j++)
  {
  switch(fork())
  {
  case -1: 
  perror("Blad utworzenia procesu potomnego");
  break;
  case 0:
  shmat(shmid1,getpid(),SHM_RND);
  shmat(shmid2,getpid(),SHM_RND);
  shmat(shmid3,getpid(),SHM_RND);
  shmat(shmid5,getpid(),SHM_RND);
  id = j;
  while(true){
    int rand_val1, rand_val2;
    rand_val1 = (rand()%1000)/(id);
    if(rand_val1%2==0){
      rand_val2 = (rand()%1000)/(id+1);
      if(rand_val2%2==0){
        procesy[id]=1;
        sleep(1);
        pisarz(id);}
      else{
      procesy[id]=2;
      sleep(2);
      czytelnik(id);
      }
    }
    else
    {
      sleep(1);
      procesy[id]=0;
      relaks();}
    } 
  exit(0);
  }
  }
  odczyt();
  while( ( wpid = wait(0)) > 0);
}

void odczyt(){
  P(MUTEX,1);
  msgctl(msgid, IPC_STAT, &mbuf);
  int numberOfBooks = (int)(mbuf.msg_qnum);
  if ((dane[3]==LICZBA_PROCESOW)&&(numberOfBooks==MAX_KSIAZEK))
  {
    printf("\t \t \t \t \t \t \t ZAKLESZCZENIE \n");
  }
  if ((dane[4]==LICZBA_PROCESOW)&&(numberOfBooks==0))
  {
    printf("\t \t \t \t \t \t \t ZAKLESZCZENIE \n");
  }
  if ((dane[0]!=0)&&(numberOfBooks==0))
  {
    printf("\t \t \t \t \t \t \t ZAKLESZCZENIE \n");
  }
  printf("Czytelników(czytelnia): %d \t Pisarzy(czytelnia): %d \t Czytelników(czytelnia+oczekiwanie): %d \t Pisarzy(czytelnia+oczekiwanie): %d \t Relaks: %d \t Książek: %d \n", dane[0], dane[1], dane[4], dane[3], dane[2], numberOfBooks);

  V(MUTEX,1);
}

int findMax(int id)
{
  int temp = 0;
  int max = 0;
  for(int i=1; i<ROZMIAR_TABLICY; i++)
    {
	    temp = *(tablicaKsiazek + id*ROZMIAR_TABLICY + i);
      if (temp>max)
      {
        max = temp;
      }
    }
  return max;
}

int rejestrCzytelnicy(int id) // przypisuje kto musi przeczytać daną książkę
{
  int ileOsobMusiPrzeczytac = 0;
  for(int i=1; i<ROZMIAR_TABLICY; i++)
    {
      if(procesy[i]==2)
	    {
	      *(tablicaKsiazek + id*ROZMIAR_TABLICY + i)+=1; // set to all people in library this book
        ileOsobMusiPrzeczytac += 1;
	    }
    }

  return ileOsobMusiPrzeczytac;
}

int rejestrCzytelnik(int id)
{
  for(int i=1; i<ROZMIAR_TABLICY; i++)
    {
      if(*(tablicaKsiazek + i*ROZMIAR_TABLICY + id)!=0){ // if reader must read some book, we return the type
        *(tablicaKsiazek + i*ROZMIAR_TABLICY + id)-=1;
	return i;}
    }
  return 0; // first type
}

void relaks()
{
  dane[2]+=1;
  odczyt();
  int czas_relaksu=rand()%3;
  sleep(czas_relaksu);
  dane[2]-=1;
}

void czytajPis(int id)
{
  msgctl(msgid, IPC_STAT, &mbuf);
  numberOfBooks = (int)(mbuf.msg_qnum);
  if(numberOfBooks==0) // pisarz - gdy nie ma co czytać to może nic nie czytać
  {
    return;
  }
  else{
    P(KSIAZEK,1);
    int doPrzeczytania = rejestrCzytelnik(id); // typ wiadomości tj. indeks autora
    msgrcv(msgid,&msg, sizeof(msg.mvalue),doPrzeczytania,0);
    msg2.mtype = msg.mtype;
    if (doPrzeczytania==0)
  {
    msg2.mvalue = msg.mvalue;
    msgsnd(msgid,&msg2,sizeof(msg2.mvalue),0);
    V(KSIAZEK,1); // oddaję książkę
  }
  else{
    msg2.mvalue = msg.mvalue - 1; // bo już jedna osoba mniej musi daną książkę przeczytać
    if (msg2.mvalue==0)
    {
      V(MIEJSCE_NA_POLCE,1); // już nie dodamy tej książki ponownie
    }
    else{
    msgsnd(msgid,&msg2,sizeof(msg2.mvalue),0);
    V(KSIAZEK,1);
    }
    }
  }
  int czas_czytania = rand()%5;
  sleep(czas_czytania);
}

void czytajCzyt(int id)
{
  // czytanie
  int czas_czytania = rand()%5;
  sleep(czas_czytania);
  // czytanie
  P(MUTEX,1);
  int doPrzeczytania = rejestrCzytelnik(id); // typ wiadomości tj. indeks autora
  msgrcv(msgid,&msg, sizeof(msg.mvalue),doPrzeczytania,0);
  msg2.mtype = msg.mtype;
  if (doPrzeczytania==0)
  {
    msg2.mvalue = msg.mvalue;
    msgsnd(msgid,&msg2,sizeof(msg2.mvalue),0);
    V(KSIAZEK,1); // oddaję książkę
  }
  else{
    msg2.mvalue = msg.mvalue - 1; // bo już jedna osoba mniej musi daną książkę przeczytać
    if (msg2.mvalue==0)
    {
      V(MIEJSCE_NA_POLCE,1); // już nie dodamy tej książki ponownie
    }
    else{
    msgsnd(msgid,&msg2,sizeof(msg2.mvalue),0);
    V(KSIAZEK,1);
    }
  }
  V(MUTEX,1);
}

void dodajKsiazke(int id)
{
  int autor = id;
  int wiadomosc = rejestrCzytelnicy(autor);
  msg.mtype = autor;
  msg.mvalue = wiadomosc;  // traścią wiadomości jest ile osób musi przeczytać tą książkę
  msgsnd(msgid,&msg,sizeof(msg.mvalue),0);
  V(KSIAZEK,1);
}

void pisarz(int id)
{
  dane[3]+=1;
  odczyt();
  P(MIEJSCE_NA_POLCE, 1);
  P(CZYTELNICY,LICZBA_PROCESOW);
  dane[1]+=1;
  // w czytelni
  czytajPis(id);
  int czas_pisania = rand()%5;
  sleep(czas_pisania);
  dodajKsiazke(id);
  dane[1]-=1;
  // w czytelni
  odczyt();
  V(CZYTELNICY,LICZBA_PROCESOW);
  dane[3]-=1;  
}

void czytelnik(int id)
{
  dane[4]+=1;
  P(KSIAZEK,1);
  V(KSIAZEK,1);
  P(CZYTELNICY, 1);
  P(KSIAZEK,1);
  dane[0]+=1;
  // w czytelni
  czytajCzyt(id);
  //w czytelni
  dane[0]-=1;
  odczyt();
  V(CZYTELNICY, 1);
  dane[4]-=1;
}


