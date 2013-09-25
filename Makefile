warmup2: warmup2.o th_packet.o  th_token.o th_server.o  utility.o
	gcc -o warmup2 -g warmup2.o th_packet.o th_token.o th_server.o utility.o -lpthread

warmup2.o: warmup2.c th_packet.h th_token.h th_server.h utility.h
	gcc -g -c -Wall warmup2.c th_packet.c th_token.c th_server.c utility.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

th_packet.o: th_packet.c th_packet.h utility.h
	gcc -g -c -Wall th_packet.c utility.c

th_token.o: th_token.c th_token.h utility.h
	gcc -g -c -Wall th_token.c utility.c

th_server.o: th_server.c th_server.h utility.h
	gcc -g -c -Wall th_server.c utility.c

utility.o: utility.c utility.h
	gcc -g -c -Wall utility.c

clean:
	rm -f *.o warmup2
