main: main.c tbt.o
	gcc -o $@ $^ $(CFLAGS)

test: run-tests
	./run-tests

run-tests: test.c tbt.o
	gcc -o run-tests test.c tbt.o

debug: clean
	@make CFLAGS=-g

clean:
	rm -f main *.o
