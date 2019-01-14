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

#define MAX 50
#define uzycie 0
#define LICZBA_PROCESOW 30
#define MINIMALNA_LICZBA_KSIAZEK 0
#define ROZMIAR_TABLICY LICZBA_PROCESOW+1

static struct sembuf buff;
int shmid1, shmid2, shmid3, semid, msgid;
pid_t wpid;
int *buf;
int *procesy;
int *tablicaKsiazek;
int id;
struct msgbuf msg, rcv;
struct msqid_ds mbuf;
uint msgg;

void pisarz(int);
void czytelnik(int);
void czytaj(int);
void napiszKsiazke(int);
void relaks();
void wyswietl();
void rejestrCzytelnicy(int);
bool rejestrPisarz(int);
int rejestrCzytelnik(int);

void podnies(int semid, int semnum){
   buff.sem_num = semnum;
   buff.sem_op = 1;
   buff.sem_flg = 0;
   if (semop(semid, &buff, 1) == -1){
      perror("Podnoszenie semafora");
      exit(1);
   }
}

void opusc(int semid, int semnum){
   buff.sem_num = semnum;
   buff.sem_op = -1;
   buff.sem_flg = IPC_NOWAIT;
   /*if (semop(semid, &buff, 1) == -1){
      perror("Opuszczenie semafora");
      exit(1);
      }*/
   if (semop(semid, &buff, 1) == -1)
     {
       return;
     }
}

void fill(int *p, int lP)
{
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


  main(){
#define KLUCZ_KOL 4646
#define KLUCZ_SEM 34343

    system("ipcrm -a");
    wpid = getpid();
    shmid1 = shmget(45281, sizeof(int), IPC_CREAT|0600);
    if (shmid1 == -1){
      perror("Utworzenie segmentu pamieci wspoldzielonej");
      exit(1);
    }
    shmid2 = shmget(45981, ROZMIAR_TABLICY*sizeof(int), IPC_CREAT|0600);
    if (shmid1 == -1){
      perror("Utworzenie segmentu pamieci wspoldzielonej");
      exit(1);
    }
    shmid3 = shmget(48281, ROZMIAR_TABLICY*sizeof(int), IPC_CREAT|0600);
    if (shmid1 == -1){
      perror("Utworzenie segmentu pamieci wspoldzielonej");
      exit(1);
    }

    
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
  
    msgid = msgget(KLUCZ_KOL, IPC_CREAT|0600);
    if (msgid == -1){
      perror("Utworzenie kolejki komunikatow");
      exit(1);
    }
    else
      { printf("ID kolejki: %d\n", msgid);}

    msgctl(msgid, IPC_STAT, &mbuf);
    msgg = (uint)(mbuf.msg_qnum);
    semid = semget(KLUCZ_SEM, 1, IPC_CREAT|0600);
    if (semid == -1){
      perror("Utworzenie tablicy semaforow");
      exit(1);
    }
    else{
      if (semctl(semid, 0, SETVAL, (int)1) == -1){ //do nieprzerywania zapisu, uzycie
	perror("Nadanie wartosci semaforowi 0");
	exit(1);
      }
    }
    srand(time(NULL));
    for(int j=0;j<LICZBA_PROCESOW;j++)
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
	    id = j+1;
	    while(true){
	      int rand_val1, rand_val2;
	      rand_val1 = (rand()%1000)/(id);
	      if(rand_val1%2==0){
	
		rand_val2 = (rand()%1000)/(id+1);
		if(rand_val2%2==0)
		  {
		    procesy[id]=1;
		    printf("pisarz id %d\n", id);
		    wyswietl();
		    pisarz(id);
		  }
		else
		  {
		    procesy[id]=2;
		    printf("czytelnik id %d\n", id);
		    //wyswietl();
		    czytelnik(id);
		  }
	      }
	      else
		{
		  procesy[id]=0;
		  printf("relaks id %d\n", id);
		  //wyswietl();
		  relaks();
		}
	      
	    } //end while
	    exit(0);
	  } // end switch
	
      } // end for
    while( ( wpid = wait(0)) > 0);
  }

// buf[0] - wartosc elementu, buf[1] - ilość książek, buf[2] - ilość pisarzy, buf[3] - ilość czytelników

void wyswietl()
{
  opusc(semid, uzycie);
  system("clear");
  printf("\n");
  for (int i = 1; i < ROZMIAR_TABLICY; i++) {
    for (int j = 1; j < ROZMIAR_TABLICY; j++) {
      printf("%d ", *(tablicaKsiazek + i * ROZMIAR_TABLICY + j));
    }
  printf("\n");
  }
  podnies(semid, uzycie);
}

void rejestrCzytelnicy(int id)
{
  for(int i=0; i<ROZMIAR_TABLICY; i++)
    {
      if(procesy[i]==2)
	{
	  *(tablicaKsiazek + i*ROZMIAR_TABLICY + id)=1; // set to all people in library this book
	}
    }
}

bool rejestrPisarz(int id)
{
  for(int i=0; i<ROZMIAR_TABLICY; i++)
    {
      if(*(tablicaKsiazek + i*ROZMIAR_TABLICY + id)==1) // if he has got some unread book
	return false;
    }
  return true; // if he hasn't
}

int rejestrCzytelnik(int id)
{
  for(int i=0; i<ROZMIAR_TABLICY; i++)
    {
      if(*(tablicaKsiazek + id*ROZMIAR_TABLICY + i)==1){ // if reader must read some book, we return the type
	*(tablicaKsiazek + id*ROZMIAR_TABLICY + i) = 0;
	return i;}
    }
  return 0; // first type
}

void relaks()
{
  int czas_relaksu=rand()%30;
  sleep(czas_relaksu);
}

void czytaj(int id)
{
  
  opusc(semid,uzycie);
  msgctl(msgid, IPC_STAT, &mbuf);
  msgg = (uint)(mbuf.msg_qnum);
  if(msgg<(MINIMALNA_LICZBA_KSIAZEK+1))
    {
      podnies(semid,uzycie);
      return;
    }
  int doPrzeczytania = rejestrCzytelnik(id); // typ wiadomości tj. indeks autora
  msgrcv(msgid,&msg, sizeof(msg.mvalue),doPrzeczytania,IPC_NOWAIT);
  printf("Odczyt: %d\n", msg.mvalue);
  msg.mtype = doPrzeczytania;
  msgsnd(msgid,&msg,sizeof(msg.mvalue),0);
  msgctl(msgid, IPC_STAT, &mbuf);
  msgg = (uint)(mbuf.msg_qnum);
  printf("Aktualna ilosc ksiazek: %d\n", msgg);
  podnies(semid,uzycie);

  int czas_czytania = rand()%15;
  sleep(czas_czytania);
  
}

void napiszKsiazke(int id)
{
  int autor = id;
  
  opusc(semid,uzycie);
  msgctl(msgid, IPC_STAT, &mbuf);
  msgg = (uint)(mbuf.msg_qnum);
  if(msgg==MAX)
    {
      podnies(semid,uzycie);
      return;
    }
  if(!rejestrPisarz(autor))
    {
      podnies(semid,uzycie);
      return;
    }
  msgrcv(msgid,&msg, sizeof(msg.mvalue),id,IPC_NOWAIT); // delete old one
  msg.mtype = autor;
  msg.mvalue = buf[0];
  msgsnd(msgid,&msg,sizeof(msg.mvalue),0);
  printf("Zapis %d\n", buf[0]);
  //buf[0] = (*buf+1)%MAX;
  buf[0]=11;
  rejestrCzytelnicy(autor);
  msgctl(msgid, IPC_STAT, &mbuf);
  msgg = (uint)(mbuf.msg_qnum);
  printf("Aktualna ilosc ksiazek: %d\n", msgg);
  podnies(semid,uzycie);

  int czas_pisania = rand()%10;
  sleep(czas_pisania);
}
void pisarz(int id)
{
    czytaj(id);
    napiszKsiazke(id);
}

void czytelnik(int id)
{
  
    czytaj(id);
}


