#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * Il processo padre deve generare due processi. I due processi generano numeri casuali da 0 a 100 e p1 comunica con padre se numero è disparo, p2 se numero è pari, il padre per ogni coppia di numeri
 * ne fa la somma.
 *
 */

 #define PIPE_RD 0
 #define PIPE_WR 1

int main(){


    pid_t processo1;
    pid_t processo2;

    int fd_pipe1[2]; //pipe per il primo processo
    int fd_pipe2[2]; //pipe per il secondo processo
    pipe(fd_pipe1);  //inizializzo pipe primo processo
    pipe(fd_pipe2); //inizializzo pipe secondo processo


    processo1 = fork(); //creo processo 1
    if(processo1 < 0){ //se fallisce creazione, esco.
        printf("Errore nella creazione del processo!\n");
        exit(EXIT_FAILURE);
    }
    else if(processo1 == 0){ //altrimenti...
        close(fd_pipe1[PIPE_RD]); //chiudo pipe di lettura (devo scrivere quindi non serve);
        close(fd_pipe2[PIPE_RD]); //chiudo pipe2, non mi serve
        close(fd_pipe2[PIPE_WR]);

        while(1){
            int num = rand() % 100; //genero un numero casuale da 0 a 100
            if(num % 2 != 0){ //se disparo
                write(fd_pipe1[PIPE_WR], &num, sizeof(num)); //scrivo nel file descriptor di scrittura della pipe
                printf("Inviato %d al processo padre da P1\n", num);
                pid_t this = getpid(); //ottengo il pid di questo processo per gestione segnale
                kill(this, SIGSTOP); //stoppo questo processo, aspettando che venga "risvegliato" dal padre

            }

            sleep(2); //rallento l'esecuzione per "debugging visivo"


        }

    }

    processo2 = fork(); //creo secondo processo
    if(processo2 < 0){ //se fallisce creazione, esco
        printf("Errore nella creazione del processo!\n");
        exit(EXIT_FAILURE);
    }
    else if(processo2 == 0){ //altrimenti

        close(fd_pipe2[PIPE_RD]); //chiudo pipe in lettura
        close(fd_pipe1[PIPE_RD]); //chiudo pipe 1, non mi serve
        close(fd_pipe1[PIPE_WR]);

        while(1){
            int num = rand() % 100; //genero un numero casuale da 0 a 100
            if(num % 2 == 0){
                write(fd_pipe2[PIPE_WR], &num, sizeof(num)); //scrivo del file descriptor di scrittura della pipe
                printf("Inviato %d al processo padre da P2\n", num);
                pid_t this = getpid(); //ottengo il pid di questo processo per gestione segnale
                kill(this, SIGSTOP); //stoppo questo processo, aspettando che venga "risvegliato dal padre"
            }

            sleep(2); //ralletto l'esecuzione per "debbuging visivo"

        }

        }

    //PROCESSO PADRE
    close(fd_pipe1[PIPE_WR]); //chiudo le pipe in scrittura
    close(fd_pipe2[PIPE_WR]);

    int n1, n2;
    while(1){
        read(fd_pipe1[PIPE_RD], &n1, sizeof(n1)); //leggo i numeri scritti nelle pipe dai processi
        read(fd_pipe2[PIPE_RD], &n2, sizeof(n2));

        int sum = n1 + n2;
        printf("PADRE riceve %d e %d -> somma = %d\n", n1, n2, sum);
        if (sum > 190) { //se la somma supera 190, esco e termino i processi
                    printf("Somma > 190! termino...\n");
                    kill(processo1, SIGKILL);
                    kill(processo2, SIGKILL);
                    break;
                }
        else{ //altrimenti, li risveglio
            kill(processo1, SIGCONT);
            kill(processo2, SIGCONT);
            }

    }

    //chiudo le pipe in lettura
    close(fd_pipe1[PIPE_RD]);
    close(fd_pipe2[PIPE_RD]);

    //attendo che la chiusura dei processi avvenga con successo, per evitare processi zombi
    waitpid(processo1, NULL, 0);
    waitpid(processo2, NULL, 0);

    return 0;

}
