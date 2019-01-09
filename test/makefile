include ../config

INCLUDE=`rokigl-config -I`
LIB=`rokigl-config -L`
LINK=`rokigl-config -l`

CC=gcc
CFLAGS=-ansi -Wall -O3 $(LIB) $(INCLUDE) -funroll-loops

%: %.c
	@$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	-@rm -f *.o *~ core *test
