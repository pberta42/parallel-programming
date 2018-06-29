/*
Meno:
Datum:

Simulujte nasledujucu situaciu. Piati casnici obsluhuju v restauracii. Styria kuchari varia v kuchyni.
Casnikovi trva nejaky cas, kym donesie objednavku od stola do kuchyne (v simulacii 1s). Potom caka na
kuchara, ktoremu chvilu trva, kym jedlo uvari (v simulacii 2s). Nasledne casnik donesie jedlo z kuchyne
zakaznikovi (v simulacii 1s). Cela simulacia nech trva nejaky cas (30s).

1. Doplnte do programu pocitadlo celkoveho poctu uvarenych jedal a tiez nech si kazdy casnik pocita, kolko
objednavok vybavil. Na konci simulacie vypiste hodnoty pocitadiel.

2. Ked casnik vybavi 2 objednavky, pocka na dalsich dvoch a spravia si prestavku (v simulacii 2s).

3. Osetrite v programe spravne ukoncenie simulacie hned po uplynuti stanoveneho casu (nezacne sa dalsia cinnost).


Poznamky:
- na synchronizaciu pouzite iba mutexy + podmienene premenne
- nespoliehajte sa na uvedene casy, simulacia by mala fungovat aj s inymi casmi
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int uvarene = 0;
int cakaju = 0;
int pauzuju = 0;
int objednane = 0;
int uvarene_akt = 0;

pthread_mutex_t uvar_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t pauza_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t varenie_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t uvarene_cond = PTHREAD_COND_INITIALIZER;

// signal na zastavenie
int stoj = 0;

// casnik - cesta do kuchyne a z kuchyne
void cesta(void) {
    sleep(1);
}

//  kuchyna - varenie
void varenie(void) {
    sleep(2);
}

//
void *kuchar( void *ptr ) {

    while(!stoj){

        pthread_mutex_lock(&uvar_mutex);
        while (objednane < 1){
            pthread_cond_wait(&varenie_cond,&uvar_mutex);
            if (stoj) {
                pthread_mutex_unlock(&cond_mutex);
                pthread_mutex_unlock(&uvar_mutex);
                pthread_cond_broadcast(&pauza_cond);
                pthread_cond_broadcast(&varenie_cond);
                pthread_cond_broadcast(&uvarene_cond);
                pthread_exit(NULL);
            }
        }

        objednane--;
        pthread_mutex_unlock(&uvar_mutex);

        printf("varim\n");
        varenie();
        if (stoj) {
            pthread_mutex_unlock(&cond_mutex);
            pthread_mutex_unlock(&uvar_mutex);
            pthread_cond_broadcast(&pauza_cond);
            pthread_cond_broadcast(&varenie_cond);
            pthread_cond_broadcast(&uvarene_cond);
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&uvar_mutex);

        uvarene_akt++;
        uvarene++;

        pthread_mutex_unlock(&uvar_mutex);
        pthread_cond_broadcast(&uvarene_cond);
    }
    return NULL;
}

// casnik
void *casnik( void *ptr ) {

    int mojCount = 0; // kolko som ja obsluzil

    // pokial nie je zastaveny
    while(!stoj) {
        printf("Zacinam %d\n",mojCount);

        cesta();
        if (stoj) {
            pthread_mutex_unlock(&cond_mutex);
            pthread_mutex_unlock(&uvar_mutex);
            pthread_cond_broadcast(&pauza_cond);
            pthread_cond_broadcast(&varenie_cond);
            pthread_cond_broadcast(&uvarene_cond);
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&uvar_mutex);
        objednane++;
        pthread_mutex_unlock(&uvar_mutex);

        pthread_cond_broadcast(&varenie_cond);


        pthread_mutex_lock(&uvar_mutex);
        while (uvarene_akt < 1){
            pthread_cond_wait(&uvarene_cond,&uvar_mutex);
            if (stoj) {
                pthread_mutex_unlock(&cond_mutex);
                pthread_mutex_unlock(&uvar_mutex);
                pthread_cond_broadcast(&pauza_cond);
                pthread_cond_broadcast(&varenie_cond);
                pthread_cond_broadcast(&uvarene_cond);
                pthread_exit(NULL);
            }
        }

        printf("Odnasam\n");
        uvarene_akt--;

        pthread_mutex_unlock(&uvar_mutex);
		cesta();
		if (stoj) {
            pthread_mutex_unlock(&cond_mutex);
            pthread_mutex_unlock(&uvar_mutex);
            pthread_cond_broadcast(&pauza_cond);
            pthread_cond_broadcast(&varenie_cond);
            pthread_cond_broadcast(&uvarene_cond);
            pthread_exit(NULL);
        }
        mojCount++;

		// idem na pauzu
		if (mojCount > 1){
            pthread_mutex_lock(&cond_mutex);
            printf("Idem cakat na pauzu\n");
            cakaju++;

            if (cakaju == 3){
                pthread_cond_broadcast(&pauza_cond);
            }
            else{
                while (cakaju < 3){
                    pthread_cond_wait(&pauza_cond,&cond_mutex);
                    if (stoj) {
                        pthread_mutex_unlock(&cond_mutex);
                        pthread_mutex_unlock(&uvar_mutex);
                        pthread_cond_broadcast(&pauza_cond);
                        pthread_cond_broadcast(&varenie_cond);
                        pthread_cond_broadcast(&uvarene_cond);
                        pthread_exit(NULL);
                    }
                }
            }

            printf("Pauzujem naozaj\n");

            // idem na pauzu
            pauzuju++;

            if (pauzuju == 3){
                cakaju = cakaju - 3;
                pauzuju = 0;
            }
            // pauzujem
            pthread_mutex_unlock(&cond_mutex);

            sleep(2);

            if (stoj) {
                pthread_mutex_unlock(&cond_mutex);
                pthread_mutex_unlock(&uvar_mutex);
                pthread_cond_broadcast(&pauza_cond);
                pthread_cond_broadcast(&varenie_cond);
                pthread_cond_broadcast(&uvarene_cond);
                pthread_exit(NULL);
            }

            mojCount = 0;
		}
    }
    return NULL;
}

int main(void) {
    int i;

    pthread_t casnici[5];
    pthread_t kuchari[4];

    for (i=0;i<5;i++) pthread_create(&casnici[i], NULL, &casnik, NULL);
    for (i=0;i<4;i++) pthread_create(&kuchari[i], NULL, &kuchar, NULL);

    sleep(30);
    stoj = 1;

    for (i=0;i<5;i++) pthread_join(casnici[i], NULL);
    for (i=0;i<4;i++) pthread_join(kuchari[i], NULL);

    printf("Dokopy sa uvarilo %d jedal\n", uvarene);

    exit(EXIT_SUCCESS);
}
