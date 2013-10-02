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
void init_stats();
void block_signal();
void unblock_SIGNAL();
void interrupt_main();

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
    TOTAL_SERVED = 0;
    SERVER_DIE = 0;

    /* Read Options */
    for(i=1;i<argc;i=i+2){
        if (i%2!=0 && argv[i][0]=='-'){
            if ((strcmp(argv[i]+1, "lambda") == 0) && ((i+1)<argc)){
                lambda = argv[i+1];
                if(check_num(lambda)== -1){
                    fprintf(stderr, "Value of lambda is not a number.\n");
                    exit(0);
                }
                continue;
            }
            else if ((strcmp(argv[i]+1, "mu") == 0) && ((i+1)<argc)){
                mu = argv[i+1];
                if(check_num(mu)== -1){
                    fprintf(stderr, "Value of mu is not a number.\n");
                    exit(0);
                }
                continue;
            }
            else if ((strcmp(argv[i]+1, "r") == 0) && ((i+1)<argc)){
                r = argv[i+1];
                if(check_num(r)== -1){
                    fprintf(stderr, "Value of r is not a number.\n");
                    exit(0);
                }
                continue;
            }
            else if ((strcmp(argv[i]+1, "B") == 0) && ((i+1)<argc)){
                B = argv[i+1];
                if(isNum(B)==-1){
                    fprintf(stderr, "Value of B is not a number.\n");
                    exit(0);
                }
                continue;
            }
            else if((strcmp(argv[i]+1, "P") == 0) && ((i+1)<argc)){
                P = argv[i+1];
                if(isNum(P) == -1){
                    fprintf(stderr, "Value of P is not a number.\n");
                    exit(0);
                }
                continue;
            }
            else if ((strcmp(argv[i]+1, "n") == 0) && ((i+1)<argc)){
                n = argv[i+1];
                if(isNum(n)==-1){
                    fprintf(stderr, "Value of n is not a number.\n");
                    exit(0);
                }
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

    if(FILENAME!=NULL){
        FILE *fp = fopen(FILENAME, "r");
        if(fp==NULL){
            perror("Error: Unable to open the file ");
            exit(0);
        }
        fclose(fp);
    }

   
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

    /* Print statistics */
    //print_stats();

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

    unblock_SIGNAL();
   
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

void unblock_SIGNAL()
{
    ACT.sa_handler = interrupt_main;
    sigaction(SIGINT, &ACT, NULL);
    pthread_sigmask(SIG_UNBLOCK, &NEW, NULL);
}

void interrupt_main()
{
    //printf("<-----CTR+C RECIEVED------>\n");
    SERVER_DIE = 1;
    pthread_kill(TOKEN, SIGUSR1);
    pthread_kill(PACKET, SIGUSR2);

    pthread_mutex_lock(&m);
    //printf("Inside mutex of Man\n");
    while(!My402ListEmpty(Q2)){
        My402ListElem *elem = My402ListFirst(Q2);
        My402dataElem *topEle = (My402dataElem*)(elem->obj);
        My402ListUnlink(Q2, elem);

        TIME_AT_Q1 = diff_timeval(TIME_AT_Q1, topEle->q1duration);
    }
    /* Clear the list */
    My402ListUnlinkAll(Q1);
    pthread_mutex_unlock(&m);

    pthread_cond_broadcast(&cond_t);

    //printf("<-----MAIN DYING------>\n");
    pthread_exit(0);
}
