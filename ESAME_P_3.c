#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

//Il primo processo figlio genera numero da 0 a 100;
//Il padre lo legge e lo moltiplica per k casuale;
//Il secondo processo legge il numero e lo stampa;

//Standard per pipes
#define PIPE_RD 0
#define PIPE_WR 1


int main(){

    pid_t p1; //processo 1
    pid_t p2; //processo 2

    //file descriptors per le pipes dei processi
    int fd_pipe1[2];
    int fd_pipe2[2];

    //inizializzo pipes
    pipe(fd_pipe1);
    pipe(fd_pipe2);


    //=================
    // PROCESSO 1
    // ================
    p1 = fork();
    if(p1 < 0){
        fprintf(stderr,"Errore creazione figlio 1! \n");
        exit(EXIT_FAILURE);
    }
    else if (p1 == 0){
        //chiudo le pipe del secondo processo, non mi servono
        close(fd_pipe2[PIPE_RD]);
        close(fd_pipe2[PIPE_WR]);
        //chiudo anche quella in lettura, non mi serve devo scrivere
        close(fd_pipe1[PIPE_RD]);

        while(1){
            int num = (rand() % 100); //genero
            write(fd_pipe1[PIPE_WR], &num, sizeof(num)); //scrivo
            fprintf(stdout, "Inviato %d dal P1\n", num);
            sleep(3); //sleep per debug
        }


    }

    //=================
    // PROCESSO 2
    // ================
    p2 = fork();
    if(p2 < 0){
        fprintf(stderr, "Errore creazione figlio 2! \n");
        exit(EXIT_FAILURE);
    }
    else if(p2 == 0){
        //chiudo le pipe del primo processo, non mi servono
        close(fd_pipe1[PIPE_RD]);
        close(fd_pipe1[PIPE_WR]);
        //chiudo anche quella in scrittura, non mi serve devo leggere
        close(fd_pipe2[PIPE_WR]);

        while(1){
            int num_to_print;
            read(fd_pipe2[PIPE_RD], &num_to_print, sizeof(num_to_print)); //leggo
            printf("Numero letto dal P2: %d\n", num_to_print);

            sleep(3); //sleep per debug
        }

    }

    //=================
    // PROCESSO PADRE
    // ================

    //chiudo pipes inutili
    close(fd_pipe1[PIPE_WR]);
    close(fd_pipe2[PIPE_RD]);

    while(1){
        int k = (rand() * getpid()) % 100; //genereo
        int num_to_mult;
        read(fd_pipe1[PIPE_RD], &num_to_mult, sizeof(num_to_mult)); //leggo
        printf("Padre legge %d e lo moltiplica per %d\n",num_to_mult, k);
        num_to_mult = num_to_mult*k; //moltiplico
        write(fd_pipe2[PIPE_WR], &num_to_mult, sizeof(num_to_mult)); //mando a P2

        sleep(3); //sleep per debug

    }




}
