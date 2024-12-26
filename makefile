CC = g++
CFLAGS = -g -Wall -pthread

all: dineseating

dineseating: main.o producer.o consumer.o log.o
	$(CC) $(CFLAGS) -o dineseating main.o producer.o consumer.o log.o

main.o: main.cpp producer.h consumer.h seating.h log.h
	$(CC) $(CFLAGS) -c main.cpp

producer.o: producer.cpp producer.h seating.h log.h
	$(CC) $(CFLAGS) -c producer.cpp

consumer.o: consumer.cpp consumer.h seating.h log.h
	$(CC) $(CFLAGS) -c consumer.cpp

log.o: log.cpp log.h seating.h
	$(CC) $(CFLAGS) -c log.cpp

clean:
	rm -f *.o dineseating
