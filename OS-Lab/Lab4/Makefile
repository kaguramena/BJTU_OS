simulate: simulator.c
	gcc -o $@ $<

test: simulate
	./simulate BestMatch > BestMatch.txt
	./simulate FirstMatch > FirstMatch.txt