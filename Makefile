
CC     ?= gcc
CXX    ?= g++
CFLAGS += -Wall -O2 -D_GNU_SOURCE

SOURCE = \
		BrandmeisterDC.c settings.c logger.c scanner.c bm_api.c

all:		clean BrandmeisterDC

BrandmeisterDC:
		$(CC) $(SOURCE) $(CFLAGS) -o BrandmeisterDC

clean:
		$(RM) BrandmeisterDC *.o *.d *.bak *~
