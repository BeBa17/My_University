#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define MIEJSC_W_PORCIE 250
#define HOLOWNIKOW 250
#define LICZBA_PROCESOW 300

pthread_mutex_t mutex;
pthread_cond_t cond_miejsc;
pthread_cond_t cond_holownikow;

int licznik_miejsc = MIEJSC_W_PORCIE;
int licznik_holownikow = HOLOWNIKOW;
void wyswietl();

struct thread_data
{
    int thread_id, masa;
};

void P_miejsc()
{
    pthread_mutex_lock(&mutex);
    while (licznik_miejsc==0)
        pthread_cond_wait(&cond_miejsc, &mutex);
    licznik_miejsc--;
    wyswietl();
    pthread_mutex_unlock(&mutex);
}

void V_miejsc()
{
    pthread_mutex_lock(&mutex);
    licznik_miejsc++;
    pthread_cond_signal(&cond_miejsc);
    wyswietl();
    pthread_mutex_unlock(&mutex);
}

void P_holownikow(int potrzebna_ilosc)
{
    pthread_mutex_lock(&mutex);
    while (licznik_holownikow<potrzebna_ilosc)
        pthread_cond_wait(&cond_holownikow, &mutex);
    licznik_holownikow-=potrzebna_ilosc;
    pthread_mutex_unlock(&mutex);
}

void V_holownikow(int potrzebna_ilosc)
{
    pthread_mutex_lock(&mutex);
    licznik_holownikow+=potrzebna_ilosc;
    pthread_cond_broadcast(&cond_holownikow);
    pthread_mutex_unlock(&mutex);
}

void wyswietl()
{
    system("clear");
    printf("Ogolnie: \n Statkow: %d\t Miejsc w porcie: %d\t Holownikow %d\nAktualnie wolnych:\n Miejsc: %d\t Holownikow: %d\n",
    LICZBA_PROCESOW, MIEJSC_W_PORCIE, HOLOWNIKOW, licznik_miejsc, licznik_holownikow);
}

void *statek(void *threadarg)
{
    struct thread_data *my_data;   
    
    my_data = (struct thread_data *) threadarg;

    while(1){
        
        int czas_postoju = rand()%40;
        int czas_wplywania = rand()%20;
        int czas_wyplywania = rand()%20;
        int czas_rejsu = rand()%60;
        int potrzebna_ilosc_holownikow = (my_data->masa - 1)/1000 + 1;
        P_miejsc();
        P_holownikow(potrzebna_ilosc_holownikow);
        sleep(czas_wplywania);
        V_holownikow(potrzebna_ilosc_holownikow);
        wyswietl();
        sleep(czas_postoju);
        P_holownikow(potrzebna_ilosc_holownikow);
        sleep(czas_wyplywania);
        V_holownikow(potrzebna_ilosc_holownikow);
        V_miejsc();
        sleep(czas_rejsu);
    }

   pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));

    pthread_t t_statkow[LICZBA_PROCESOW];
    struct thread_data td[LICZBA_PROCESOW];

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond_miejsce = PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond_holownikow = PTHREAD_COND_INITIALIZER;

    for(int i=0; i<LICZBA_PROCESOW; i++)
    {
        td[i].thread_id = i+1;
        td[i].masa = rand()%(1000*HOLOWNIKOW);

        if(pthread_create(&t_statkow, NULL, statek, (void *)&td[i])) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }
    pthread_exit(NULL);
    return 0;
}