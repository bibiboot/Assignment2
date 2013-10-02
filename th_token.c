#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "th_token.h"

void* token_init(void *val)
{
    ACT_TKN.sa_handler = interrupt_token;
    sigaction(SIGUSR1, &ACT_TKN, NULL);
    unsigned long long rate = MICRO/atof(((MyPacketData*)val)->r);
    if (rate == 0)
        rate = 1;
    MAX_TOKEN = to_ll(((MyPacketData*)val)->B);
    
    token_engine(rate);

    //printf("TOKEN DEAD\n");
    return((void*)0);        
}

void print_token_arrived()
{
    gettimeofday(&TKN_BEFORE, NULL);

    /* Calculate arrival time for stats */
    struct timeval inter_timeval = diff_timeval(TKN_BEFORE, TKN_BEFORE_PREV);

    struct timeval etime = diff_timeval(TKN_BEFORE, START_TIMEVAL);
    memcpy(&TKN_BEFORE_PREV, &TKN_BEFORE, sizeof(PKT_BEFORE));

    /* Print */
    fprintf(stdout,"%08llu.%03ldms: token t%llu arrives, token bucket now has %llu token, arrival-time = %llu.%03ldms\n",
                toMilliSeconds(etime), etime.tv_usec%MILLI, TOTAL, AVAILABLE,
                toMilliSeconds(inter_timeval), inter_timeval.tv_usec%MILLI);
}

void print_token_dropped()
{
    gettimeofday(&TKN_BEFORE, NULL);

    struct timeval etime = diff_timeval(TKN_BEFORE, START_TIMEVAL);
    memcpy(&TKN_BEFORE_PREV, &TKN_BEFORE, sizeof(PKT_BEFORE));

    fprintf(stdout,"%08llu.%03ldms: token t%llu arrives, dropped\n",
            toMilliSeconds(etime), etime.tv_usec%MILLI, TOTAL);
}


void endQ1_stats(struct timeval q1start, 
                 unsigned long long pack_num)
{
    /* Add statistics */
    struct timeval current;
    gettimeofday(&current, NULL);
    struct timeval q1end = diff_timeval(current, START_TIMEVAL);
    struct timeval q1duration = diff_timeval(q1end, q1start);
    TIME_AT_Q1 = add_timeval(TIME_AT_Q1, q1duration);

    fprintf(stdout,
            "%08llu.%03ldms: p%llu leaves Q1, time in Q1 = %llu.%03ldms, token bucket has %llu token\n",
            toMilliSeconds(q1end), q1end.tv_usec%1000, pack_num,
            toMilliSeconds(q1duration), q1duration.tv_usec%MILLI, AVAILABLE);
}


void token_engine(unsigned long long token_rate){

    My402ListElem *elem;
    My402dataElem *topEle;
    struct timeval etime, current;
    int broadcast;

    while(!(SERVER_DIE && My402ListEmpty(Q1))){

        broadcast = 0;

        /* Calculate the actual sleep time. */
        unsigned long long actual_inter_time = time_to_sleep(token_rate, TKN_BEFORE);
        /* Sleep */
        usleep(actual_inter_time);
        /*LOCK*/
        //printf("TOKENXXXXXXXXXXXXXX\n");
        pthread_mutex_lock(&m);
          
        TOTAL++;

        if(AVAILABLE<MAX_TOKEN){
            AVAILABLE++;
            print_token_arrived();
        } else {
            DROPPED++;
            print_token_dropped();
        }

        /* Checl if move from Q1 to Q2 is allowed */
        if(!My402ListEmpty(Q1)){
            /* Get top of the Q1 */
            elem = My402ListFirst(Q1);
            topEle = (My402dataElem*)(elem->obj);

            /* Check if their is enough token */
            if (topEle->token <= AVAILABLE){
                /* Remove it from Q1 */
                My402ListUnlink(Q1, elem);

                /* Decrement token */
                AVAILABLE-=topEle->token;

                /* Add stats */
                endQ1_stats(topEle->q1start, topEle->count);

                /* Check if Q2 is empty */
                if(My402ListEmpty(Q2))
                    broadcast = 1;

                gettimeofday(&current, NULL);
                etime = diff_timeval(current, START_TIMEVAL);

                /* Set start time in Q2 */
                topEle->q2start = etime;

                /* Add to Q2 */
                My402ListAppend(Q2, (void*)topEle);

                /* PRINT */
                fprintf(stdout, "%08llu.%03ldms: p%llu enters Q2\n",
                       toMilliSeconds(etime), etime.tv_usec%MILLI, topEle->count);

                if(broadcast == 1){
                    //printf("BROADCASTED\n");
                    //pthread_mutex_unlock(&m);
                    pthread_cond_broadcast(&cond_t);
                    //continue;
                }
            }
        }
        pthread_mutex_unlock(&m);
    }
}

void interrupt_token()
{
    //printf("<----------TOKEN DYING---------->\n"); 
    /* Check for mutex is locked or not */
    pthread_mutex_unlock(&m);
    pthread_exit(0);
}
