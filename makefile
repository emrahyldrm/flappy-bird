flappybird : flappy.c
	gcc -o $@ $^ -lncurses -O3

clean:
	rm -f flappybird
