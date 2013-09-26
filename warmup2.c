#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "th_packet.h"
#include "th_token.h"
#include "th_server.h"
#include "utility.h"

void print_input(char *lambda, char* mu, char *FILENAME,
                 char *r, char *B, char *P, char *n);
void create_packet_thread(char *lambda, char* mu,
                    char *FILENAME, char *r,
                    char *B, char *P, char *n);
void print_stats();


/* Initialize mutex */

int main(int argc, char *argv[])
{
    /* Packet Thread */
    /* Initialize the mutex */
    pthread_mutex_init(&m, 0);

    int i;
    FILE *fp ;
    char buffer[1024];
    int line = 0;
    int result, inter_time, token, service_time, num;
    char *lambda = "2";
    char *mu = "4";
    char *r = "4";
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
    /* Create packet thread */

    /* Initialize the time */ 
    gettimeofday(&START_TIMEVAL, NULL);

    fprintf(stdout, "\n");
    print_emulation_time();
    fprintf(stdout, "emulation begins\n");

    create_packet_thread(lambda, mu, FILENAME, r, B, P, n);

    /* Print statistics */
    print_stats();
    return(0);
}

void create_packet_thread(char *lambda, char* mu, 
                    char *FILENAME, char *r, 
                    char *B, char *P, char *n)
{
    /* Thread creation */
    MyPacketData *pd = malloc(sizeof(MyPacketData));
    pd->FILENAME = FILENAME;
    pd->lambda = lambda;
    pd->mu = mu;
    pd->r = r;
    pd->B = B;
    pd->P = P;
    pd->n = n;
   
    pthread_t packet, token, server;
    
    /* Create packet thread */
    pthread_create(&packet, 0, packet_init, (void *)pd);
    /* Create Token thread */
    pthread_create(&token, 0, token_init, (void *)pd);
    /* Create Server thread */
    pthread_create(&server, 0, server_init, (void *)pd);
   
    pthread_join(packet, 0);
    pthread_join(token, 0);
    pthread_join(server, 0);
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

void print_stats()
{
    fprintf(stdout, "\nStatistics:\n\n");
    fprintf(stdout, "%4saverage packet inter-arrival time = \n", "");
    fprintf(stdout, "%4saverage packet service time = \n\n", ""); 

    fprintf(stdout, "%4saverage number of packets in Q1 = \n", ""); 
    fprintf(stdout, "%4saverage number of packets in Q2 = \n", ""); 
    fprintf(stdout, "%4saverage number of packets at S = \n\n", ""); 

    fprintf(stdout, "%4saverage time a packet spent in system = \n", ""); 
    fprintf(stdout, "%4sstandard deviation for time spent in system = \n\n", ""); 

    fprintf(stdout, "%4stoken drop probability = \n", ""); 
    fprintf(stdout, "%4spacket drop probability = \n", ""); 
}
