warmup2: warmup2.o th_packet.o utility.o
	gcc -o warmup2 -g warmup2.o th_packet.o utility.o -lpthread

warmup2.o: warmup2.c th_packet.h utility.h
	gcc -g -c -Wall warmup2.c th_packet.c utility.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

th_packet.o: th_packet.c th_packet.h utility.h
	gcc -g -c -Wall th_packet.c utility.c

utility.o: utility.c utility.h
	gcc -g -c -Wall utility.c

clean:
	rm -f *.o warmup2
