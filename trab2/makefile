CFLAGS = -Wall -Wextra -pedantic
LDLIBS = -lpthread
OBJS = main.o rw.o
TEST_OBJS = rwtest.o rw.o

main: $(OBJS)
	$(CC) -o main $(OBJS) $(LDLIBS)

main.o:
rw.o:

.PHONY: clean
clean:
	rm -f $(OBJS) $(TEST_OBJS) main

test: $(TEST_OBJS)
	$(CC) -o test $(TEST_OBJS) $(LDLIBS)
	./test
