#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "th_packet.h"
#include "th_token.h"
#include "th_server.h"
#include "utility.h"

void print_input(char *lambda, char* mu, char *FILENAME,
                 char *r, char *B, char *P, char *n);
void create_packet_thread(char *lambda, char* mu,
                    char *FILENAME, char *r,
                    char *B, char *P, char *n);
void print_stats(struct timeval emulation_time);
void init_stats();
void block_signal();


/* Initialize mutex */

int main(int argc, char *argv[])
{
    /* Packet Thread */
    /* Initialize the mutex */
    pthread_mutex_init(&m, 0);

    int i;
    char *lambda = "0.5";
    char *mu = "0.35";
    char *r = "1.5";
    char *B = "10";
    char *P = "3";
    char *n = "20";
    char *FILENAME = NULL;
    AVAILABLE = 0;
    DROPPED = 0;
    DROPPED_PKT = 0;
    TOTAL = 0;
    struct timeval end;
    SERVER_DIE = 0;
    PACKET_DEAD = 0;
    TOKEN_DEAD = 0;

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
        fprintf(stderr, "Wrong command line argument\n");
        exit(0);
        break;
     }

    /*Allocate memory to list*/
    Q1 = malloc(sizeof(My402List));
    Q2 = malloc(sizeof(My402List));
    /*Initilialize the list*/
    My402ListInit(Q1);
    My402ListInit(Q2);

    /* Block Signal from Main thread */
    block_signal();
   
    print_input(lambda, mu, FILENAME, r, B, P, n);
    /* Create packet thread */

    fprintf(stdout, "\n");
    /* Initialize the stats */ 
    init_stats();

    struct timeval current = diff_timeval(START_TIMEVAL, PKT_BEFORE);
    fprintf(stdout, "%08llu.%03ldms: emulation begins\n",
            toMilliSeconds(current), current.tv_usec%MILLI);

    /* Create threads */
    create_packet_thread(lambda, mu, FILENAME, r, B, P, n);

    gettimeofday(&end, NULL);
    /* Find the emulation of the program */
    struct timeval emulation_time = diff_timeval(end, START_TIMEVAL);

    /* Print statistics */
    print_stats(emulation_time);

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
   
    /* Create packet thread */
    pthread_create(&PACKET, 0, packet_init, (void *)pd);
    /* Create Token thread */
    pthread_create(&TOKEN, 0, token_init, (void *)pd);
    /* Create Server thread */
    pthread_create(&SERVER, 0, server_init, (void *)pd);
   
    pthread_join(PACKET, 0);
    pthread_join(TOKEN, 0);
    pthread_join(SERVER, 0);

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

void print_stats(struct timeval emulation_time)
{
    fprintf(stdout, "\nStatistics:\n\n");
    if (NUM_PACKETS == 0)
        fprintf(stdout, 
               "%4saverage packet inter-arrival time = NA ( No packet arrived at this facility )\n", 
               "");
    else
        fprintf(stdout, "%4saverage packet inter-arrival time = %.6gsec\n", 
            "", (toSeconds(PKT_INTV_ARV_TIME)/NUM_PACKETS));

    if (NUM_PACKETS == 0)
        fprintf(stdout, 
               "%4saverage packet service time = NA ( No packet is still serviced )\n", 
               "");
    else
        fprintf(stdout, "%4saverage packet service time = %.6gsec\n\n", "", 
                toSeconds(SERVICE_TIME)/NUM_PACKETS);

    fprintf(stdout, "%4saverage number of packets in Q1 = %.6g\n", "",
            ((double)(toMicroSeconds(TIME_AT_Q1)))/(toMicroSeconds(emulation_time))); 
    fprintf(stdout, "%4saverage number of packets in Q2 = %.6g\n", "",
            ((double)(toMicroSeconds(TIME_AT_Q2)))/(toMicroSeconds(emulation_time))); 
    fprintf(stdout, "%4saverage number of packets at S = %.6g\n", "",
            ((double)(toMicroSeconds(TIME_AT_S)))/(toMicroSeconds(emulation_time))); 

    fprintf(stdout, "%4saverage time a packet spent in system = %.6gsec\n", "",
            toSeconds(SPENT)/NUM_PACKETS); 
    fprintf(stdout, "%4sstandard deviation for time spent in system = %.6gsec\n\n", 
            "", sqrt((SQ_SERVICE_TIME - toSeconds(SERVICE_TIME))/NUM_PACKETS)); 

    if(TOTAL == 0)
        fprintf(stdout, "%4stoken drop probability = NA ( No tokens were produced )", ""); 
    else
        fprintf(stdout, "%4stoken drop probability = %.6g\n", "", (double)DROPPED/TOTAL); 

    if(NUM_PACKETS == 0)
        fprintf(stdout, "%4spacket drop probability = NA ( No packets were produced )", "" ); 
    else 
        fprintf(stdout, "%4spacket drop probability = %.6g\n", "", (double)DROPPED_PKT/NUM_PACKETS); 
}

void init_stats(){
    /* Initialize the global stats variables */
    gettimeofday(&START_TIMEVAL, NULL);
    memcpy (&PKT_BEFORE, &START_TIMEVAL, sizeof (PKT_BEFORE));
    memcpy (&PKT_BEFORE_PREV, &START_TIMEVAL, sizeof (PKT_BEFORE));
    memcpy (&TKN_BEFORE, &START_TIMEVAL, sizeof (PKT_BEFORE));
    memcpy (&TKN_BEFORE_PREV, &START_TIMEVAL, sizeof (PKT_BEFORE));
}

void block_signal()
{
    sigemptyset(&NEW);
    sigaddset(&NEW, SIGINT);

    pthread_sigmask(SIG_BLOCK, &NEW, NULL);
}
