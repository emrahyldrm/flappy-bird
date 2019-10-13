flappybird : flappybird.cpp
	g++ -o $@ $^ -lncurses -O3

clean:
	rm -f flappybird
