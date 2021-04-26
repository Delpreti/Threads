CFLAGS = -Wall -Wextra -pedantic
LDLIBS = -lpthread
OBJS = sha256.o threadmine.o
TEST_OBJS = sha256.o test_sha.o

threadmine: $(OBJS)
	$(CC) -o threadmine $(OBJS) $(LDLIBS)

sha256.o:
threadmine.o:
test_sha.o:

.PHONY: clean
clean:
	rm -f $(OBJS) $(TEST_OBJS) threadmine test_sha

test: $(TEST_OBJS)
	$(CC) -o test_sha $(TEST_OBJS)
	./test_sha
