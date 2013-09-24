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
    unsigned long long micro = to_ll(sec)*1000000;
    return micro;
}

unsigned long long millisec_to_microsec(char* millisec)
{
    /* Convert millisec to microsec */
    unsigned long long micro = to_ll(millisec)*1000;
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
