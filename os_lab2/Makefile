CC=gcc
CFLAGS=-Wall -Wextra

all: fcfs sjf priority rr priority_rr

fcfs: driver.c list.c schedulers.c cpu.c
	$(CC) $(CFLAGS) -o fcfs driver.c list.c schedulers.c cpu.c -DFCFS

sjf: driver.c list.c schedulers.c cpu.c
	$(CC) $(CFLAGS) -o sjf driver.c list.c schedulers.c cpu.c -DSJF

priority: driver.c list.c schedulers.c cpu.c
	$(CC) $(CFLAGS) -o priority driver.c list.c schedulers.c cpu.c -DPRIORITY

rr: driver.c list.c schedulers.c cpu.c
	$(CC) $(CFLAGS) -o rr driver.c list.c schedulers.c cpu.c -DRR

priority_rr: driver.c list.c schedulers.c cpu.c
	$(CC) $(CFLAGS) -o priority_rr driver.c list.c schedulers.c cpu.c -DPRIORITY_RR

clean:
	rm -f fcfs sjf priority rr priority_rr