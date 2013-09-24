#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "th_packet.h"
#include "string.h"

void print_input(char *lambda, char* mu, char *FILENAME,
                 char *r, char *B, char *P, char *n);
void create_packet_thread(char *lambda, char* mu,
                    char *FILENAME, char *r,
                    char *B, char *P, char *n);


int main(int argc, char *argv[])
{
    /* Packet Thread */
    int i;
    FILE *fp ;
    char buffer[1024];
    int line = 0;
    int result, inter_time, token, service_time, num;
    char *lambda = "2";
    char *mu = "4";
    char *r = "1";
    char *B = "10";
    char *P = "3";
    char *n = "20";
    char *FILENAME = NULL;

    /* Read Options */
    for(i=1;i<argc;i=i+2){
        if (i%2!=0 && argv[i][0]=='-'){
            if ((strcmp(argv[i]+1, "lambda") == 0) && ((i+1)<argc)){
                lambda = argv[i+1];
                continue;
            }
            else if ((strcmp(argv[i]+1, "mu") == 0) && ((i+1)<argc)){
                mu = argv[i+1];
                continue;
            }
            else if ((strcmp(argv[i]+1, "r") == 0) && ((i+1)<argc)){
                r = argv[i+1];
                continue;
            }
            else if ((strcmp(argv[i]+1, "B") == 0) && ((i+1)<argc)){
                B = argv[i+1];
                continue;
            }
            else if((strcmp(argv[i]+1, "P") == 0) && ((i+1)<argc)){
                P = argv[i+1];
                continue;
            }
            else if ((strcmp(argv[i]+1, "n") == 0) && ((i+1)<argc)){
                n = argv[i+1];
                continue;
            }
            else if ((strcmp(argv[i]+1, "t") == 0) && ((i+1)<argc)){
                FILENAME = argv[i+1];
                continue;
            }
        }
        i =1;
        break;
     }

     /* Incase of error in file */
     if (i==1){
        fprintf(stderr, "Wrong command line argument\n");
        exit(0);
     }

    print_input(lambda, mu, FILENAME, r, B, P, n);
    create_packet_thread(lambda, mu, FILENAME, r, B, P, n);

    return(0);
}

void create_packet_thread(char *lambda, char* mu, 
                    char *FILENAME, char *r, 
                    char *B, char *P, char *n)
{
    MyPacketData *pd = malloc(sizeof(MyPacketData));
    pd->lambda = lambda;
    pd->mu = mu;
    pd->FILENAME = FILENAME;
    pd->r = r;
    pd->B = B;
    pd->P = P;
    pd->n = n;
   
    pthread_t packet;
    
    /* Create packet thread */
    pthread_create(&packet, 0, packet_init, (void *)pd);
   
    pthread_join(packet, 0);
}

void print_input(char *lambda, char* mu, char *FILENAME,
                 char *r, char *B, char *P, char *n)
{
     fprintf(stdout, "Emulation Parameters:\n");
     if (FILENAME == NULL)
         fprintf(stdout, "%4slambda =  %s\n%4smu = %s\n", "",lambda, "",mu);
     fprintf(stdout, "%4sr = %s\n%4sB = %s\n", "", r, "", B);
     if (FILENAME == NULL)
         fprintf(stdout, "%4sP =  %s\n%4snumber to arrive = %s\n", "", P,"", n);
     else
         fprintf(stdout, "%4stsfile = %s\n", "",FILENAME);
}
