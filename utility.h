#include <pthread.h>
#include <unistd.h>

//pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m;

typedef struct packet_data {
    char *lambda;
    char *mu ;
    char *r ;
    char *B ;
    char *P ;
    char *n ;
    char *FILENAME ;
} MyPacketData;

unsigned long long to_ll(char *temp);

unsigned long long sec_to_microsec(char* sec);

unsigned long long millisec_to_microsec(char* millisec);

unsigned long long millisec_llto_microsec(unsigned long long millisec);

int check_format(int result,
                 int line,
                 unsigned long long inter_time,
                 unsigned long long token,
                 unsigned long long service_time);
