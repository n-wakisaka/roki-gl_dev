include ../../config

INCLUDE=`rokigl-config -I`
LIB=`rokigl-config -L`
LINK=`rokigl-config -l`

ifeq ($(CONFIG_USE_GLUT),y)
	LINK += -lglut
endif

CC=gcc
CFLAGS=-ansi -Wall -O3 $(LIB) $(INCLUDE) -funroll-loops -g

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	rm -f *.o *~ core *test
