#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "th_token.h"

void* token_init(void *val)
{
    ACT_TKN.sa_handler = interrupt_token;
    sigaction(SIGUSR1, &ACT_TKN, NULL);

    /* Handler for thread */
    double token_rate = atof(((MyPacketData*)val)->r);
    MAX_TOKEN = to_ll(((MyPacketData*)val)->B);
    token_engine(MICRO/token_rate);

    pthread_mutex_lock(&m);
    TOKEN_DEAD = 1;
    pthread_mutex_unlock(&m);

    return((void*)0);        
}

void token_engine(unsigned long long token_rate)
{
    unsigned long long count = 1;
    int broadcast = 0;
    struct timeval etime, current, q1end, q1duration;

    while(1){

    My402ListElem *topEle; 
    broadcast =0;

    /* Calculate the actual sleep time. */
    unsigned long long actual_inter_time = time_to_sleep(token_rate, TKN_BEFORE);
    /* Sleep */
    usleep(actual_inter_time);
    /* Get Current time */
    gettimeofday(&TKN_BEFORE, NULL);
    /* Calculate arrival time for stats */
    struct timeval inter_timeval = diff_timeval(TKN_BEFORE, TKN_BEFORE_PREV);
    /* Print */
    etime = diff_timeval(TKN_BEFORE, START_TIMEVAL);
    memcpy(&TKN_BEFORE_PREV, &TKN_BEFORE, sizeof(PKT_BEFORE));

    /*LOCK*/
    pthread_mutex_lock(&m);
    /* Check if possible to shift tokens */
    

    if(My402ListEmpty(Q1) && PACKET_DEAD == 1){
        pthread_mutex_unlock(&m);
        break;
     }


    TOTAL++;
    if(AVAILABLE<MAX_TOKEN){
        AVAILABLE++;
        fprintf(stdout,"%08llu.%03ldms: token t%llu arrives, token bucket now has %llu token, arrival-time = %llu.%03ldms\n",
                toMilliSeconds(etime), etime.tv_usec%MILLI, count++, AVAILABLE,
                toMilliSeconds(inter_timeval), inter_timeval.tv_usec%MILLI);
    }
    else{
        DROPPED++;
        fprintf(stdout,"%08llu.%03ldms: token t%llu arrives, dropped\n",
                toMilliSeconds(etime), etime.tv_usec%MILLI, count++);
    }

    if(My402ListEmpty(Q2)){
        broadcast = 1;
    }

    /* Check if Q1 is not empty */
    if(!My402ListEmpty(Q1)){

        /* Get top of the Q1 */
        topEle = My402ListFirst(Q1);
        My402dataElem *dataE = (My402dataElem*)(topEle->obj);

        /* Check if their is enough token */
        if (dataE->token <= AVAILABLE){
              /* Check if Q2 is empty */
              if(My402ListEmpty(Q2) == 0){
                 /* Broadcast server once the process is done */
                 broadcast = 1;
              }

              /* Add statistics */
              gettimeofday(&current, NULL);
              q1end = diff_timeval(current, START_TIMEVAL);
              q1duration = diff_timeval(q1end, dataE->q1start);
              TIME_AT_Q1 = add_timeval(TIME_AT_Q1, q1duration);

              /* Remove it from Q1 */
              My402ListUnlink(Q1, topEle);

              /* Decrement token */
              AVAILABLE-=dataE->token;

              /* PRINT */
              fprintf(stdout,
              "%08llu.%03ldms: p%llu leaves Q1, time in Q1 = %llu.%03ldms, token bucket has %llu token\n",
              toMilliSeconds(q1end), q1end.tv_usec%1000, dataE->count,
              toMilliSeconds(q1duration), q1duration.tv_usec%MILLI, AVAILABLE);


              /* Add to Q2 */
              /* Move from Q1 to Q2 */
              /* Add to statistics */
              gettimeofday(&current, NULL);
              etime = diff_timeval(current, START_TIMEVAL);

              /* Set start time in Q2 */
              dataE->q2start = etime;

              /* Add to Q2 */
              My402ListAppend(Q2, (void*)dataE);

              /* PRINT */
              fprintf(stdout, "%08llu.%03ldms: p%llu enters Q2\n",
              toMilliSeconds(etime), etime.tv_usec%MILLI, dataE->count);

              if(broadcast == 1){
                  /* If yes then broadcast signal to server */
                  /* Wake up server thread */
                  pthread_cond_broadcast(&cond_t);
              }

        }
    }
   pthread_mutex_unlock(&m);

  }
}

void interrupt_token()
{
    printf("------------------Token thread is terminating----------------\n"); 
    pthread_exit(0);
}
