include ./buildvars

ZY6000		= j1939
ZY6000_OBJS     = j1939ForZY6000.o

SENDER          = sender
RECVER          = recver
LOOP_OBJS       = sender.o recver.o
J1939           = test_lib.o queue.o lib.o pl.o dl.o al.o

all:distclean zy6000 loop

zy6000: tag $(ZY6000_OBJS) $(J1939) j1939ForZY6000.c j1939.dat
	$(LD) $(LFLAGS) $(J1939) $(ZY6000_OBJS) -o $(ZY6000)

loop: tag $(LOOP_OBJS) $(J1939)
	$(LD) $(LFLAGS) $(J1939) sender.o -o $(SENDER)
	$(LD) $(LFLAGS) $(J1939) recver.o -o $(RECVER)
	#./test.sh

tag:
	ctags -R
	cscope -Rbkq

$(ZY6000_OBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(LOOP_OBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(J1939):
	$(MK) $(MKFLAGS) src
	$(MV) $(MVFLAGS) src/*.o ./

clean:
	$(RM) $(RMFLAGS) cscope*
	$(RM) $(RMFLAGS) tags
	$(RM) $(RMFLAGS) *.a
	$(RM) $(RMFLAGS) *.o
	$(RM) $(RMFLAGS) $(SENDER) $(RECVER) $(ZY6000)
	$(RM) $(RMFLAGS) sender2recver.can

distclean:clean
	$(MK) $(MKFLAGS) src clean
