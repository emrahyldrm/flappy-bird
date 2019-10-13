flappy.gnu : flappy.c
	gcc -o $@ $^ -lncurses -O3

clean:
	rm -f flappy.gnu
