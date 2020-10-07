CC		= gcc
CFLAGS	= -Wall -std=c17
LINK	= -lm
OUTNAME	= keepitsafe
COMFLAG = -c -Wall
.PHONY = all clean

all: keepitsafe

SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%.o)
BINSTEST := $(filter-out $(EXE),$(BINS))

keepitsafe: ${BINS}
	$(CC) $(CFLAGS) ${BINS} $(LINK) -o $(OUTNAME)

%.o: %.c
	${CC} $(COMFLAG) $<

clean: 
	rm *.o