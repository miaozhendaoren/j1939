include ./buildvars

SENDER          = sender
RECVER          = recver
OBJS            = sender.o recver.o
J1939           = test_lib.o queue.o lib.o pl.o dl.o al.o

all:distclean test

test: tag $(OBJS) $(J1939)
	$(LD) $(LFLAGS) $(J1939) sender.o -o $(SENDER)
	$(LD) $(LFLAGS) $(J1939) recver.o -o $(RECVER)
	#./test.sh

tag:
	ctags -R
	cscope -Rbkq

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(J1939):
	$(MK) $(MKFLAGS) src
	$(MV) $(MVFLAGS) src/*.o ./

clean:
	$(RM) $(RMFLAGS) cscope*
	$(RM) $(RMFLAGS) tags
	$(RM) $(RMFLAGS) *.a
	$(RM) $(RMFLAGS) *.o
	$(RM) $(RMFLAGS) $(SENDER) $(RECVER)
	$(RM) $(RMFLAGS) sender2recver.can

distclean:clean
	$(MK) $(MKFLAGS) src clean
