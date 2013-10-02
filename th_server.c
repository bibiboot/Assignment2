#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include "th_server.h"

void server_engine();

void* server_init(void *val)
{
    server_engine();
    //printf("<-----SERVER DYING----->\n");
    print_stats();
    exit(0);
    return((void*)0);        
}

struct timeval endQ2_stats(struct timeval q2start, unsigned long long pack_num){
     struct timeval current;
    /* Add to statistics */
    gettimeofday(&current, NULL);
    struct timeval q2end = diff_timeval(current, START_TIMEVAL);
    struct timeval q2duration = diff_timeval(q2end, q2start);
    TIME_AT_Q2 = add_timeval(TIME_AT_Q2, q2duration);

    /* PRINT */
    fprintf(stdout,
            "%08llu.%03ldms: p%llu begin service at S, time in Q2 = %llu.%03ldms\n",
              toMilliSeconds(q2end), q2end.tv_usec%MILLI, pack_num,
              toMilliSeconds(q2duration), q2duration.tv_usec%MILLI);

    return q2end;
}

void end_service_stats(struct timeval q2end, 
                       struct timeval q1start, 
                       unsigned long long pack_num){
    /* Add to statistics */
    struct timeval end;
    gettimeofday(&end, NULL);

    struct timeval send = diff_timeval(end, START_TIMEVAL);
    struct timeval sduration= diff_timeval(send, q1start);

    SPENT = add_timeval(SPENT, sduration);
    struct timeval serv_duration= diff_timeval(send, q2end);

    TIME_AT_S = add_timeval(TIME_AT_S, serv_duration);
    SERVICE_TIME = add_timeval(SERVICE_TIME, serv_duration);
    SQ_SPENT_TIME = SQ_SPENT_TIME + square(toSeconds(sduration));
    

    /* PRINT */
    fprintf(stdout,
              "%08llu.%03ldms: p%llu departs from S, service time = %llu.%03ldms, time in system = %llu.%03ldms\n",
              toMilliSeconds(send), send.tv_usec%MILLI, pack_num,
              toMilliSeconds(serv_duration), serv_duration.tv_usec%MILLI,
              toMilliSeconds(sduration), sduration.tv_usec%MILLI );

}

void server_engine()
{
    My402dataElem *topEle;
    struct timeval q2end;

    while(!(SERVER_DIE && My402ListEmpty(Q1) && My402ListEmpty(Q2))){
        
        pthread_mutex_lock(&m);
        if(My402ListEmpty(Q2)){
            pthread_cond_wait(&cond_t, &m);
        }
        
        if(!My402ListEmpty(Q2)){
            /* Get top element */
            My402ListElem *elem = My402ListFirst(Q2);
            topEle = (My402dataElem*)(elem->obj);
            My402ListUnlink(Q2, elem);

            q2end = endQ2_stats(topEle->q2start, topEle->count);

            pthread_mutex_unlock(&m);

            /* Sleep for packets service time */
            usleep(topEle->service_time);

            end_service_stats(q2end, topEle->q1start, topEle->count);
            TOTAL_SERVED++;

        }
    }
}
