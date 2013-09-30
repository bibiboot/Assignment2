#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include "th_server.h"

void server_engine();

void* server_init(void *val)
{
    server_engine();
    return((void*)0);        
}

void server_engine()
{
    struct timeval current, q2end, q2duration, send, sduration, end, serv_duration;
    My402dataElem *topEle;

    while(1){
        if (SERVER_DIE == 1)
            break;
        
        pthread_mutex_lock(&m);
        if(PACKET_DEAD == 0)
            pthread_cond_wait(&cond_t, &m);

        /* Working code for packet goes here */
        /* If Q2 is empty no point */
        if(My402ListEmpty(Q2) && TOKEN_DEAD == 0){
            pthread_mutex_unlock(&m);
            continue;
         }
        else if(My402ListEmpty(Q2) && TOKEN_DEAD == 1){
            pthread_mutex_unlock(&m);
            break;
        }
 
        /* Move from Q2 to service */

        /* Get top element */
        My402ListElem *elem = My402ListFirst(Q2);
        topEle = (My402dataElem*)(elem->obj);
        My402ListUnlink(Q2, elem);

        /* Add to statistics */
        gettimeofday(&current, NULL);
        q2end = diff_timeval(current, START_TIMEVAL);
        q2duration = diff_timeval(q2end, topEle->q2start);
        TIME_AT_Q2 = add_timeval(TIME_AT_Q2, q2duration);

        /* PRINT */
        fprintf(stdout,
              "%08llu.%03ldms: p%llu begin service at S, time in Q2 = %llu.%03ldms\n",
              toMilliSeconds(q2end), q2end.tv_usec%MILLI, topEle->count,
              toMilliSeconds(q2duration), q2duration.tv_usec%MILLI);
        pthread_mutex_unlock(&m);

        /* Sleep for packets service time */
        usleep(topEle->service_time);

        /* Add to statistics */
        gettimeofday(&end, NULL);
        send = diff_timeval(end, START_TIMEVAL);
        sduration= diff_timeval(send, topEle->q1start);
        SPENT = add_timeval(SPENT, sduration);
        serv_duration= diff_timeval(send, q2end);
        TIME_AT_S = add_timeval(TIME_AT_S, serv_duration);
        SERVICE_TIME = add_timeval(SERVICE_TIME, serv_duration);

        /* PRINT */
        fprintf(stdout,
              "%08llu.%03ldms: p%llu departs from S, service time = %llu.%03ldms, time in system = %llu.%03ldms\n",
              toMilliSeconds(send), send.tv_usec%MILLI, topEle->count,
              toMilliSeconds(serv_duration), serv_duration.tv_usec%MILLI,
              toMilliSeconds(sduration), sduration.tv_usec%MILLI );
    }
}
