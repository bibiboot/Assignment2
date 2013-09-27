#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define MICRO 1000000
#define MILLI 1000

struct timeval PKT_BEFORE, TKN_BEFORE, TKN_AFTER, TKN_BEFORE_PREV;
struct timeval PKT_AFTER;

struct timeval pkt_int_arv_time;
struct timeval pkt_service_time;
unsigned long long numQ1;
unsigned long long numQ2;
unsigned long long numS;
struct timeval pkt_system_spent_time;

pthread_mutex_t m;
struct timeval START_TIMEVAL;

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

unsigned long long toMicroSeconds(struct timeval a);

struct timeval micro_to_timeval(unsigned long long usec);

int check_format(int result,
                 int line,
                 unsigned long long inter_time,
                 unsigned long long token,
                 unsigned long long service_time);

struct timeval add_timeval(struct timeval a,
                           struct timeval b);

struct timeval diff_timeval(struct timeval a,
                           struct timeval b);

struct timeval print_emulation_time(int i, char *mesg);

unsigned long long time_to_sleep(int thread_type, 
                                 unsigned long long inter_time);

struct timeval copy_time_val(struct timeval source, struct timeval dest);

