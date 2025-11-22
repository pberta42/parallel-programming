#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int cooked = 0;
int waiting = 0;
int pausing = 0;
int ordered = 0;
int cooked_act = 0;

pthread_mutex_t cook_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t pause_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cooking_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cooked_cond = PTHREAD_COND_INITIALIZER;

// signal for stopping
int stop = 0;

// waiter - walk to kitchen and back
void walk(void) {
    printf("walking\n");
    sleep(1);
}

//  kitchen - cooking
void cooking(void) {
    printf("cooking\n");
    sleep(2);
}

// the chef
void *chef( void *ptr ) {

    while(!stop){

        pthread_mutex_lock(&cook_mutex);
        while (ordered < 1){
            pthread_cond_wait(&cooking_cond,&cook_mutex);
            if (stop) {
                pthread_mutex_unlock(&cond_mutex);
                pthread_mutex_unlock(&cook_mutex);
                pthread_cond_broadcast(&pause_cond);
                pthread_cond_broadcast(&cooking_cond);
                pthread_cond_broadcast(&cooked_cond);
                pthread_exit(NULL);
            }
        }

        ordered--;
        pthread_mutex_unlock(&cook_mutex);

        cooking();
        if (stop) {
            pthread_mutex_unlock(&cond_mutex);
            pthread_mutex_unlock(&cook_mutex);
            pthread_cond_broadcast(&pause_cond);
            pthread_cond_broadcast(&cooking_cond);
            pthread_cond_broadcast(&cooked_cond);
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&cook_mutex);

        cooked_act++;
        cooked++;

        pthread_mutex_unlock(&cook_mutex);
        pthread_cond_broadcast(&cooked_cond);
    }
    return NULL;
}

// waiter
void *waiter( void *ptr ) {

    int my_count = 0; // how many did i serve

    // while i am not stopped
    while(!stop) {
        printf("Starting %d\n", my_count);

        walk();
        if (stop) {
            pthread_mutex_unlock(&cond_mutex);
            pthread_mutex_unlock(&cook_mutex);
            pthread_cond_broadcast(&pause_cond);
            pthread_cond_broadcast(&cooking_cond);
            pthread_cond_broadcast(&cooked_cond);
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&cook_mutex);
        ordered++;
        pthread_mutex_unlock(&cook_mutex);

        pthread_cond_broadcast(&cooking_cond);


        pthread_mutex_lock(&cook_mutex);
        while (cooked_act < 1){
            pthread_cond_wait(&cooked_cond,&cook_mutex);
            if (stop) {
                pthread_mutex_unlock(&cond_mutex);
                pthread_mutex_unlock(&cook_mutex);
                pthread_cond_broadcast(&pause_cond);
                pthread_cond_broadcast(&cooking_cond);
                pthread_cond_broadcast(&cooked_cond);
                pthread_exit(NULL);
            }
        }

        printf("Bringing the food\n");
        cooked_act--;

        pthread_mutex_unlock(&cook_mutex);
		walk();
		if (stop) {
            pthread_mutex_unlock(&cond_mutex);
            pthread_mutex_unlock(&cook_mutex);
            pthread_cond_broadcast(&pause_cond);
            pthread_cond_broadcast(&cooking_cond);
            pthread_cond_broadcast(&cooked_cond);
            pthread_exit(NULL);
        }
        my_count++;

		// going on a break
		if (my_count > 1){
            pthread_mutex_lock(&cond_mutex);
            printf("Going to wait for a break\n");
            waiting++;

            if (waiting == 3){
                pthread_cond_broadcast(&pause_cond);
            }
            else{
                while (waiting < 3){
                    pthread_cond_wait(&pause_cond,&cond_mutex);
                    if (stop) {
                        pthread_mutex_unlock(&cond_mutex);
                        pthread_mutex_unlock(&cook_mutex);
                        pthread_cond_broadcast(&pause_cond);
                        pthread_cond_broadcast(&cooking_cond);
                        pthread_cond_broadcast(&cooked_cond);
                        pthread_exit(NULL);
                    }
                }
            }

            printf("I am on break for real\n");

            // going on a break
            pausing++;

            if (pausing == 3){
                waiting = waiting - 3;
                pausing = 0;
            }
            // I am on break
            pthread_mutex_unlock(&cond_mutex);

            sleep(2);

            if (stop) {
                pthread_mutex_unlock(&cond_mutex);
                pthread_mutex_unlock(&cook_mutex);
                pthread_cond_broadcast(&pause_cond);
                pthread_cond_broadcast(&cooking_cond);
                pthread_cond_broadcast(&cooked_cond);
                pthread_exit(NULL);
            }

            my_count = 0;
		}
    }
    return NULL;
}

int main(void) {
    int i;

    pthread_t waiters[5];
    pthread_t chefs[4];

    for (i=0;i<5;i++) pthread_create(&waiters[i], NULL, &waiter, NULL);
    for (i=0;i<4;i++) pthread_create(&chefs[i], NULL, &chef, NULL);

    sleep(30);
    stop = 1;

    for (i=0;i<5;i++) pthread_join(waiters[i], NULL);
    for (i=0;i<4;i++) pthread_join(chefs[i], NULL);

    printf("Today we cooked %d meals\n", cooked);

    exit(EXIT_SUCCESS);
}
