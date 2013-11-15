include ../buildvars

OBJS = test_lib.o queue.o lib.o pl.o dl.o al.o

all:clean $(OBJS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(RMFLAGS) $(OBJS)

