CFLAGS = -Wall -Wextra -pedantic
LDLIBS = -lpthread
OBJS = main.o rw.o

main: $(OBJS)
	$(CC) -o main $(OBJS) $(LDLIBS)

main.o:
rw.o:

.PHONY: clean
clean:
	rm -f $(OBJS) $(TEST_OBJS) main
