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
#include <unistd.h>


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

// kuchar
void *kuchar(void *ptr) {

	// pokial nie je zastaveny
	while (!stoj) {
		// cakanie na casnika
		varenie();
	}
	return NULL;
}

// casnik
void *casnik( void *ptr ) {

    // pokial nie je zastaveny
    while(!stoj) {
        cesta();
        // cakanie na uvarenie
		cesta();
    }
    return NULL;
}

int main(void) {
	int i;

	pthread_t casnici[5];
	pthread_t kuchari[4];

	for (i = 0;i<5;i++) pthread_create(&casnici[i], NULL, &casnik, NULL);
	for (i = 0;i<4;i++) pthread_create(&kuchari[i], NULL, &kuchar, NULL);

	sleep(30);
	stoj = 1;

	for (i = 0;i<5;i++) pthread_join(casnici[i], NULL);
	for (i = 0;i<4;i++) pthread_join(kuchari[i], NULL);

	printf("Dokopy sa uvarilo %d jedal\n", uvarene);

	exit(EXIT_SUCCESS);
}
