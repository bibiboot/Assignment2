#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

unsigned long long to_ll(char *temp)
{
    /* Convert char pointer to long long */
    unsigned long long value = atoll(temp);
    return value;
}

unsigned long long sec_to_microsec(char* sec)
{
    /* Convert sec to microsec */
    unsigned long long micro = to_ll(sec)*MICRO;
    return micro;
}

unsigned long long millisec_to_microsec(char* millisec)
{
    /* Convert millisec to microsec */
    unsigned long long micro = to_ll(millisec)*MILLI;
    return micro;
}

unsigned long long millisec_llto_microsec(unsigned long long millisec)
{
    /* Convert millisec to microsec */
    unsigned long long micro = millisec*MILLI;
    return micro;
}

int check_format(int result,
                 int line,
                 unsigned long long inter_time,
                 unsigned long long token, 
                 unsigned long long service_time)
{
    /* Check that first line is one integer and others are three */
    if( !((result == 3 && line!=0) || (result == 1 && line == 0)))
        return -1;
    /* Check if the inter_time, token and service_time are integers */

    return 0;
}

struct timeval add_timeval(struct timeval a,
                           struct timeval b)
{
    /* Addition of two time vals */
    a.tv_sec = a.tv_sec + b.tv_sec;
    a.tv_usec = a.tv_usec + b.tv_usec;
    a.tv_sec = a.tv_sec + a.tv_usec/MICRO;
    a.tv_usec = a.tv_usec%MICRO;
    return a;
}
    
struct timeval diff_timeval(struct timeval a,
                           struct timeval b)
{
    /* Subtraction of two time vals */
    /* a is greater then b */
    long long int interval = (a.tv_sec*MICRO+a.tv_usec) - (b.tv_sec*MICRO+b.tv_usec);
    a.tv_sec = interval/MICRO;
    a.tv_usec = interval%MICRO;
    return a;
}

unsigned long long toMilliSeconds(struct timeval a)
{
    return a.tv_sec*1000 + (a.tv_usec/1000);
}

struct timeval print_emulation_time()
{
    struct timeval current;
    gettimeofday(&current, NULL);
    struct timeval etime = diff_timeval(current, START_TIMEVAL);
    fprintf(stdout, "%08llu.%03ldms: ", toMilliSeconds(etime), etime.tv_usec%1000); 
    return etime;
}
