#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

#define PIPE_RD 0
#define PIPE_WR 1

/*
 * Processo padre disabilita SIGINT, GENERA NUMERI CAUSALI DA 0 A 100 E LI INVIA AL FIGLIO
 *
 * Processo figlio apre la directory corrente conta quanti file ci sono e se numerofile == numeropadre stampaun messaggio ed esce dal ciclo
 *
 * una volta terminato ciclo, figlio termina padre e termina poi se stesso.
 */

 int main(int argc, char* argv[]){

     pid_t figlio;

     int pipe_fd[2];
     pipe(pipe_fd);

     figlio = fork();
     if(figlio < 0){
         fprintf(stderr, "Errore creazione processo figlio!\n");
         exit(-1);
     } else if(figlio == 0) {

         int i = 0;
         close(pipe_fd[PIPE_WR]);

         while(i < 1000){

            int file_count = 0;

            DIR* directory;
            struct dirent *dir;
            directory = opendir(".");

            if(directory){

                while((dir = readdir(directory)) != NULL){

                    if(dir->d_name[0] != '.'){
                        file_count++;
                    }

                }

                closedir(directory);

            }

            int num_padre;
            read(pipe_fd[PIPE_RD], &num_padre, sizeof(num_padre));

            if(num_padre == file_count){
                printf("NUMERO FILE == NUMERO PADRE!\n");
                int pid_padre = getppid();
                kill(pid_padre, SIGTERM);
                exit(0);
            }

            i++;


         }

         int pid_padre = getppid();
         kill(pid_padre, SIGTERM);
         exit(0);

     }


     signal(SIGINT, SIG_IGN);
     close(pipe_fd[PIPE_RD]);

     srand(time(NULL));

     while(1){

         int num_rand = rand() % 101;
         write(pipe_fd[PIPE_WR], &num_rand, sizeof(num_rand));

     }




 }
