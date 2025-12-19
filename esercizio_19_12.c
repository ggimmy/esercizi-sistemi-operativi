#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*
Il processo principale crea inizialmente un buffer, implementato come un array di 11 numeri interi, inizializzati a zero. Successivamente, il processo genera tre thread utilizzando le librerie POSIX Threads (pthread), con il seguente comportamento:

Primo thread: In un ciclo infinito, sceglie casualmente una cella del buffer e vi scrive il valore +1, indipendentemente dal valore precedentemente contenuto nella cella.

Secondo thread: In un ciclo infinito, sceglie casualmente una cella del buffer e vi scrive il valore −1, indipendentemente dal valore precedentemente contenuto nella cella.

Terzo thread: In un ciclo infinito, controlla se tutte le celle del buffer contengono un valore diverso da zero. In caso affermativo:
    determina se il numero di celle contenenti il valore +1 è maggiore del numero di celle contenenti −1;
    termina l’esecuzione di tutti e tre i thread.

Vincoli di sincronizzazione
    L’accesso al buffer deve avvenire in mutua esclusione: mentre un thread accede al buffer, nessun altro thread deve potervi accedere.
    Dopo ogni accesso in lettura o scrittura al buffer, ciascun thread deve sospendere la propria esecuzione per un numero di secondi casuale compreso tra 0 e 3.
 */

int kill_var = 0; //variabile globale e condivisa utilizzata come flag per terminare i thread

pthread_mutex_t mutex; //inizializzo mutex

//routine per il primo thread
void* t1_routine(void* arg){

    int* buffer = (int*)arg;

    while(1){

        if(kill_var){
            pthread_exit(NULL); //controllo flag terminazione
        }

        int i = (rand() % 11);

        if(pthread_mutex_trylock(&mutex) == 0){ //tento accesso al mutex
            printf("Mutex sbloccato nel primo thread, eseguo...\n");
            buffer[i] = 1;
        }

        printf("Primo thread ha finito, rilascia mutex...\n");
        pthread_mutex_unlock(&mutex); //rilascio mutex
        sleep( (rand() % 4) );
   }

}

//routine secondo thread
void* t2_routine(void* arg){

    int* buffer = (int*)arg;

    while(1){

        if(kill_var){
            pthread_exit(NULL); //controllo flag terminazione
        }

        int i = (rand() % 11);

        if(pthread_mutex_trylock(&mutex) == 0){ //tento accesso al mutex
            printf("Mutex sbloccato nel secondo thread, eseguo...\n");
            buffer[i] = -1;
        }

        printf("Secondo thread ha finito, rilascia mutex...\n");
        pthread_mutex_unlock(&mutex);
        sleep( (rand() % 4) );
   }

}

//routine terzo thread
void* t3_routine(void* arg){

    int* buffer = (int*)arg;

    while(1){

        int plus = 0;
        int minus = 0;

        int i = 0;
        if(pthread_mutex_trylock(&mutex) == 0){

            printf("Mutex sbloccato nel terzo thread, eseguo...\n");
            while(i < 11){
                if(buffer[i] == 1){

                    plus++;

                } else if(buffer[i] == -1){

                    minus++;

                } else {

                    break;

                }

                i++;
            }
        }

        //se tutte le celle sono scritte...
        if((plus + minus) == 11 && plus > minus){
            printf("Numero di celle con +1: %d\nNumero di celle con -1: %d\n", plus, minus);
            kill_var = 1; //flag terminazione
            printf("Terzo thread ha finito, rilascia mutex...\n");
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL); //termino questo thread
        } else {
            printf("Terzo thread ha finito, rilascia mutex...\n");
            pthread_mutex_unlock(&mutex);
        }

        sleep( (rand() % 4) );

    }

}


int main(int argc, char* argv[]){

    int buffer[11] = {0};

    pthread_t threads[3];
    int status;

    //creazione primo thread
    status = pthread_create(&threads[0], NULL, t1_routine, (void *)buffer);
    if(status != 0){
        fprintf(stderr, "Errore in creazione thread %ld",threads[0]);
        exit(-1);
    }

    //creazione secondo thread
    status = pthread_create(&threads[0], NULL, t2_routine, (void *)buffer);
    if(status != 0){
        fprintf(stderr, "Errore in creazione thread %ld",threads[1]);
        exit(-1);
    }

    //creazione terzo thread
    status = pthread_create(&threads[0], NULL, t3_routine, (void *)buffer);
    if(status != 0){
        fprintf(stderr, "Errore in creazione thread %ld",threads[2]);
        exit(-1);
    }

    //attendo che i thread concludano le loro routine
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %d terminato\n", i);
    }

    printf("Programma terminato\n");


    return 0;

}
