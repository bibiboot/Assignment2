
void* packet_init(void *val);

void deter_packet(unsigned long long inter_time, 
                  unsigned long long token, 
                  unsigned long long service_time,
                  unsigned long long num);

void trace_packet(char *FILENAME, 
                  unsigned long long B);

void packet_engine(unsigned long long inter_time, 
                   unsigned long long token, 
                   unsigned long long service_time,
                   unsigned long long count);
