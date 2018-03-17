#
# Makefile of hope
#
CFLAGS = -O
GDATA =	define.h macro.h parameter.h
CC = cc 
#CC = acc
#
# for hope
#
SRCS =	error.c hash.c help.c read_cct.c \
	print.c fsim.c lsim.c main.c read_faults.c \
	read_test.c stem.c structure.c time.c truth.c
OBJS =	error.o hash.o help.o read_cct.o \
	print.o fsim.o lsim.o main.o read_faults.o \
	read_test.o stem.o structure.o time.o truth.o


hope:	$(OBJS) $(GDATA)
	$(CC) $(CFLAGS) $(OBJS) -o hope

error.o: error.c $(GDATA)
	$(CC) $(CFLAGS) -c error.c

hash.o:	hash.c $(GDATA)
	$(CC) $(CFLAGS) -c hash.c

help.o:	help.c $(GDATA)
	$(CC) $(CFLAGS) -c help.c

read_cct.o: read_cct.c $(GDATA)
	$(CC) $(CFLAGS) -c read_cct.c

print.o: print.c $(GDATA)
	$(CC) $(CFLAGS) -c print.c

fsim.o:	fsim.c $(GDATA)
	$(CC) $(CFLAGS) -c fsim.c

lsim.o:	lsim.c $(GDATA)
	$(CC) $(CFLAGS) -c lsim.c

main.o:	main.c $(GDATA)
	$(CC) $(CFLAGS) -c main.c

read_faults.o:	read_faults.c $(GDATA)
	$(CC) $(CFLAGS) -c read_faults.c

read_test.o: read_faults.c $(GDATA)
	$(CC) $(CFLAGS) -c read_test.c

stem.o:	stem.c
	$(CC) $(CFLAGS) -c stem.c

structure.o: structure.c $(GDATA)
	$(CC) $(CFLAGS) -c structure.c

time.o:	time.c $(GDATA)
	$(CC) $(CFLAGS) -c time.c

truth.o: truth.c $(GDATA)
	$(CC) $(CFLAGS) -c truth.c



lint:
	lint $(SRCS)

clear:
	rm -f *.o

